
#include "stdafx.h"
#include "Utils.h"

// Creates a compatible video format with a different subtype.
namespace MediaController
{
    HRESULT ConfigureAudioEncoding(IMFCaptureSource *pSource, IMFCaptureRecordSink *pRecord, REFGUID guidEncodingType)
    {
        IMFCollection *pAvailableTypes = NULL;
        IMFMediaType *pMediaType = NULL;
        IMFAttributes *pAttributes = NULL;

        // Configure the audio format for the recording sink.

        HRESULT hr = MFCreateAttributes(&pAttributes, 1);
        if (FAILED(hr))
        {
            goto done;
        }

        // Enumerate low latency media types
        hr = pAttributes->SetUINT32(MF_LOW_LATENCY, TRUE);
        if (FAILED(hr))
        {
            goto done;
        }


        // Get a list of encoded output formats that are supported by the encoder.
        hr = MFTranscodeGetAudioOutputAvailableTypes(guidEncodingType, MFT_ENUM_FLAG_ALL | MFT_ENUM_FLAG_SORTANDFILTER,
            pAttributes, &pAvailableTypes);
        if (FAILED(hr))
        {
            goto done;
        }

        // Pick the first format from the list.
        hr = GetCollectionObject(pAvailableTypes, 0, &pMediaType);
        if (FAILED(hr))
        {
            goto done;
        }

        // Connect the audio stream to the recording sink.
        DWORD dwSinkStreamIndex;
        hr = pRecord->AddStream((DWORD)MF_CAPTURE_ENGINE_PREFERRED_SOURCE_STREAM_FOR_AUDIO, pMediaType, NULL, &dwSinkStreamIndex);
        if (hr == MF_E_INVALIDSTREAMNUMBER)
        {
            //If an audio device is not present, allow video only recording
            hr = S_OK;
        }
    done:
        SafeRelease(&pAvailableTypes);
        SafeRelease(&pMediaType);
        SafeRelease(&pAttributes);
        return hr;
    }

    HRESULT ConfigureVideoEncoding(IMFCaptureSource *pSource, IMFCaptureRecordSink *pRecord, REFGUID guidEncodingType)
    {
        IMFMediaType *pMediaType = NULL;
        IMFMediaType *pMediaType2 = NULL;
        GUID guidSubType = GUID_NULL;

        // Configure the video format for the recording sink.
        HRESULT hr = pSource->GetCurrentDeviceMediaType((DWORD)MF_CAPTURE_ENGINE_PREFERRED_SOURCE_STREAM_FOR_VIDEO_RECORD, &pMediaType);
        if (FAILED(hr))
        {
            goto done;
        }

        hr = CloneVideoMediaType(pMediaType, guidEncodingType, &pMediaType2);
        if (FAILED(hr))
        {
            goto done;
        }


        hr = pMediaType->GetGUID(MF_MT_SUBTYPE, &guidSubType);
        if (FAILED(hr))
        {
            goto done;
        }

        if (guidSubType == MFVideoFormat_H264_ES || guidSubType == MFVideoFormat_H264)
        {
            //When the webcam supports H264_ES or H264, we just bypass the stream. The output from Capture engine shall be the same as the native type supported by the webcam
            hr = pMediaType2->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
        }
        else
        {
            UINT32 uiEncodingBitrate;
            hr = GetEncodingBitrate(pMediaType2, &uiEncodingBitrate);
            if (FAILED(hr))
            {
                goto done;
            }

            hr = pMediaType2->SetUINT32(MF_MT_AVG_BITRATE, uiEncodingBitrate);
        }

        if (FAILED(hr))
        {
            goto done;
        }

        // Connect the video stream to the recording sink.
        DWORD dwSinkStreamIndex;
        hr = pRecord->AddStream((DWORD)MF_CAPTURE_ENGINE_PREFERRED_SOURCE_STREAM_FOR_VIDEO_RECORD, pMediaType2, NULL, &dwSinkStreamIndex);

    done:
        SafeRelease(&pMediaType);
        SafeRelease(&pMediaType2);
        return hr;
    }

    HRESULT CopyAttribute(IMFAttributes *pSrc, IMFAttributes *pDest, const GUID& key)
    {
        PROPVARIANT var;
        PropVariantInit(&var);
        HRESULT hr = pSrc->GetItem(key, &var);
        if (SUCCEEDED(hr))
        {
            hr = pDest->SetItem(key, var);
            PropVariantClear(&var);
        }
        return hr;
    }

    HRESULT CloneVideoMediaType(IMFMediaType *pSrcMediaType, REFGUID guidSubType, IMFMediaType **ppNewMediaType)
    {
        IMFMediaType *pNewMediaType = NULL;

        HRESULT hr = MFCreateMediaType(&pNewMediaType);
        if (FAILED(hr))
        {
            goto done;
        }

        hr = pNewMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        if (FAILED(hr))
        {
            goto done;
        }

        hr = pNewMediaType->SetGUID(MF_MT_SUBTYPE, guidSubType);
        if (FAILED(hr))
        {
            goto done;
        }

        hr = CopyAttribute(pSrcMediaType, pNewMediaType, MF_MT_FRAME_SIZE);
        if (FAILED(hr))
        {
            goto done;
        }

        hr = CopyAttribute(pSrcMediaType, pNewMediaType, MF_MT_FRAME_RATE);
        if (FAILED(hr))
        {
            goto done;
        }

        hr = CopyAttribute(pSrcMediaType, pNewMediaType, MF_MT_PIXEL_ASPECT_RATIO);
        if (FAILED(hr))
        {
            goto done;
        }

        hr = CopyAttribute(pSrcMediaType, pNewMediaType, MF_MT_INTERLACE_MODE);
        if (FAILED(hr))
        {
            goto done;
        }

        *ppNewMediaType = pNewMediaType;
        (*ppNewMediaType)->AddRef();

    done:

        if (pNewMediaType != NULL)
        {
            pNewMediaType->Release();
            pNewMediaType = NULL;
        }

        return hr;
    }

    HRESULT GetEncodingBitrate(IMFMediaType *pMediaType, UINT32 *uiEncodingBitrate)
    {
        UINT32 uiWidth;
        UINT32 uiHeight;
        float uiBitrate;
        UINT32 uiFrameRateNum;
        UINT32 uiFrameRateDenom;

        HRESULT hr = GetFrameSize(pMediaType, &uiWidth, &uiHeight);
        if (FAILED(hr))
        {
            goto done;
        }

        hr = GetFrameRate(pMediaType, &uiFrameRateNum, &uiFrameRateDenom);
        if (FAILED(hr))
        {
            goto done;
        }

        uiBitrate = uiWidth / 3.0f * uiHeight * uiFrameRateNum / uiFrameRateDenom;

        *uiEncodingBitrate = (UINT32)uiBitrate;

    done:

        return hr;
    }
}

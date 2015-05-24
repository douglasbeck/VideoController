#pragma once

#include <new>
#include <windows.h>
#include <windowsx.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <mfcaptureengine.h>

namespace VideoController
{
    HRESULT CloneVideoMediaType(IMFMediaType *pSrcMediaType, REFGUID guidSubType, IMFMediaType **ppNewMediaType);
    HRESULT ConfigureAudioEncoding(IMFCaptureSource *pSource, IMFCaptureRecordSink *pRecord, REFGUID guidEncodingType);
    HRESULT ConfigureVideoEncoding(IMFCaptureSource *pSource, IMFCaptureRecordSink *pRecord, REFGUID guidEncodingType);
    HRESULT GetEncodingBitrate(IMFMediaType *pMediaType, UINT32 *uiEncodingBitrate);

    // Gets an interface pointer from a Media Foundation collection.
    template <class IFACE>
    HRESULT GetCollectionObject(IMFCollection *pCollection, DWORD index, IFACE **ppObject)
    {
        IUnknown *pUnk;
        HRESULT hr = pCollection->GetElement(index, &pUnk);
        if (SUCCEEDED(hr))
        {
            hr = pUnk->QueryInterface(IID_PPV_ARGS(ppObject));
            pUnk->Release();
        }
        return hr;
    }

    // Helper function to get the frame rate from a video media type.
    inline HRESULT GetFrameRate(
        IMFMediaType *pType,
        UINT32 *pNumerator,
        UINT32 *pDenominator
        )
    {
        return MFGetAttributeRatio(
            pType,
            MF_MT_FRAME_RATE,
            pNumerator,
            pDenominator
            );
    }

    // Helper function to get the frame size from a video media type.
    inline HRESULT GetFrameSize(IMFMediaType *pType, UINT32 *pWidth, UINT32 *pHeight)
    {
        return MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, pWidth, pHeight);
    }


    template <class T> void SafeRelease(T **ppT)
    {
        if (*ppT)
        {
            (*ppT)->Release();
            *ppT = NULL;
        }
    }

    template <class T> void IpSafeRelease(T* ipT)
    {
        if (ipT != nullptr)
        {
            ipT->Release();
            ipT = nullptr;
        }
    }
}


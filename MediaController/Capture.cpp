#include "stdafx.h"
#include <mfapi.h>
#include <mfidl.h>
#include "Capture.h"

namespace MediaController
{
    void Capture::CaptureDevices()
    {
        IMFAttributes*  pAttributes = NULL;
        IMFActivate**   ppDevices = NULL;
        UINT32          numDevices = 0;

        HRESULT hResult = MFCreateAttributes(&pAttributes, 1);

        if (FAILED(hResult))
        {
            goto done;
        }

        // Ask for source type = video capture devices
        hResult = pAttributes->SetGUID(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);

        if (FAILED(hResult))
        {
            goto done;
        }

        // Enumerate devices.
        hResult = MFEnumDeviceSources(pAttributes, &ppDevices, &numDevices);

        if (FAILED(hResult))
        {
            goto done;
        }

        for (UINT32 i = 0; i < numDevices; i++)
        {
            m_videoDevices->Add(gcnew VideoDevice(ppDevices[i]));
        }

    done:

        return;
    }

    HRESULT Capture::CreateD3DManager()
    {
        HRESULT                 hResult = S_OK;
        D3D_FEATURE_LEVEL       FeatureLevel;
        IMFDXGIDeviceManager*   pDXGIMan = NULL;
        ID3D11Device*           pDX11Device = NULL;
        ID3D11DeviceContext*    pDX11DeviceContext = NULL;
        UINT                    resetToken;

        hResult = CreateDX11Device(&pDX11Device, &pDX11DeviceContext, &FeatureLevel);

        if (SUCCEEDED(hResult))
        {
            hResult = MFCreateDXGIDeviceManager(&resetToken, &pDXGIMan);

            if (SUCCEEDED(hResult))
            {
                hResult = pDXGIMan->ResetDevice(pDX11Device, resetToken);

                if (SUCCEEDED(hResult))
                {
                    m_pDX11Device = pDX11Device;
                    m_pDXGIMan = pDXGIMan;
                    m_ResetToken = resetToken;
                }
            }
        }

        SafeRelease(&pDX11DeviceContext);

        if (FAILED(hResult))
        {
            SafeRelease(&pDXGIMan);
            SafeRelease(&pDX11Device);
        }

        return hResult;
    }

    HRESULT Capture::CreateDX11Device(_Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppDeviceContext, _Out_ D3D_FEATURE_LEVEL* pFeatureLevel)
    {
        HRESULT hr = S_OK;
        static const D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };


        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            D3D11_CREATE_DEVICE_VIDEO_SUPPORT,
            levels,
            ARRAYSIZE(levels),
            D3D11_SDK_VERSION,
            ppDevice,
            pFeatureLevel,
            ppDeviceContext
            );

        if (SUCCEEDED(hr))
        {
            ID3D10Multithread* pMultithread;
            hr = ((*ppDevice)->QueryInterface(IID_PPV_ARGS(&pMultithread)));

            if (SUCCEEDED(hr))
            {
                pMultithread->SetMultithreadProtected(TRUE);
            }

            SafeRelease(&pMultithread);

        }

        return hr;
    }

    HRESULT Capture::Initialize(VideoDevice^ videoDevice)
    {
        HRESULT                         hResult = S_OK;
        IMFAttributes*                  pAttributes = NULL;
        IMFCaptureEngineClassFactory*   pFactory = NULL;

        if (m_bInitialized)
        {
            return hResult;
        }

        m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

        if (NULL == m_hEvent)
        {
            hResult = HRESULT_FROM_WIN32(GetLastError());
            goto Exit;
        }

        //Create a D3D Manager
        hResult = CreateD3DManager();

        if (FAILED(hResult))
        {
            goto Exit;
        }

        hResult = MFCreateAttributes(&pAttributes, 1);

        if (FAILED(hResult))
        {
            goto Exit;
        }

        hResult = pAttributes->SetUnknown(MF_CAPTURE_ENGINE_D3D_MANAGER, m_pDXGIMan);

        if (FAILED(hResult))
        {
            goto Exit;
        }

        // Create the factory object for the capture engine.
        hResult = CoCreateInstance(
            CLSID_MFCaptureEngineClassFactory, 
            NULL,
            CLSCTX_INPROC_SERVER, 
            IID_PPV_ARGS(&pFactory));

        if (FAILED(hResult))
        {
            goto Exit;
        }

        // Create and initialize the capture engine.

        IMFCaptureEngine* pEngine;

        hResult = pFactory->CreateInstance(CLSID_MFCaptureEngine, IID_PPV_ARGS(&pEngine));

        if (FAILED(hResult))
        {
            goto Exit;
        }

        m_pEngine = pEngine;

        hResult = m_pEngine->Initialize(m_pOnEventCallback, pAttributes, NULL, videoDevice->GetVideoDevice());

        if (FAILED(hResult))
        {
            goto Exit;
        }

    Exit:

        if (SUCCEEDED(hResult))
        {
            m_bInitialized = true;
        }

        if (NULL != pAttributes)
        {
            pAttributes->Release();
            pAttributes = NULL;
        }

        if (NULL != pFactory)
        {
            pFactory->Release();
            pFactory = NULL;
        }

        return hResult;
    }

        void Capture::OnInitialized(HRESULT& hrStatus)
        {
            if (hrStatus == MF_E_NO_CAPTURE_DEVICES_AVAILABLE)
            {
                hrStatus = S_OK;  // No capture device. Not an application error.
            }
        }

    void Capture::OnPreviewStarted(HRESULT hResult)
    {
        m_bPreviewing = SUCCEEDED(hResult);
        m_captureEventSink->OnPreviewStarted(hResult);
    }

    void Capture::OnPreviewStopped(HRESULT hResult)
    {
        m_bPreviewing = false;
        m_captureEventSink->OnPreviewStopped(hResult);
    }

    void Capture::OnRecordStarted(HRESULT hResult)
    {
        m_bRecording = SUCCEEDED(hResult);
        m_captureEventSink->OnRecordStarted(hResult);
    }
    
    void Capture::OnRecordStopped(HRESULT hResult)
    {
        m_bRecording = false;
        SetEvent(m_hEvent);
        m_captureEventSink->OnRecordStopped(hResult);
    }

    HRESULT Capture::StartPreview(IntPtr^ viewerWindow)
    {
        IMFCaptureSink*     pSink = NULL;
        IMFMediaType*       pMediaType = NULL;
        IMFMediaType*       pMediaType2 = NULL;
        IMFCaptureSource*   pSource = NULL;
        HRESULT             hResult = S_OK;
        HWND                hwndViewerWindow;

        if (m_pEngine == NULL)
        {
            return MF_E_NOT_INITIALIZED;
        }

        if (m_bPreviewing == true)
        {
            return S_OK;
        }

        if (viewerWindow == nullptr)
        {
            return E_INVALIDARG;
        }

        hwndViewerWindow = (HWND)viewerWindow->ToPointer();

        // Get a pointer to the preview sink.
        if (m_pPreview == NULL)
        {
            hResult = m_pEngine->GetSink(MF_CAPTURE_ENGINE_SINK_TYPE_PREVIEW, &pSink);

            if (FAILED(hResult))
            {
                goto done;
            }

            IMFCapturePreviewSink* pPreview;

            hResult = pSink->QueryInterface(IID_PPV_ARGS(&pPreview));

            if (FAILED(hResult))
            {
                goto done;
            }

            m_pPreview = pPreview;

            hResult = m_pPreview->SetRenderHandle(hwndViewerWindow);

            if (FAILED(hResult))
            {
                goto done;
            }

            hResult = m_pEngine->GetSource(&pSource);

            if (FAILED(hResult))
            {
                goto done;
            }

            // Configure the video format for the preview sink.
            hResult = pSource->GetCurrentDeviceMediaType((DWORD)MF_CAPTURE_ENGINE_PREFERRED_SOURCE_STREAM_FOR_VIDEO_PREVIEW, &pMediaType);

            if (FAILED(hResult))
            {
                goto done;
            }

            hResult = CloneVideoMediaType(pMediaType, MFVideoFormat_RGB32, &pMediaType2);

            if (FAILED(hResult))
            {
                goto done;
            }

            hResult = pMediaType2->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);

            if (FAILED(hResult))
            {
                goto done;
            }

            // Connect the video stream to the preview sink.
            DWORD dwSinkStreamIndex;
            hResult = m_pPreview->AddStream((DWORD)MF_CAPTURE_ENGINE_PREFERRED_SOURCE_STREAM_FOR_VIDEO_PREVIEW, pMediaType2, NULL, &dwSinkStreamIndex);

            if (FAILED(hResult))
            {
                goto done;
            }
        }

        hResult = m_pEngine->StartPreview();

    done:
        SafeRelease(&pSink);
        SafeRelease(&pMediaType);
        SafeRelease(&pMediaType2);
        SafeRelease(&pSource);

        return hResult;
    }

    HRESULT Capture::StopPreview()
    {
        if (m_pEngine == NULL)
        {
            return MF_E_NOT_INITIALIZED;
        }

        if (!m_bPreviewing)
        {
            return S_OK;
        }

        return m_pEngine->StopPreview();
    }

    void Capture::UnInitialize()
    {
        if (NULL != m_hEvent)
        {
            CloseHandle(m_hEvent);
            m_hEvent = NULL;
        }

        if (m_pPreview != NULL)
        {
            m_pPreview->Release();
            m_pPreview = NULL;
        }

        if (m_pEngine != NULL)
        {
            m_pEngine->Release();
            m_pEngine = NULL;
        }

        if (m_pDXGIMan != NULL)
        {
            m_pDXGIMan->ResetDevice(m_pDX11Device, m_ResetToken);
        }

        if (m_pDX11Device != NULL)
        {
            m_pDX11Device->Release();
            m_pDX11Device = NULL;
        }

        if (m_pDXGIMan != NULL)
        {
            m_pDXGIMan->Release();
            m_pDXGIMan = NULL;
        }

        m_bInitialized  = false;
        m_bPreviewing   = false;
        m_bRecording    = false;
    }

    void Capture::StartContinuousRecord()
    {
        m_bContinouslyRecording = true;
        this->StartRecord();
        m_ContinuousRecordTimer->Start();
    }

    void Capture::StopContinuousRecord()
    {
        m_bContinouslyRecording = false;
        m_ContinuousRecordTimer->Stop();
        this->StopRecord();
    }

    HRESULT Capture::StartRecord()
    {
        if (m_pEngine == NULL)
        {
            return MF_E_NOT_INITIALIZED;
        }

        if (m_bRecording == true)
        {
            return MF_E_INVALIDREQUEST;
        }

        pin_ptr<const wchar_t> fileName = PtrToStringChars(m_FileManager->GetNextFile());

        PWSTR pszExt = PathFindExtension(fileName);

        GUID guidVideoEncoding;
        GUID guidAudioEncoding;

        if (wcscmp(pszExt, L".mp4") == 0)
        {
            guidVideoEncoding = MFVideoFormat_H264;
            guidAudioEncoding = MFAudioFormat_AAC;
        }
        else if (wcscmp(pszExt, L".wmv") == 0)
        {
            guidVideoEncoding = MFVideoFormat_WMV3;
            guidAudioEncoding = MFAudioFormat_WMAudioV9;
        }
        else if (wcscmp(pszExt, L".wma") == 0)
        {
            guidVideoEncoding = GUID_NULL;
            guidAudioEncoding = MFAudioFormat_WMAudioV9;
        }
        else
        {
            return MF_E_INVALIDMEDIATYPE;
        }

        IMFCaptureSink *pSink = NULL;
        IMFCaptureRecordSink *pRecord = NULL;
        IMFCaptureSource *pSource = NULL;

        HRESULT hr = m_pEngine->GetSink(MF_CAPTURE_ENGINE_SINK_TYPE_RECORD, &pSink);
        if (FAILED(hr))
        {
            goto done;
        }

        hr = pSink->QueryInterface(IID_PPV_ARGS(&pRecord));
        if (FAILED(hr))
        {
            goto done;
        }

        hr = m_pEngine->GetSource(&pSource);
        if (FAILED(hr))
        {
            goto done;
        }

        // Clear any existing streams from previous recordings.
        hr = pRecord->RemoveAllStreams();
        if (FAILED(hr))
        {
            goto done;
        }

        hr = pRecord->SetOutputFileName(fileName);
        if (FAILED(hr))
        {
            goto done;
        }

        // Configure the video and audio streams.
        if (guidVideoEncoding != GUID_NULL)
        {
            hr = ConfigureVideoEncoding(pSource, pRecord, guidVideoEncoding);
            if (FAILED(hr))
            {
                goto done;
            }
        }

        if (guidAudioEncoding != GUID_NULL)
        {
            hr = ConfigureAudioEncoding(pSource, pRecord, guidAudioEncoding);
            if (FAILED(hr))
            {
                goto done;
            }
        }

        hr = m_pEngine->StartRecord();
        if (FAILED(hr))
        {
            goto done;
        }

        m_bRecording = true;

    done:

        SafeRelease(&pSink);
        SafeRelease(&pSource);
        SafeRelease(&pRecord);

        return hr;
    }

    HRESULT Capture::StopRecord()
    {
        HRESULT hr = S_OK;

        if (m_bRecording)
        {
            hr = m_pEngine->StopRecord(TRUE, FALSE);
            WaitForResult();
        }

        return hr;
    }
}

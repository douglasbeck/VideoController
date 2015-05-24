#include "Stdafx.h"
#include <mfapi.h>
#include <mfidl.h>
#include <Mfcaptureengine.h>
#include <shlwapi.h>
#include "CaptureEngineOnEventCallback.h"
#include "Capture.h"

namespace VideoController
{
    STDMETHODIMP CaptureEngineOnEventCallback::QueryInterface(REFIID riid, void** ppv)
    {
        static const QITAB qit[] =
        {
            QITABENT(CaptureEngineOnEventCallback, IMFCaptureEngineOnEventCallback),
            { 0 }
        };
        return QISearch(this, qit, riid, ppv);
    }

    STDMETHODIMP_(ULONG) CaptureEngineOnEventCallback::AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }

    STDMETHODIMP_(ULONG) CaptureEngineOnEventCallback::Release()
    {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0)
        {
            delete this;
        }
        return cRef;
    }

    // Callback method to receive events from the capture engine.
    STDMETHODIMP CaptureEngineOnEventCallback::OnEvent(_In_ IMFMediaEvent* pEvent)
    {
        // We're about to fall asleep, that means we've just asked the CE to stop the preview
        // and record.  We need to handle it here since our message pump may be gone.
        GUID    guidType;

        HRESULT hrStatus = pEvent->GetStatus(&hrStatus);

        HRESULT hr = pEvent->GetExtendedType(&guidType);

        if (FAILED(hr))
        {
            return hr;
        }

        if (guidType == MF_CAPTURE_ENGINE_PREVIEW_STARTED)
        {
            m_Capture->OnPreviewStarted(hrStatus);
        }
        else if (guidType == MF_CAPTURE_ENGINE_PREVIEW_STOPPED)
        {
            m_Capture->OnPreviewStopped(hrStatus);
        }
        else if (guidType == MF_CAPTURE_ENGINE_RECORD_STARTED)
        {
            m_Capture->OnRecordStarted(hrStatus);
        }
        else if (guidType == MF_CAPTURE_ENGINE_RECORD_STOPPED)
        {
            m_Capture->OnRecordStopped(hrStatus);
        }

        return S_OK;
    }

}

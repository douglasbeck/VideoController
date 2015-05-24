#pragma once

#include <vcclr.h>

namespace MediaController
{
    ref class Capture;

    // The event callback object.
    class CaptureEngineOnEventCallback : public IMFCaptureEngineOnEventCallback
    {
    public:
        CaptureEngineOnEventCallback(Capture^ capture)
            : m_cRef(1),
            m_fSleeping(false),
            m_Capture(capture)
        {}

        // IUnknown
        STDMETHODIMP            QueryInterface(REFIID riid, void** ppv);
        STDMETHODIMP_(ULONG)    AddRef();
        STDMETHODIMP_(ULONG)    Release();

        // IMFCaptureEngineOnEventCallback
        STDMETHODIMP            OnEvent(_In_ IMFMediaEvent* pEvent);

    private:

        long                m_cRef;
        bool                m_fSleeping;
        gcroot<Capture^>    m_Capture;
    };
}

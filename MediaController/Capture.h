#pragma once

#ifndef UNICODE
#define UNICODE
#endif 

#if !defined( NTDDI_VERSION )
#define NTDDI_VERSION NTDDI_WIN8
#endif

#if !defined( _WIN32_WINNT )
#define _WIN32_WINNT _WIN32_WINNT_WIN8
#endif

#include "videodevice.h"
#include "ICaptureEventSink.h"
#include "FileManager.h"
#include <new>
#include <windows.h>
#include <windowsx.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <mfcaptureengine.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <commctrl.h>
#include <d3d11.h>
#include <vcclr.h>
#include "Utils.h"
#include "CaptureEngineOnEventCallback.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::Timers;
using namespace System::Windows;

namespace MediaController
{

const UINT WM_APP_CAPTURE_EVENT = WM_APP + 1;

public ref class Capture
{
    bool                            m_bPreviewing;
    bool                            m_bRecording;
    bool                            m_bInitialized;
    bool                            m_bContinouslyRecording;
    HANDLE                          m_hEvent;

    FileManager^			        m_FileManager;
    List<VideoDevice^>^             m_videoDevices;
    ICaptureEventSink^              m_captureEventSink;
    IMFCaptureEngine*               m_pEngine;
    IMFCapturePreviewSink*          m_pPreview;
    CaptureEngineOnEventCallback*   m_pOnEventCallback;
    Timer^                          m_ContinuousRecordTimer;
    const UInt32                    RecordLoopPeriod = 30000;
    const String^                   m_baseFileName = L"G:\\OfficeVideo";

    // DXGI DevManager support
    IMFDXGIDeviceManager*           m_pDXGIMan = NULL;
    ID3D11Device*                   m_pDX11Device = NULL;
    UINT                            m_ResetToken = 0;
    
public:

    Capture(ICaptureEventSink^ captureEventSink):
        m_bInitialized(false),
        m_bContinouslyRecording(false)
        {

        Debug::Print(L"Capture::Capture");

        m_FileManager           = gcnew FileManager(const_cast<String^>(m_baseFileName), 4);
        m_videoDevices          = gcnew List<VideoDevice^>();
        m_captureEventSink      = captureEventSink;
        m_pOnEventCallback      = new (std::nothrow) CaptureEngineOnEventCallback(this);
        m_ContinuousRecordTimer = gcnew Timer(RecordLoopPeriod);

        m_ContinuousRecordTimer->AutoReset = true;
        m_ContinuousRecordTimer->Elapsed += gcnew ElapsedEventHandler(this, &Capture::OnTimedEvent);
    }

    void            CaptureDevices();
    static HRESULT  CreateDX11Device(_Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppDeviceContext, _Out_ D3D_FEATURE_LEVEL* pFeatureLevel);
    HRESULT         CreateD3DManager();
    HRESULT         Initialize(VideoDevice^ videoDevice);
    virtual void    OnPreviewStarted(HRESULT hResult);
    virtual void    OnPreviewStopped(HRESULT hResult);
    void            OnRecordStarted(HRESULT hResult);
    void            OnRecordStopped(HRESULT hResult);
    HRESULT         StartPreview(IntPtr^ viewerWindow);
    HRESULT         StopPreview();
    void            StartContinuousRecord();
    void            StopContinuousRecord();
    HRESULT         StartRecord();
    HRESULT         StopRecord();
    void            UnInitialize();

    void OnTimedEvent(Object^ source, ElapsedEventArgs^ e)
    {
        this->StopRecord();
        this->StartRecord();
    }

    List<VideoDevice^>^ GetDevices()
    {
        return m_videoDevices;
    }

    void DestroyCaptureEngine()
    {
        if (NULL != m_hEvent)
        {
            CloseHandle(m_hEvent);
            m_hEvent = NULL;
        }

        IpSafeRelease(m_pPreview);
        IpSafeRelease(m_pEngine);
        IpSafeRelease(m_pOnEventCallback);

        if (m_pDXGIMan)
        {
            m_pDXGIMan->ResetDevice(m_pDX11Device, m_ResetToken);
        }

        IpSafeRelease(m_pDX11Device);
        IpSafeRelease(m_pDXGIMan);

        m_bPreviewing = false;
        m_bRecording = false;
    }

private:

    // Capture Engine Event Handlers
    void OnInitialized(HRESULT& hrStatus);

    void WaitForResult()
    {
        WaitForSingleObject(m_hEvent, INFINITE);
    }

public:

    ~Capture()
    {
        DestroyCaptureEngine();
    }
};

}

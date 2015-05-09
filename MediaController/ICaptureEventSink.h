#pragma once

namespace MediaController
{ 
    public interface class ICaptureEventSink
    {
        virtual void OnPreviewStarted(int hResult);
        virtual void OnPreviewStopped(int hResult);
        virtual void OnRecordStarted(int hResult);
        virtual void OnRecordStopped(int hResult);
    };
}

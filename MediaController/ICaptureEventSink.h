#pragma once

namespace MediaController
{ 
    public interface class ICaptureEventSink
    {
        virtual void OnPreviewCompleted();
    };
}

#pragma once

#include <mfidl.h>

using namespace System;
using namespace System::Diagnostics;

namespace MediaController
{
    public ref class VideoDevice
    {
    public:

        VideoDevice(IMFActivate* pDevice);
        String^ GetName() { return friendlyName; }
        IMFActivate* GetVideoDevice() { return pVideoDevice; }

        virtual String^ ToString() override
        {
            return friendlyName;
        }

    private:

        IMFActivate*    pVideoDevice = NULL;
        System::String^ friendlyName;
    };

}
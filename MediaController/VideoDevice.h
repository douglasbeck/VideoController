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

    private:

        IMFActivate*    pVideoDevice = NULL;
        System::String^ friendlyName;
    };

}
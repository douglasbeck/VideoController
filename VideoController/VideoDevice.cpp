#include "stdafx.h"
#include "VideoDevice.h"

namespace VideoController
{
    VideoDevice::VideoDevice(IMFActivate* pDevice)
    {
        WCHAR *szFriendlyName = NULL;
        UINT32 cchName;

        HRESULT hResult = pDevice->GetAllocatedString(
            MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
            &szFriendlyName, 
            &cchName);

        if (FAILED(hResult))
        {
            return;
        }

        friendlyName = gcnew String(szFriendlyName);
        pVideoDevice = pDevice;

        pVideoDevice->AddRef();
    }
}
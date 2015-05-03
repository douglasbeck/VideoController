#pragma once

#include <new>
#include <windows.h>
#include <windowsx.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <mfcaptureengine.h>

HRESULT CloneVideoMediaType(IMFMediaType *pSrcMediaType, REFGUID guidSubType, IMFMediaType **ppNewMediaType);

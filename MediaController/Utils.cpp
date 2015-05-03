
#include "stdafx.h"
#include "Utils.h"

// Creates a compatible video format with a different subtype.

HRESULT CopyAttribute(IMFAttributes *pSrc, IMFAttributes *pDest, const GUID& key)
{
    PROPVARIANT var;
    PropVariantInit(&var);
    HRESULT hr = pSrc->GetItem(key, &var);
    if (SUCCEEDED(hr))
    {
        hr = pDest->SetItem(key, var);
        PropVariantClear(&var);
    }
    return hr;
}

HRESULT CloneVideoMediaType(IMFMediaType *pSrcMediaType, REFGUID guidSubType, IMFMediaType **ppNewMediaType)
{
    IMFMediaType *pNewMediaType = NULL;

    HRESULT hr = MFCreateMediaType(&pNewMediaType);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pNewMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pNewMediaType->SetGUID(MF_MT_SUBTYPE, guidSubType);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = CopyAttribute(pSrcMediaType, pNewMediaType, MF_MT_FRAME_SIZE);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = CopyAttribute(pSrcMediaType, pNewMediaType, MF_MT_FRAME_RATE);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = CopyAttribute(pSrcMediaType, pNewMediaType, MF_MT_PIXEL_ASPECT_RATIO);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = CopyAttribute(pSrcMediaType, pNewMediaType, MF_MT_INTERLACE_MODE);
    if (FAILED(hr))
    {
        goto done;
    }

    *ppNewMediaType = pNewMediaType;
    (*ppNewMediaType)->AddRef();

done:

    if (pNewMediaType != NULL)
    {
        pNewMediaType->Release();
        pNewMediaType = NULL;
    }

    return hr;
}

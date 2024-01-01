#pragma once
#include "IGraphicsWnd.h"
#include "IGraphicsWndFactory.h"

#ifdef __cplusplus
extern "C" {
#endif

    HRESULT CreateIGraphicsWnd(const char* implName, IGraphicsWnd** res);

    HRESULT GetIGraphicsWndFactory(IGraphicsWndFactory** factory);
    HRESULT SetIGraphicsWndFactory(IGraphicsWndFactory* factory);

#ifdef __cplusplus
}
#endif

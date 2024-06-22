#pragma once
#include "IGraphicsWnd.h"
#include "IGraphicsWndFactory.h"

#ifdef __cplusplus
extern "C" {
#endif

    IRESULT CreateIGraphicsWnd(const char* implName, IGraphicsWnd** res);

    IRESULT GetIGraphicsWndFactory(IGraphicsWndFactory** factory);
    IRESULT SetIGraphicsWndFactory(IGraphicsWndFactory* factory);

#ifdef __cplusplus
}
#endif

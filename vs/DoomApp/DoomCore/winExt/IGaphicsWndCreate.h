#pragma once
#include "IGraphicsWnd.h"

#ifdef __cplusplus
extern "C" {
#endif

    HRESULT CreateIGraphicsWnd(const char* implName, IGraphicsWnd** res);

#ifdef __cplusplus
}
#endif
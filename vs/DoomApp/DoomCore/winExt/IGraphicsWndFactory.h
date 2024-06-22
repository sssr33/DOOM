#pragma once
#include "InterfaceBase.h"
#include "IGraphicsWnd.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct IGraphicsWndFactory IGraphicsWndFactory;

    struct IGraphicsWndFactoryVtbl {
        IRESULT(__stdcall* QueryInterface)(
            IGraphicsWndFactory* This,
            const IGUID* guid,
            void** ppvObject);

        int32_t(__stdcall* AddRef)(
            IGraphicsWndFactory* This);

        int32_t(__stdcall* Release)(
            IGraphicsWndFactory* This);

        IRESULT(__stdcall* CreateGraphicsWnd)(
            IGraphicsWndFactory* This, const char* implName, IGraphicsWnd** res);
    };

    struct IGraphicsWndFactory {
        const struct IGraphicsWndFactoryVtbl* vtable;
    };

#ifdef __cplusplus
}
#endif
#pragma once
#include "InterfaceBase.h"
#include "IGraphicsWnd.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct IGraphicsWndFactory IGraphicsWndFactory;

    struct IGraphicsWndFactoryVtbl {
        HRESULT(__stdcall* QueryInterface)(
            IGraphicsWndFactory* This,
            const IID* riid,
            void** ppvObject);

        ULONG(__stdcall* AddRef)(
            IGraphicsWndFactory* This);

        ULONG(__stdcall* Release)(
            IGraphicsWndFactory* This);

        HRESULT(__stdcall* CreateGraphicsWnd)(
            IGraphicsWndFactory* This, const char* implName, IGraphicsWnd** res);
    };

    struct IGraphicsWndFactory {
        const struct IGraphicsWndFactoryVtbl* vtable;
    };

#ifdef __cplusplus
}
#endif
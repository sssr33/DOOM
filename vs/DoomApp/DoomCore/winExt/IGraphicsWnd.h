#pragma once
#include "InterfaceBase.h"
#include "IGraphicsWndInputEvent.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct IGraphicsWnd IGraphicsWnd;

    struct IGraphicsWndVtbl {
        HRESULT(__stdcall* QueryInterface)(
            IGraphicsWnd* This,
            const IID* riid,
            void** ppvObject);

        HRESULT(__stdcall* AddRef)(
            IGraphicsWnd* This);

        HRESULT(__stdcall* Release)(
            IGraphicsWnd* This);

        HRESULT(__stdcall* TryGetNextInputEvent)(
            IGraphicsWnd* This,
            IGraphicsWndInputEvent** evt);
    };

    struct IGraphicsWnd {
        const struct IGraphicsWndVtbl* vtable;
    };

#ifdef __cplusplus
}
#endif
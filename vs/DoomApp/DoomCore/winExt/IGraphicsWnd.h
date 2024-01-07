#pragma once
#include "InterfaceBase.h"
#include "IGraphicsWndInputEvent.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct IGraphicsWnd IGraphicsWnd;

    struct IGraphicsWndVtbl {
        HRESULT(__stdcall* QueryInterface)(
            IGraphicsWnd* This,
            const IID* riid,
            void** ppvObject);

        ULONG(__stdcall* AddRef)(
            IGraphicsWnd* This);

        ULONG(__stdcall* Release)(
            IGraphicsWnd* This);

        HRESULT(__stdcall* InitializeScreen)(
            IGraphicsWnd* This,
            int width,
            int height,
            int bitDepth
            );

        HRESULT(__stdcall* GetCPUBackBuffer)(
            IGraphicsWnd* This,
            void** buffer
            );

        HRESULT(__stdcall* SetPalette)(
            IGraphicsWnd* This,
            const uint8_t* paletteIndexes,
            const uint8_t* paletteColors,
            uint32_t paletteSize
        );

        HRESULT(__stdcall* FinishScreenUpdate)(
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

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

        // paletteIndexes contains paletteSize * 3(rgb) bytes, so size of each paletteIndexes is 768 bytes(256 * 3)
        //                PLAYPAL lump contains all palettes and has size 10752 which is equals to 14 palettes as described in https://doomwiki.org/wiki/PLAYPAL
        // paletteColors contains paletteSize bytes. It contains color values that can be indexed by paletteIndexes
        //               r = paletteColors[paletteIndexes[0]]
        //               g = paletteColors[paletteIndexes[1]]
        //               b = paletteColors[paletteIndexes[2]]
        //               paletteIndexes += 3
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

#pragma once

#include <atomic>
#include <winExt\InterfacePtr.h>
#include <winExt\IGraphicsWnd.h>

class Dx11GraphicsWnd : public IGraphicsWnd {
public:
    static InterfacePtr<Dx11GraphicsWnd> Make();

    HRESULT __stdcall QueryInterface(
        const IID* riid,
        void** ppvObject);

    ULONG __stdcall AddRef();

    ULONG __stdcall Release();

    HRESULT __stdcall InitializeScreen(
        int width,
        int height,
        int bitDepth
        );

    HRESULT __stdcall GetCPUBackBuffer(
        void** buffer
        );

    HRESULT __stdcall FinishScreenUpdate();

    HRESULT __stdcall TryGetNextInputEvent(
        IGraphicsWndInputEvent** evt);

private:
    class VTable {
    public:
        static VTable& Instance();

        IGraphicsWndVtbl* GetVTable();

    private:
        VTable();

        IGraphicsWndVtbl vtable = {};
    };

    Dx11GraphicsWnd();

    std::atomic<ULONG> refCounter = 1;
};

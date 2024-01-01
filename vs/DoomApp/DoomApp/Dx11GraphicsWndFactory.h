#pragma once

#include <atomic>
#include <string>
#include <winExt\InterfacePtr.h>
#include <winExt\IGraphicsWndFactory.h>

class Dx11GraphicsWndFactory : public IGraphicsWndFactory {
public:
    static const std::string GraphicsImplName;

    static InterfacePtr<Dx11GraphicsWndFactory> Make();

    HRESULT __stdcall QueryInterface(
        const IID* riid,
        void** ppvObject);

    ULONG __stdcall AddRef();

    ULONG __stdcall Release();

    HRESULT __stdcall CreateGraphicsWnd(
        const char* implName, IGraphicsWnd** res);

private:
    class VTable {
    public:
        static VTable& Instance();

        IGraphicsWndFactoryVtbl* GetVTable();

    private:
        VTable();

        IGraphicsWndFactoryVtbl vtable = {};
    };

    Dx11GraphicsWndFactory();

    std::atomic<ULONG> refCounter = 1;
};

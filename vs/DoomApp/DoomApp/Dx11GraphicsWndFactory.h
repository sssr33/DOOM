#pragma once

#include <atomic>
#include <string>
#include <winExt\InterfacePtr.h>
#include <winExt\IGraphicsWndFactory.h>

class Dx11GraphicsWndFactory : public IGraphicsWndFactory {
public:
    static const std::string GraphicsImplName;

    static InterfacePtr<Dx11GraphicsWndFactory> Make();

    IRESULT __stdcall QueryInterface(
        const IGUID* guid,
        void** ppvObject);

    int32_t __stdcall AddRef();

    int32_t __stdcall Release();

    IRESULT __stdcall CreateGraphicsWnd(
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

    std::atomic<int32_t> refCounter = 1;
};

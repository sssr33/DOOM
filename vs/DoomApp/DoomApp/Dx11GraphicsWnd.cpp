#include "stdafx.h"
#include "Dx11GraphicsWnd.h"

InterfacePtr<Dx11GraphicsWnd> Dx11GraphicsWnd::Make() {
    return InterfacePtr<Dx11GraphicsWnd>(new Dx11GraphicsWnd, DoNotAddRefKey{});
}

HRESULT __stdcall Dx11GraphicsWnd::QueryInterface(
    const IID* riid,
    void** ppvObject)
{
    return E_NOINTERFACE;
}

ULONG __stdcall Dx11GraphicsWnd::AddRef() {
    return ++this->refCounter;
}

ULONG __stdcall Dx11GraphicsWnd::Release() {
    auto refc = --this->refCounter;

    if (refc == 0) {
        delete this;
    }

    return refc;
}

HRESULT __stdcall Dx11GraphicsWnd::InitializeScreen(
    int width,
    int height,
    int bitDepth
)
{
    return E_NOTIMPL;
}

HRESULT __stdcall Dx11GraphicsWnd::GetCPUBackBuffer(
    void** buffer
)
{
    return E_NOTIMPL;
}

HRESULT __stdcall Dx11GraphicsWnd::FinishScreenUpdate() {
    return E_NOTIMPL;
}

HRESULT __stdcall Dx11GraphicsWnd::TryGetNextInputEvent(
    IGraphicsWndInputEvent** evt)
{
    return E_NOTIMPL;
}

Dx11GraphicsWnd::Dx11GraphicsWnd() {
    this->vtable = VTable::Instance().GetVTable();
}

Dx11GraphicsWnd::VTable& Dx11GraphicsWnd::VTable::Instance() {
    static VTable instance;
    return instance;
}

IGraphicsWndVtbl* Dx11GraphicsWnd::VTable::GetVTable() {
    return &this->vtable;
}

Dx11GraphicsWnd::VTable::VTable() {
    this->vtable.QueryInterface = [](
        IGraphicsWnd* This,
        const IID* riid,
        void** ppvObject)
    {
        auto tmp = static_cast<Dx11GraphicsWnd*>(This);
        return tmp->QueryInterface(riid, ppvObject);
    };

    this->vtable.AddRef = [](IGraphicsWnd* This)
    {
        auto tmp = static_cast<Dx11GraphicsWnd*>(This);
        return tmp->AddRef();
    };

    this->vtable.Release = [](IGraphicsWnd* This)
    {
        auto tmp = static_cast<Dx11GraphicsWnd*>(This);
        return tmp->Release();
    };

    this->vtable.InitializeScreen = [](
        IGraphicsWnd* This,
        int width,
        int height,
        int bitDepth)
    {
        auto tmp = static_cast<Dx11GraphicsWnd*>(This);
        return tmp->InitializeScreen(width, height, bitDepth);
    };

    this->vtable.GetCPUBackBuffer = [](
        IGraphicsWnd* This,
        void** buffer)
    {
        auto tmp = static_cast<Dx11GraphicsWnd*>(This);
        return tmp->GetCPUBackBuffer(buffer);
    };

    this->vtable.FinishScreenUpdate = [](
        IGraphicsWnd* This)
    {
        auto tmp = static_cast<Dx11GraphicsWnd*>(This);
        return tmp->FinishScreenUpdate();
    };

    this->vtable.TryGetNextInputEvent = [](
        IGraphicsWnd* This,
        IGraphicsWndInputEvent** evt)
    {
        auto tmp = static_cast<Dx11GraphicsWnd*>(This);
        return tmp->TryGetNextInputEvent(evt);
    };
}

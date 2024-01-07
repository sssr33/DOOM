#include "stdafx.h"
#include "Dx11GraphicsWnd.h"
#include "vtable_bind.h"

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
    this->cpuVideoBuf.resize(width * height * (bitDepth / 8));

    return S_OK;
}

HRESULT __stdcall Dx11GraphicsWnd::GetCPUBackBuffer(
    void** buffer
)
{
    if (!buffer) {
        return E_POINTER;
    }

    *buffer = this->cpuVideoBuf.data();

    return S_OK;
}

HRESULT __stdcall Dx11GraphicsWnd::SetPalette(
    const uint8_t* paletteIndexes,
    const uint8_t* paletteColors,
    uint32_t paletteSize
)
{
    return S_OK;
}

HRESULT __stdcall Dx11GraphicsWnd::FinishScreenUpdate() {
    return S_OK;
}

HRESULT __stdcall Dx11GraphicsWnd::TryGetNextInputEvent(
    IGraphicsWndInputEvent** evt)
{
    return S_OK;
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
    vtable_bind_base<Dx11GraphicsWnd>(this->vtable);
    vtable_bind<&Dx11GraphicsWnd::InitializeScreen>(this->vtable.InitializeScreen);
    vtable_bind<&Dx11GraphicsWnd::GetCPUBackBuffer>(this->vtable.GetCPUBackBuffer);
    vtable_bind<&Dx11GraphicsWnd::SetPalette>(this->vtable.SetPalette);
    vtable_bind<&Dx11GraphicsWnd::FinishScreenUpdate>(this->vtable.FinishScreenUpdate);
    vtable_bind<&Dx11GraphicsWnd::TryGetNextInputEvent>(this->vtable.TryGetNextInputEvent);
}

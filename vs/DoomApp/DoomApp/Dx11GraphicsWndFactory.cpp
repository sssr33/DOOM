#include "stdafx.h"
#include "Dx11GraphicsWndFactory.h"
#include "Dx11GraphicsWnd.h"
#include "vtable_bind.h"

const std::string Dx11GraphicsWndFactory::GraphicsImplName = "dx11_win";

InterfacePtr<Dx11GraphicsWndFactory> Dx11GraphicsWndFactory::Make() {
    return InterfacePtr<Dx11GraphicsWndFactory>(new Dx11GraphicsWndFactory, DoNotAddRefKey{});
}

HRESULT __stdcall Dx11GraphicsWndFactory::QueryInterface(
    const IID* riid,
    void** ppvObject)
{
    return E_NOINTERFACE;
}

ULONG __stdcall Dx11GraphicsWndFactory::AddRef() {
    return ++this->refCounter;
}

ULONG __stdcall Dx11GraphicsWndFactory::Release() {
    auto refc = --this->refCounter;

    if (refc == 0) {
        delete this;
    }

    return refc;
}

HRESULT __stdcall Dx11GraphicsWndFactory::CreateGraphicsWnd(
    const char* implName, IGraphicsWnd** res)
{
    if (!res) {
        return E_POINTER;
    }

    if (implName == Dx11GraphicsWndFactory::GraphicsImplName) {
        auto impl = Dx11GraphicsWnd::Make();
        *res = impl.detach();
        return S_OK;
    }

    return HRESULT_FROM_WIN32(ERROR_CALL_NOT_IMPLEMENTED);
}

Dx11GraphicsWndFactory::Dx11GraphicsWndFactory() {
    this->vtable = VTable::Instance().GetVTable();
}

Dx11GraphicsWndFactory::VTable& Dx11GraphicsWndFactory::VTable::Instance() {
    static VTable instance;
    return instance;
}

IGraphicsWndFactoryVtbl* Dx11GraphicsWndFactory::VTable::GetVTable() {
    return &this->vtable;
}

Dx11GraphicsWndFactory::VTable::VTable() {
    vtable_bind_base<Dx11GraphicsWndFactory>(this->vtable);
    vtable_bind<&Dx11GraphicsWndFactory::CreateGraphicsWnd>(this->vtable.CreateGraphicsWnd);
}

#include "Dx11GraphicsWndInputEvent.h"
#include "..\vtable_bind.h"

InterfacePtr<Dx11GraphicsWndInputEvent> Dx11GraphicsWndInputEvent::Make() {
    return InterfacePtr<Dx11GraphicsWndInputEvent>(new Dx11GraphicsWndInputEvent, DoNotAddRefKey{});
}

IRESULT __stdcall Dx11GraphicsWndInputEvent::QueryInterface(
    const IGUID* guid,
    void** ppvObject)
{
    return IRESULT_ERROR;
}

int32_t __stdcall Dx11GraphicsWndInputEvent::AddRef() {
    return ++this->refCounter;
}

int32_t __stdcall Dx11GraphicsWndInputEvent::Release() {
    auto refc = --this->refCounter;

    if (refc == 0) {
        delete this;
    }

    return refc;
}

IRESULT __stdcall Dx11GraphicsWndInputEvent::GetEventType(
    enum GraphicsWndInputEventType* type)
{
    if (!type) {
        return IRESULT_ERROR;
    }

    *type = this->eventType;

    return IRESULT_OK;
}

IRESULT __stdcall Dx11GraphicsWndInputEvent::GetKey(
    enum GraphicsWndInputEventKey* key)
{
    if (!key) {
        return IRESULT_ERROR;
    }

    *key = this->key;

    return IRESULT_OK;
}

IRESULT __stdcall Dx11GraphicsWndInputEvent::GetKeyChar(
    int* keyChar)
{
    if (!keyChar) {
        return IRESULT_ERROR;
    }

    *keyChar = this->keyChar;

    return IRESULT_OK;
}

IRESULT __stdcall Dx11GraphicsWndInputEvent::GetMouseButton(
    enum GraphicsWndInputEventMouseButton* mouseButton)
{
    if (!mouseButton) {
        return IRESULT_ERROR;
    }

    *mouseButton = this->mouseButton;

    return IRESULT_OK;
}

IRESULT __stdcall Dx11GraphicsWndInputEvent::GetMousePosition(
    int* x,
    int* y)
{
    if (!x || !y) {
        return IRESULT_ERROR;
    }

    *x = this->mouseX;
    *y = this->mouseY;

    return IRESULT_OK;
}

void Dx11GraphicsWndInputEvent::SetEventType(GraphicsWndInputEventType type) {
    this->eventType = type;
}

void Dx11GraphicsWndInputEvent::SetKey(GraphicsWndInputEventKey key) {
    this->key = key;
}

void Dx11GraphicsWndInputEvent::SetKeyChar(int keyChar) {
    this->keyChar = keyChar;
}

void Dx11GraphicsWndInputEvent::SetMouseButton(GraphicsWndInputEventMouseButton mouseButton) {
    this->mouseButton = mouseButton;
}

void Dx11GraphicsWndInputEvent::SetMousePosition(int x, int y) {
    this->mouseX = x;
    this->mouseY = y;
}

Dx11GraphicsWndInputEvent::Dx11GraphicsWndInputEvent() {
    this->vtable = VTable::Instance().GetVTable();
}




Dx11GraphicsWndInputEvent::VTable& Dx11GraphicsWndInputEvent::VTable::Instance() {
    static VTable instance;
    return instance;
}

IGraphicsWndInputEventVtbl* Dx11GraphicsWndInputEvent::VTable::GetVTable() {
    return &this->vtable;
}

Dx11GraphicsWndInputEvent::VTable::VTable() {
    vtable_bind_base<Dx11GraphicsWndInputEvent>(this->vtable);
    vtable_bind<&Dx11GraphicsWndInputEvent::GetEventType>(this->vtable.GetEventType);
    vtable_bind<&Dx11GraphicsWndInputEvent::GetKey>(this->vtable.GetKey);
    vtable_bind<&Dx11GraphicsWndInputEvent::GetKeyChar>(this->vtable.GetKeyChar);
    vtable_bind<&Dx11GraphicsWndInputEvent::GetMouseButton>(this->vtable.GetMouseButton);
    vtable_bind<&Dx11GraphicsWndInputEvent::GetMousePosition>(this->vtable.GetMousePosition);
}

#pragma once

#include <winExt\InterfacePtr.h>
#include <winExt\IGraphicsWndInputEvent.h>

class Dx11GraphicsWndInputEvent : public IGraphicsWndInputEvent {
public:
    static InterfacePtr<Dx11GraphicsWndInputEvent> Make();

    IRESULT __stdcall QueryInterface(
        const IGUID* guid,
        void** ppvObject);

    int32_t __stdcall AddRef();

    int32_t __stdcall Release();

    IRESULT __stdcall GetEventType(
        enum GraphicsWndInputEventType* type);

    IRESULT __stdcall GetKey(
        enum GraphicsWndInputEventKey* key);

    IRESULT __stdcall GetKeyChar(
        int* keyChar);

    IRESULT __stdcall GetMouseButton(
        enum GraphicsWndInputEventMouseButton* mouseButton);

    IRESULT __stdcall GetMousePosition(
        int* x,
        int* y);

    void SetEventType(GraphicsWndInputEventType type);

    void SetKey(GraphicsWndInputEventKey key);
    void SetKeyChar(int keyChar);

    void SetMouseButton(GraphicsWndInputEventMouseButton mouseButton);
    void SetMousePosition(int x, int y);

private:
    class VTable {
    public:
        static VTable& Instance();

        IGraphicsWndInputEventVtbl* GetVTable();

    private:
        VTable();

        IGraphicsWndInputEventVtbl vtable = {};
    };

    Dx11GraphicsWndInputEvent();

    std::atomic<int32_t> refCounter = 1;

    GraphicsWndInputEventType eventType = GraphicsWndInputEventType_None;

    GraphicsWndInputEventKey key = GraphicsWndInputEventKey_None;
    int keyChar = 0;

    GraphicsWndInputEventMouseButton mouseButton = GraphicsWndInputEventMouseButton_None;
    int mouseX = 0;
    int mouseY = 0;
};

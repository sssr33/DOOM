#pragma once
#include "InterfaceBase.h"

#ifdef __cplusplus
extern "C" {
#endif

    enum GraphicsWndInputEventType {
        GraphicsWndInputEventType_None,
        GraphicsWndInputEventType_KeyPress,
        GraphicsWndInputEventType_KeyRelease,
        GraphicsWndInputEventType_MousePress,
        GraphicsWndInputEventType_MouseRelease,
        GraphicsWndInputEventType_MouseMove,
    };

    enum GraphicsWndInputEventKey {
        GraphicsWndInputEventKey_None,
        GraphicsWndInputEventKey_Char,
        GraphicsWndInputEventKey_Left,
        GraphicsWndInputEventKey_Right,
        GraphicsWndInputEventKey_Down,
        GraphicsWndInputEventKey_Up,
        GraphicsWndInputEventKey_Escape,
        GraphicsWndInputEventKey_Return,
        GraphicsWndInputEventKey_Tab,
        GraphicsWndInputEventKey_F1,
        GraphicsWndInputEventKey_F2,
        GraphicsWndInputEventKey_F3,
        GraphicsWndInputEventKey_F4,
        GraphicsWndInputEventKey_F5,
        GraphicsWndInputEventKey_F6,
        GraphicsWndInputEventKey_F7,
        GraphicsWndInputEventKey_F8,
        GraphicsWndInputEventKey_F9,
        GraphicsWndInputEventKey_F10,
        GraphicsWndInputEventKey_F11,
        GraphicsWndInputEventKey_F12,
        GraphicsWndInputEventKey_BackSpace,
        GraphicsWndInputEventKey_Delete,
        GraphicsWndInputEventKey_Pause,
        GraphicsWndInputEventKey_Equal,
        GraphicsWndInputEventKey_Subtract,
        GraphicsWndInputEventKey_ShiftLeft,
        GraphicsWndInputEventKey_ShiftRight,
        GraphicsWndInputEventKey_CtrlLeft,
        GraphicsWndInputEventKey_CtrlRight,
        GraphicsWndInputEventKey_AltLeft,
        GraphicsWndInputEventKey_AltRight,
    };

    enum GraphicsWndInputEventMouseButton {
        GraphicsWndInputEventMouseButton_None,
        GraphicsWndInputEventMouseButton_Left,
        GraphicsWndInputEventMouseButton_Right,
        GraphicsWndInputEventMouseButton_Middle,
        GraphicsWndInputEventMouseButton_Button4,
        GraphicsWndInputEventMouseButton_Button5,
    };

    typedef struct IGraphicsWndInputEvent IGraphicsWndInputEvent;

    struct IGraphicsWndInputEventVtbl {
        IRESULT(__stdcall* QueryInterface)(
            IGraphicsWndInputEvent* This,
            const IGUID* guid,
            void** ppvObject);

        int32_t(__stdcall* AddRef)(
            IGraphicsWndInputEvent* This);

        int32_t(__stdcall* Release)(
            IGraphicsWndInputEvent* This);

        IRESULT(__stdcall* GetEventType)(
            IGraphicsWndInputEvent* This,
            enum GraphicsWndInputEventType* type);

        IRESULT(__stdcall* GetKey)(
            IGraphicsWndInputEvent* This,
            enum GraphicsWndInputEventKey* key);

        IRESULT(__stdcall* GetKeyChar)(
            IGraphicsWndInputEvent* This,
            int* keyChar);

        IRESULT(__stdcall* GetMouseButton)(
            IGraphicsWndInputEvent* This,
            enum GraphicsWndInputEventMouseButton* mouseButton);

        IRESULT(__stdcall* GetMousePosition)(
            IGraphicsWndInputEvent* This,
            int* x,
            int* y);
    };

    struct IGraphicsWndInputEvent {
        const struct IGraphicsWndInputEventVtbl* vtable;
    };

#ifdef __cplusplus
}
#endif
#pragma once
#include "Dx11GraphicsWndInputEvent.h"

#include <queue>
#include <Windows.h>

class Dx11GraphicsWndInputHandler {
public:
    bool HandleInputEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    IRESULT __stdcall TryGetNextInputEvent(
        IGraphicsWndInputEvent** evt);

private:
    bool HandleMouseEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void CreateKeyEvent(bool isKeyUpEvent, WPARAM wParam, LPARAM lParam);

    static bool IsRightKey(LPARAM lParam);

    std::queue<InterfacePtr<Dx11GraphicsWndInputEvent>> inputEvents;
};

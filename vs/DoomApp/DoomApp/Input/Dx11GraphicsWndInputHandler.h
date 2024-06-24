#pragma once
#include "Dx11GraphicsWndInputEvent.h"

#include <queue>
#include <functional>
#include <Windows.h>
#include <DirectXMath.h>

class Dx11GraphicsWndInputHandler {
public:
    using WindowToGameCoordsFn = std::function<DirectX::XMINT2(const DirectX::XMINT2&)>;

    bool HandleInputEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    IRESULT __stdcall TryGetNextInputEvent(
        IGraphicsWndInputEvent** evt);

    void SetWindowToGameCoords(WindowToGameCoordsFn windowToGameCoords);

private:
    bool HandleMouseEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void CreateKeyEvent(bool isKeyUpEvent, WPARAM wParam, LPARAM lParam);

    static bool IsRightKey(LPARAM lParam);
    static bool IsSysKeyAlt(WPARAM wParam, LPARAM lParam);

    std::queue<InterfacePtr<Dx11GraphicsWndInputEvent>> inputEvents;
    WindowToGameCoordsFn windowToGameCoords;
};

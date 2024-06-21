#include "Window.h"

#include <bit>
#include <stdexcept>
#include <cassert>

Window::Window(std::wstring windowClassName, std::wstring windowName)
    : windowClassName(std::move(windowClassName))
    , windowName(std::move(windowName))
{
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &Window::WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
    wc.lpszClassName = this->windowClassName.c_str();

    if (!RegisterClass(&wc)) {
        DWORD errorCode = GetLastError();

        if (errorCode != ERROR_CLASS_ALREADY_EXISTS) {
            throw std::runtime_error("RegisterClass failed");
        }
    }

    this->hwnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP /*| WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST*/,
        this->windowClassName.c_str(),
        this->windowName.c_str(),
        WS_OVERLAPPEDWINDOW,
        //WS_POPUP | WS_THICKFRAME | WS_BORDER | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX, // to remove border
        // Size and position
        //CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        200, 200, 1280, 720,
        nullptr,       // Parent window
        nullptr,       // Menu
        wc.hInstance,
        this
    );

    if (!this->hwnd) {
        throw std::runtime_error("CreateWindowEx failed");
    }

    ShowWindow(this->hwnd, SW_SHOW);
}

Window::~Window() {
    if (!this->destroyReceived) {
        BOOL result = DestroyWindow(hwnd);

        if (result == FALSE) {
            DWORD errorCode = GetLastError();
            // TODO check logic
            assert(false);
        }
    }
}

HWND Window::GetHwnd() const {
    return this->hwnd;
}

Window::CustomWndProcHandler Window::GetCustomWndProcHandler() const {
    return this->customWndProcHandler;
}

void Window::SetCustomWndProcHandler(CustomWndProcHandler handler) {
    this->customWndProcHandler = std::move(handler);
}

WindowsThreadMessageQueue Window::MakeWindowsThreadMessageQueue() {
    return WindowsThreadMessageQueue();
}

LRESULT Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LONG_PTR userData = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    auto wnd = std::bit_cast<Window*>(userData);

    switch (uMsg) {
    case WM_CREATE: {
        auto createStruct = std::bit_cast<CREATESTRUCT*>(lParam);
        auto wndTmp = static_cast<Window*>(createStruct->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, std::bit_cast<LONG_PTR>(wndTmp));
        return 0;
    }
    case WM_DESTROY:
        if (wnd) {
            wnd->destroyReceived = true;
        }
        PostQuitMessage(0);
        return 0;
    default: {
        if (wnd && wnd->customWndProcHandler) {
            return wnd->customWndProcHandler(wnd, hwnd, uMsg, wParam, lParam);
        }
        else {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    }

    return 0;
}

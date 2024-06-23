#include "Dx11GraphicsWndInputHandler.h"

#include <string>

bool Dx11GraphicsWndInputHandler::HandleInputEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (this->HandleMouseEvents(uMsg, wParam, lParam)) {
        return true;
    }
    else if (this->HandleKeyEvents(uMsg, wParam, lParam)) {
        return true;
    }

    return false;
}

IRESULT __stdcall Dx11GraphicsWndInputHandler::TryGetNextInputEvent(
    IGraphicsWndInputEvent** evt)
{
    if (!evt) {
        return IRESULT_ERROR;
    }

    if (!this->inputEvents.empty()) {
        auto inputEvt = std::move(this->inputEvents.front());
        this->inputEvents.pop();

        *evt = inputEvt.detach();
    }

    return IRESULT_OK;
}

bool Dx11GraphicsWndInputHandler::HandleMouseEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    /*switch (uMsg) {
    case WM_MOUSEMOVE: {
        break;
    }
    default:
        return false;
    }*/

    //return true;

    return false;
}

bool Dx11GraphicsWndInputHandler::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    bool eventHandled = true;

    auto OnKeyUp = [&]
    {
        this->CreateKeyEvent(true, wParam, lParam);
    };
    auto OnKeyDown = [&]
    {
        this->CreateKeyEvent(false, wParam, lParam);
    };

    switch (uMsg) {
    case WM_KEYUP: {
        OnKeyUp();
        break;
    }
    case WM_KEYDOWN: {
        OnKeyDown();
        break;
    }
    case WM_SYSKEYUP: {
        if (IsSysKeyAlt(wParam, lParam)) {
            OnKeyUp();
        }
        else {
            eventHandled = false;
        }
        break;
    }
    case WM_SYSKEYDOWN: {
        if (IsSysKeyAlt(wParam, lParam)) {
            OnKeyDown();
        }
        else {
            eventHandled = false;
        }
        break;
    }
    default:
        eventHandled = false;
        break;
    }

    return eventHandled;
}

void Dx11GraphicsWndInputHandler::CreateKeyEvent(bool isKeyUpEvent, WPARAM wParam, LPARAM lParam) {
    GraphicsWndInputEventType evtType = isKeyUpEvent ? GraphicsWndInputEventType_KeyRelease : GraphicsWndInputEventType_KeyPress;
    GraphicsWndInputEventKey key = GraphicsWndInputEventKey_None;

    switch (wParam) {
    case VK_LEFT: key = GraphicsWndInputEventKey_Left; break;
    case VK_RIGHT: key = GraphicsWndInputEventKey_Right; break;
    case VK_DOWN: key = GraphicsWndInputEventKey_Down; break;
    case VK_UP: key = GraphicsWndInputEventKey_Up; break;
    case VK_ESCAPE: key = GraphicsWndInputEventKey_Escape; break;
    case VK_RETURN: key = GraphicsWndInputEventKey_Return; break;
    case VK_TAB: key = GraphicsWndInputEventKey_Tab; break;
    case VK_F1: key = GraphicsWndInputEventKey_F1; break;
    case VK_F2: key = GraphicsWndInputEventKey_F2; break;
    case VK_F3: key = GraphicsWndInputEventKey_F3; break;
    case VK_F4: key = GraphicsWndInputEventKey_F4; break;
    case VK_F5: key = GraphicsWndInputEventKey_F5; break;
    case VK_F6: key = GraphicsWndInputEventKey_F6; break;
    case VK_F7: key = GraphicsWndInputEventKey_F7; break;
    case VK_F8: key = GraphicsWndInputEventKey_F8; break;
    case VK_F9: key = GraphicsWndInputEventKey_F9; break;
    case VK_F10: key = GraphicsWndInputEventKey_F10; break;
    case VK_F11: key = GraphicsWndInputEventKey_F11; break;
    case VK_F12: key = GraphicsWndInputEventKey_F12; break;
    case VK_BACK: key = GraphicsWndInputEventKey_BackSpace; break;
    case VK_DELETE: key = GraphicsWndInputEventKey_Delete; break;
    case VK_PAUSE: key = GraphicsWndInputEventKey_Pause; break;
    case VK_OEM_PLUS: key = GraphicsWndInputEventKey_Equal; break;
    case VK_OEM_MINUS: key = GraphicsWndInputEventKey_Subtract; break;
    case VK_SHIFT: {
        key = IsRightKey(lParam) ?
            GraphicsWndInputEventKey_ShiftRight :
            GraphicsWndInputEventKey_ShiftLeft;
        break;
    }
    case VK_CONTROL: {
        key = IsRightKey(lParam) ?
            GraphicsWndInputEventKey_CtrlRight :
            GraphicsWndInputEventKey_CtrlLeft;
        break;
    }
    case VK_MENU: {
        key = IsRightKey(lParam) ?
            GraphicsWndInputEventKey_AltRight :
            GraphicsWndInputEventKey_AltLeft;
        break;
    }
    default:
        key = GraphicsWndInputEventKey_Char;
        break;
    }

    if (key != GraphicsWndInputEventKey_None) {
        auto inputEvt = Dx11GraphicsWndInputEvent::Make();

        inputEvt->SetEventType(evtType);
        inputEvt->SetKey(key);

        if (key == GraphicsWndInputEventKey_Char) {
            // doom handles lower case
            inputEvt->SetKeyChar(std::tolower(static_cast<int>(wParam)));
        }

        this->inputEvents.push(std::move(inputEvt));

        {
            std::wstring str;

            switch (key)
            {
            case GraphicsWndInputEventKey_None:
                str = L"None";
                break;
            case GraphicsWndInputEventKey_Char:
                str = L"Char";
                break;
            case GraphicsWndInputEventKey_Left:
                str = L"Left";
                break;
            case GraphicsWndInputEventKey_Right:
                str = L"Right";
                break;
            case GraphicsWndInputEventKey_Down:
                str = L"Down";
                break;
            case GraphicsWndInputEventKey_Up:
                str = L"Up";
                break;
            case GraphicsWndInputEventKey_Escape:
                str = L"Escape";
                break;
            case GraphicsWndInputEventKey_Return:
                str = L"Return";
                break;
            case GraphicsWndInputEventKey_Tab:
                str = L"Tab";
                break;
            case GraphicsWndInputEventKey_F1:
                str = L"F1";
                break;
            case GraphicsWndInputEventKey_F2:
                str = L"F2";
                break;
            case GraphicsWndInputEventKey_F3:
                str = L"F3";
                break;
            case GraphicsWndInputEventKey_F4:
                str = L"F4";
                break;
            case GraphicsWndInputEventKey_F5:
                str = L"F5";
                break;
            case GraphicsWndInputEventKey_F6:
                str = L"F6";
                break;
            case GraphicsWndInputEventKey_F7:
                str = L"F7";
                break;
            case GraphicsWndInputEventKey_F8:
                str = L"F8";
                break;
            case GraphicsWndInputEventKey_F9:
                str = L"F9";
                break;
            case GraphicsWndInputEventKey_F10:
                str = L"F10";
                break;
            case GraphicsWndInputEventKey_F11:
                str = L"F11";
                break;
            case GraphicsWndInputEventKey_F12:
                str = L"F12";
                break;
            case GraphicsWndInputEventKey_BackSpace:
                str = L"BackSpace";
                break;
            case GraphicsWndInputEventKey_Delete:
                str = L"Delete";
                break;
            case GraphicsWndInputEventKey_Pause:
                str = L"Pause";
                break;
            case GraphicsWndInputEventKey_Equal:
                str = L"Equal";
                break;
            case GraphicsWndInputEventKey_Subtract:
                str = L"Subtract";
                break;
            case GraphicsWndInputEventKey_ShiftLeft:
                str = L"ShiftLeft";
                break;
            case GraphicsWndInputEventKey_ShiftRight:
                str = L"ShiftRight";
                break;
            case GraphicsWndInputEventKey_CtrlLeft:
                str = L"CtrlLeft";
                break;
            case GraphicsWndInputEventKey_CtrlRight:
                str = L"CtrlRight";
                break;
            case GraphicsWndInputEventKey_AltLeft:
                str = L"AltLeft";
                break;
            case GraphicsWndInputEventKey_AltRight:
                str = L"AltRight";
                break;
            default:
                break;
            }

            std::wstring upOrDownStr = isKeyUpEvent ? L"UP" : L"DOWN";

            if (key == GraphicsWndInputEventKey_Char) {
                str = static_cast<wchar_t>(wParam);
            }

            std::wstring msg = std::wstring()
                + upOrDownStr
                + L" "
                + str
                + L"\n"
                ;

            OutputDebugStringW(msg.c_str());
        }
    }
}

bool Dx11GraphicsWndInputHandler::IsRightKey(LPARAM lParam) {
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keyup#parameters
    // 24    Indicates whether the key is an extended key, such as the right-hand ALT and CTRL keys that appear on an enhanced 101- or 102-key keyboard. The value is 1 if it is an extended key; otherwise, it is 0.
    if (lParam & ((LPARAM)1 << 24)) {
        return true;
    }

    return false;
}

bool Dx11GraphicsWndInputHandler::IsSysKeyAlt(WPARAM wParam, LPARAM lParam) {
    // https://www.reddit.com/r/learnprogramming/comments/nqrt4o/comment/h0d1te9/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button
    bool isAlt = (HIWORD(lParam) && KF_ALTDOWN) || (wParam == VK_MENU);
    return isAlt;
}

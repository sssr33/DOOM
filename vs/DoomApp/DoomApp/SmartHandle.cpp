#include "SmartHandle.h"

void CloseHandleDeleter::operator()(HANDLE handle) {
    CloseHandle(handle);
}

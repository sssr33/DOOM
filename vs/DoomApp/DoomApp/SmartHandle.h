#pragma once

#include <Windows.h>
#include <memory>
#include <type_traits>

struct CloseHandleDeleter {
    void operator()(HANDLE handle);
};

using UniqueEvent = std::unique_ptr<std::remove_pointer_t<HANDLE>, CloseHandleDeleter>;

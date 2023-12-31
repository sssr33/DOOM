#pragma once

// DOOM defines SHORT and LONG as macros so push/pop them to fix warnings
#if defined(SHORT)
#pragma push_macro("SHORT")
#define RESTORE_SHORT
#undef SHORT
#endif

#if defined(LONG)
#pragma push_macro("LONG")
#define RESTORE_LONG
#undef LONG
#endif

// https://stackoverflow.com/questions/21399650/cannot-include-both-files-winsock2-windows-h
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#undef LoadMenu

#if defined(RESTORE_SHORT)
#undef RESTORE_SHORT
#pragma pop_macro("SHORT")
#endif

#if defined(RESTORE_LONG)
#undef RESTORE_LONG
#pragma pop_macro("LONG")
#endif

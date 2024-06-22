#pragma once

// WARNING DO NOT USE windows headers because it will conflict with boolean type which must be int

//#include <windowsSafe.h>
//#include <time.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <stdio.h>

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

#undef MAXCHAR
#undef MAXSHORT
#undef MAXINT
#undef MAXLONG
#undef MINCHAR
#undef MINSHORT
#undef MININT
#undef MINLONG

#define ioctl ioctlsocket
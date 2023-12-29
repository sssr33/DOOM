#pragma once

// https://stackoverflow.com/questions/21399650/cannot-include-both-files-winsock2-windows-h
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#undef LoadMenu

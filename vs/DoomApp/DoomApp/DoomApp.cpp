// DoomApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Wrapper.h"

#include <windows.h>

int main()
{
	char *argv[] = {
		"DoomApp.exe"
	};

	int argc = ARRAYSIZE(argv);

	mainWrapper(argv, argc);

    return 0;
}
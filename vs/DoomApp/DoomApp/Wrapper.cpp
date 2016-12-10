#include "Wrapper.h"

extern "C" {
#include <doomdef.h>
#include <m_argv.h>
#include <d_main.h>
}

void mainWrapper(char **argv, int argc) {
	myargc = argc;
	myargv = argv;

	D_DoomMain();
}
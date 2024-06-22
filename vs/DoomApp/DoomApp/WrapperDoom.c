#include "WrapperDoom.h"

#include <doomdef.h>
#include <m_argv.h>
#include <d_main.h>

void mainDoom(char** argv, int argc) {
    myargc = argc;
    myargv = argv;

    D_DoomMain();
}

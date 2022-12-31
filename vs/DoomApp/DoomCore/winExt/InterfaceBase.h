#pragma once

#define WIN32_LEAN_AND_MEAN
// https://stackoverflow.com/questions/27435565/undef-a-typedef-in-c
#define boolean booleanWindows
#include <Unknwn.h>
#undef boolean

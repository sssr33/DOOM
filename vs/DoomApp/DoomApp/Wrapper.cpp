#include "Wrapper.h"

extern "C" {
#include "WrapperDoom.h"
}

//extern "C" {
//#include <doomdef.h>
//#include <m_argv.h>
//#include <d_main.h>
//}
//// from doom code to fix warnings
//#undef MAXCHAR
//#undef MAXSHORT
//#undef MAXINT
//#undef MAXLONG
//#undef MINCHAR
//#undef MINSHORT
//#undef MININT
//#undef MINLONG

#include <string>
#include <cassert>
#include <Windows.h>

void mainWrapper(char **argv, int argc) {
	/*myargc = argc;
	myargv = argv;*/

	{
		errno_t err = {};
		WCHAR path[MAX_PATH];
		GetModuleFileNameW(NULL, path, MAX_PATH);

		std::wstring pathStr = path;
		pathStr = pathStr.substr(0, pathStr.find_last_of(L"\\/"));

		err = _wputenv_s(L"HOME", pathStr.c_str());
		if (err != 0) {
			assert(false);
		}

		err = _wputenv_s(L"DOOMWADDIR", pathStr.c_str());
		if (err != 0) {
			assert(false);
		}
	}

	mainDoom(argv, argc);

	//D_DoomMain();
}
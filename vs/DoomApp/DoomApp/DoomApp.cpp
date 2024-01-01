// DoomApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Wrapper.h"
#include "Dx11GraphicsWndFactory.h"

#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>
#include <ranges>
#include <cassert>
#include <stdexcept>
#include <winExt\IGaphicsWndCreate.h>

void SetGraphicsWndFactory();
std::vector<char*> MakeArgv();

int main()
{
	SetGraphicsWndFactory();

	auto argv = MakeArgv();
	mainWrapper(argv.data(), static_cast<int>(argv.size()));

    return 0;
}

void SetGraphicsWndFactory() {
	HRESULT hr = S_OK;
	auto factory = Dx11GraphicsWndFactory::Make();

	hr = SetIGraphicsWndFactory(factory.get());
	if (FAILED(hr)) {
		throw std::runtime_error("SetGraphicsWndFactory failed");
	}
}

std::vector<char*> MakeArgv() {
	static std::vector<std::string> argvStr =
	{
		"DoomApp.exe",
		"-graphicsImplName", Dx11GraphicsWndFactory::GraphicsImplName
	};

	std::vector<char*> argv;

	argv.reserve(argvStr.size());
	std::ranges::transform(argvStr, std::back_inserter(argv), [](std::string& i) { return i.data(); });

	return argv;
}

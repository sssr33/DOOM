// DoomApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Wrapper.h"
#include "Dx11GraphicsWndFactory.h"
#include "Window.h"

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

#include <thread>

HANDLE mainThread = 0;

void CALLBACK WakeWorker(ULONG_PTR)
{
	int stop = 234;
}

int main()
{
	//{
	//	DWORD threadId = GetThreadId(GetCurrentThread());

	//	mainThread = OpenThread(THREAD_SET_CONTEXT, FALSE, threadId);
	//}

	//auto th = std::thread([]
	//	{
	//		Sleep(5000);

	//		if (!QueueUserAPC(WakeWorker, mainThread, 0)) {
	//			int stop = 234;
	//		}
	//	});

	//while (true)
	//{
	//	Window wnd(L"DoomWndClass", L"Doom");
	//	Window wnd1(L"DoomWndClass", L"Doom2");
	//	Window wnd2(L"DoomWndClass", L"Doom3");
	//	Window wnd3(L"DoomWndClass", L"Doom4");

	//	WindowsThreadMessageQueue wndMsgQueue;

	//	//Sleep(3000);

	//	while (true)
	//	{
	//		wndMsgQueue.WaitAndProcessQueuedMessages();
	//	}

	//	int stop = 234;
	//}

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

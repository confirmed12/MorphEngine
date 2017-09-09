#include "Cheat.h"
#include "rbot.h"
#include "thirdperson.h"
#include "SkinChanger.h"
#include "DLLMain.h"
#include "xor.h"

#include <wininet.h>
#include <cstring>
#include <windows.h>
#include <iostream>
#include <urlmon.h>
#include <ctype.h>

#pragma comment(lib,"wininet.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"WS2_32.lib")

void CheatThread()
{
	U::Setup();

	while (!G::PressedKeys[VK_END])
	{
		//if (Vars.Misc.Cname)
			//E::Misc->ChangeCName();

		if (Vars.Misc.ChatSpam)
			E::Misc->Chatspam();

		if (Vars.Misc.LocSpam)
			E::Misc->Run_LocationSpam();

		//if (Vars.Misc.NameSteal)
			//E::Misc->NameSteal();
	}

}

//bool CanConnect() {
//	HINTERNET connect = InternetOpen(/*MyBrowser*/Xor<0x90, 10, 0xF6D9C953>("\xDD\xE8\xD0\xE1\xFB\xE2\xE5\xF2\xEA" + 0xF6D9C953).s, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
//
//	if (!connect)
//	return false;
//
//	std::string URL = /*http://morphster.xyz/checker/canload.php*/Xor<0x86, 41, 0xBF4F01C7>("\xEE\xF3\xFC\xF9\xB0\xA4\xA3\xE0\xE1\xFD\xE0\xF9\xE1\xE7\xF1\xE7\xB8\xEF\xE1\xE3\xB5\xF8\xF4\xF8\xFD\xF4\xC5\xD3\x8D\xC0\xC5\xCB\xCA\xC8\xC9\xCD\x84\xDB\xC4\xDD" + 0xBF4F01C7).s;
//	LPCSTR Final = URL.c_str();
//
//	HINTERNET OpenAddress = InternetOpenUrl(connect, Final, NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);
//
//	//WSAAsyncGetServByName(NULL,NULL,/*http://morphster.xyz*/Xor<0xB3, 21, 0x47C75843>("\xDB\xC0\xC1\xC6\x8D\x97\x96\xD7\xD4\xCE\xCD\xD6\xCC\xB4\xA4\xB0\xED\xBC\xBC\xBC" + 0x47C75843).s,"","",1);
//
//	if (!OpenAddress) {
//	InternetCloseHandle(connect);
//	return false;
//	}
//
//	char DataReceived[4096];
//	DWORD NumberOfBytesRead = 0;
//	while (InternetReadFile(OpenAddress, DataReceived, 4096, &NumberOfBytesRead) && NumberOfBytesRead) {}
//
//	InternetCloseHandle(OpenAddress);
//	InternetCloseHandle(connect);
//
//	if (strstr(DataReceived, /*True*/Xor<0xC5, 5, 0xFA79B0E5>("\x91\xB4\xB2\xAD" + 0xFA79B0E5).s) || strstr(DataReceived, /*Start*/Xor<0xAB, 6, 0xE5D98F4C>("\xF8\xD8\xCC\xDC\xDB" + 0xE5D98F4C).s)) {
//	return true;
//	}
//	return false;
//}

//void mamkamain()
//{
//	bool isconnected = CanConnect();
//
//	/*AllocConsole();
//	AttachConsole(GetCurrentProcessId());
//	freopen("CONIN$", "r", stdin);
//	freopen("CONOUT$", "w", stdout);
//	freopen("CONOUT$", "w", stderr);
//	SetConsoleTitleA("FARTUMASTIAYEware.zz");
//
//	printf("Connected: %s", isconnected ? "true" : "false");*/
//
//	if (isconnected == true && CanConnect() == true) {
//		G::Window = FindWindowA( /*Valve001*/Xor<0xDA, 9, 0x55400699>("\x8C\xBA\xB0\xAB\xBB\xEF\xD0\xD0" + 0x55400699).s, NULL);
//
//		Hooks::oldWindowProc = (WNDPROC)SetWindowLongPtr(G::Window, GWL_WNDPROC, (LONG_PTR)Hooks::WndProc);
//
//		CheatThread();
//	}
//	else
//		ExitProcess(0);
//}

DWORD WINAPI DllMain( HMODULE hDll, DWORD dwReason, LPVOID lpReserved )
{
	switch( dwReason )
	{
	case DLL_PROCESS_ATTACH:
	{
		while (!(G::Window = FindWindowA(XorStr("Valve001"), NULL))) Sleep(200);
		Hooks::oldWindowProc = (WNDPROC)SetWindowLongPtr(G::Window, GWL_WNDPROC, (LONG_PTR)Hooks::WndProc);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CheatThread, NULL, NULL, NULL);

		G::Dll = hDll;

		return TRUE;
	}
	case DLL_PROCESS_DETACH:
	{
		ExitProcess(0);
		return TRUE;
	}

	return FALSE;
	}
}
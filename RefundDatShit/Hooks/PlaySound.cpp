#include "../Cheat.h"

// c&p from Mark_HC's source because it worked

PlaySoundFn oPlaySound;
void __stdcall Hooks::PlaySound_CSGO( const char* fileName )
{
	oPlaySound( fileName );

	if (!Vars.Misc.AutoAccept || I::Engine->IsInGame()) return;

	if (!strcmp(fileName, charenc("UI/competitive_accept_beep.wav"))) {

		//This is the function that is called when you press the big ACCEPT button
		U::IsReady = (IsReadyFn)((DWORD)U::FindPattern(charenc("client.dll"), charenc("55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 8B BE")));
		//Accept the game
		U::IsReady();

		//This will flash the CSGO window on the taskbar
		//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
		FLASHWINFO fi;
		fi.cbSize = sizeof(FLASHWINFO);
		fi.hwnd = G::Window;
		fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
		fi.uCount = 0;
		fi.dwTimeout = 0;
		FlashWindowEx(&fi);
	}
}
#pragma once

#include "../SDK/Entity.h"
#include "../Cheat.h"
#include "../Enhancements.h"

namespace G // Global Stuff
{
	extern bool			Aimbotting;
	extern CBaseEntity* LocalPlayer;
	extern QAngle doz;
	extern bool			Return;
	extern CUserCmd*	UserCmd;
	extern float		FSNLBY;
	extern bool			LastLBYUpdate;
	extern HMODULE		Dll;
	extern HWND			Window;
	extern int			TickCount;
	extern Vector		BestHitboxVec;

	extern bool			PressedKeys[ 256 ];
	extern bool			d3dinit;
	extern float		FOV;
	extern int			ChamMode;
	extern bool			SendPacket;
	extern bool ChangeAAY;
	extern QAngle VisualAngle;
	extern int			BestTarget;
}

namespace H // Global Hooks
{
	extern VTHook*	VPanel;
	extern VTHook*	ClientMode;
	extern VTHook*	Client;
	extern VTHook* Input;
	extern VTHook*	MDLCache;
	extern VTHook*	ModelRender;
	extern VTHook*	Surface;
	extern VTHook*  D3D9;
	extern VTHook*	GameEvent;
}

namespace E
{
	extern CLegitBot*			LegitBot;
	extern CRageBot*			RageBot;
	extern CVisuals*			Visuals;
	extern CMiscellaneous*		Misc;
	extern CPredictionSystem*	PredictionSystem;
	extern CFakeLag*			FakeLag;
	extern CClantagChanger*		ClantagChanger;
	extern CNameChanger*		NameChanger;
}

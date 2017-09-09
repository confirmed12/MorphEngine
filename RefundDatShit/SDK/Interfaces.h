#pragma once

#include "Interfaces/IBaseClientDLL.h"
#include "Interfaces/IClientEntityList.h"
#include "Interfaces/IClientModeShared.h"
#include "Interfaces/ICvar.h"
#include "Interfaces/IEngineClient.h"
#include "Interfaces/IEngineTrace.h"
#include "Interfaces/IGlobalVarsBase.h"
#include "Interfaces/IInputSystem.h"
#include "Interfaces.h"
#include "Interfaces/ISurface.h"
#include "Interfaces/IVPanel.h"
#include "Interfaces/IVModelInfo.h"
#include "Interfaces/IVModelRender.h"
#include "Interfaces/IMaterialSystem.h"
#include "Interfaces/IMaterial.h"
#include "Interfaces/IRenderView.h"
#include "Interfaces/IMoveHelper.h"
#include "Interfaces/IGameMovement.h"
#include "Interfaces/IPrediction.h"
#include "VPhysics.h"
#include "Interfaces/IVDebugOverlay.h"
#include "Interfaces/IStudioRender.h"
#include "../GameEvent.h"

namespace I // Interfaces
{
	extern IBaseClientDll*			Client;
	extern IClientModeShared*		ClientMode;
	extern CInput*					Input;
	extern IClientEntityList*		ClientEntList;
	extern ICVar*					Cvar;
	extern IMDLCache*				MDLCache;
	extern IInputSystem*			InputSystem;
	extern ILocalize*				Localize;
	extern IEngineClient*			Engine;
	extern IEngineTrace*			EngineTrace;
	extern IGlobalVarsBase*			Globals;
	extern ISurface*				Surface;
	//extern IBaseFileSystem*			FileSystem;
	extern IGameEventManager2*		GameEvent;
	extern IVPanel*					VPanel;
	extern IVModelRender*			ModelRender;
	extern IVModelInfo*				ModelInfo;
	extern IMaterialSystem*			MaterialSystem;
	extern IMaterial*				Material;
	extern IVRenderView*			RenderView;
	extern IGameMovement*			GameMovement;
	extern IMoveHelper*				MoveHelper;
	extern IPrediction*				Prediction;
	extern IPhysicsSurfaceProps*	Physprops;
	extern IVDebugOverlay*			DebugOverlay;
	extern IStudioRender*			StudioRender;
}
#include "../SDK.h"

bool				G::Aimbotting = false;
CBaseEntity*		G::LocalPlayer;
QAngle				G::doz;
bool				G::Return;
CUserCmd*			G::UserCmd;
int					G::TickCount;
Vector				G::BestHitboxVec = Vector(0,0,0);
HMODULE				G::Dll;
float				G::FSNLBY;
bool				G::LastLBYUpdate = false;
HWND				G::Window;
bool				G::PressedKeys[ 256 ] = {};
bool				G::d3dinit = false;
float				G::FOV;
int					G::ChamMode = -1;
bool				G::SendPacket = true;
bool				G::ChangeAAY = true;
QAngle				G::VisualAngle;
int					G::BestTarget = -1;

VTHook*				H::VPanel;
VTHook*				H::ClientMode;
VTHook*				H::Client;
//VTHook*				H::Prediction;
VTHook*				H::ModelRender;
VTHook*				H::MDLCache;
VTHook*				H::Surface;
VTHook*				H::Input;
VTHook*				H::GameEvent;
VTHook*				H::D3D9;

CLegitBot*			E::LegitBot = new CLegitBot();
CRageBot*			E::RageBot = new CRageBot();
CVisuals*			E::Visuals = new CVisuals();
CMiscellaneous*		E::Misc = new CMiscellaneous();
CFakeLag*			E::FakeLag = new CFakeLag();
CClantagChanger*	E::ClantagChanger = new CClantagChanger();
CNameChanger*		E::NameChanger = new CNameChanger();

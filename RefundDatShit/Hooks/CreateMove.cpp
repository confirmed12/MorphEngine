#include "../Cheat.h"
//#define WEAPON_DATA_SIZE ( sizeof( pWeaponData ) / sizeof( *pWeaponData ) )
#include "../Settings/Vars.h"

CreateMoveFn oCreateMove;

bool __stdcall Hooks::CreateMove( float flInputSampleTime, CUserCmd* cmd )
{
	bool oReturn = oCreateMove(flInputSampleTime, cmd);
	G::LocalPlayer = U::GetLocalPlayer();
	G::UserCmd = cmd;

	if (Vars.Ragebot.Enabled) Vars.Legitbot.Aimbot.Enabled = false;
	else if (Vars.Legitbot.Aimbot.Enabled) Vars.Ragebot.Enabled = false;

	G::SendPacket = true;

	if (cmd->command_number == 0 || !I::Engine->IsInGame())
		return false;

	if (!G::LocalPlayer || !G::LocalPlayer->GetAlive())
		return G::Return;

	DWORD* fp;
	__asm mov fp, ebp;

	if (Vars.Misc.Bhop)
		E::Misc->Bunnyhop();

	if (Vars.Misc.AutoStrafe > 0)
		E::Misc->AutoStrafe();

	//E::Misc->CircleStrafer();

	Vars.wpn = G::LocalPlayer->GetWeapon()->GetItemDefinitionIndex();

	PredictionSystem->StartPrediction();

	E::LegitBot->Run();
		
	if (Vars.Ragebot.Enabled)
		E::RageBot->Run();

	if (Vars.Ragebot.Antiaim.Enabled)
		AntiAim->Run();

	if( Vars.Misc.AirStuck )
		E::Misc->AirStuck();

	if (Vars.Ragebot.UntrustedCheck)
		cmd->viewangles.Clamp();

	E::FakeLag->Run();
	/*
	if (G::LocalPlayer->GetWeapon()->IsRevolver())
	{
		float flPostponeFireReady = G::LocalPlayer->GetWeapon()->GetflPostponeFireReadyTime();
		if (flPostponeFireReady > 0 && flPostponeFireReady - 1.f < I::Globals->curtime)
		{
			cmd->buttons &= ~IN_ATTACK;
		}
	}
	*/
	PredictionSystem->EndPrediction();

	if ((G::UserCmd->buttons & IN_ATTACK) && G::LocalPlayer->GetWeapon()->IsGun() && Vars.Ragebot.Enabled)
		Vars.m_iBulletsFire += 1;

	if (Vars.Ragebot.UntrustedCheck)
		cmd->viewangles.Clamp();

	if (Vars.Ragebot.AutoPistol || Vars.Legitbot.Aimbot.AutoPistol)
	{
		int iWeaponID = G::LocalPlayer->GetWeapon()->GetWeaponID();

		if (iWeaponID == WEAPON_FIVESEVEN || iWeaponID == WEAPON_DEAGLE || iWeaponID == WEAPON_ELITE || iWeaponID == WEAPON_P250 || iWeaponID == WEAPON_GLOCK || iWeaponID == WEAPON_USP_SILENCER || iWeaponID == WEAPON_TEC9 || iWeaponID == WEAPON_HKP2000)
		{
			if (G::UserCmd->buttons & IN_ATTACK)
			{
				float flCurTime = (float)G::LocalPlayer->GetTickBase() * I::Globals->interval_per_tick;
				float flNextAttack = G::LocalPlayer->GetWeapon()->GetNextPrimaryAttack();

				if (flNextAttack > flCurTime)
				{
					cmd->buttons &= ~IN_ATTACK;
					cmd->buttons |= IN_ATTACK;
					cmd->buttons &= ~IN_ATTACK;
				}
			}
		}
	}

	G::VisualAngle = *G::LocalPlayer->GetHeadRotation();

	*(bool*)(*fp - 0x1C) = G::SendPacket;

	return false;
}
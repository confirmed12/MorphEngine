#include "../Cheat.h"
#include "../SkinChanger.h"
#include "../thirdperson.h"
#include "../asus.h"

struct PlayerAA
{
	CBaseEntity* player;
	QAngle angle;

	PlayerAA(CBaseEntity* player, QAngle angle)
	{
		this->player = player;
		this->angle = angle;
	}
};

namespace LBYthings
{
	float OldYawDeltasX[64];
	float OldLowerBodyYawsX[64];
	float flTemp[64];
	float resolved_yaw[64];

	float LastLBYUpdateTime;

	float GetLatency()
	{
		INetChannelInfo *nci = I::Engine->GetNetChannelInfo();
		if (nci)
		{
			float Latency = nci->GetAvgLatency(2) + nci->GetAvgLatency(1);
			return Latency;
		}
		else
		{
			return 0.0f;
		}
	}

	bool NextLBYUpdate()
	{
		static float OldLBY = 0.f;
		float flServerTime = I::Globals->curtime;
		static int LBYBreakerTimer = 0;

		if (OldLBY != *G::LocalPlayer->GetLowerBodyYawTarget())
		{
			LBYBreakerTimer++;
			OldLBY = *G::LocalPlayer->GetLowerBodyYawTarget();
			LastLBYUpdateTime = flServerTime;
		}

		if (G::LocalPlayer->GetVelocity().Length2D() > 0.5)
		{
			LastLBYUpdateTime = flServerTime;
			return true;
		}


		if ((LastLBYUpdateTime + 1 - (GetLatency() * 2) < flServerTime) && (G::LocalPlayer->GetFlags() & FL_ONGROUND))
		{
			if (LastLBYUpdateTime + 1.1 - (GetLatency() * 2) < flServerTime)
			{
				LastLBYUpdateTime += 1.1;
			}
			return true;
		}

		return false;
	}


	void lbydelta(CBaseEntity* ent)
	{
		float flYaw;

		float LowerYaw = *ent->GetLowerBodyYawTarget();
		int index = ent->GetIndex();

		if (OldLowerBodyYawsX[index] != LowerYaw)
		{
			OldYawDeltasX[index] = LowerYaw - OldLowerBodyYawsX[index];
			OldLowerBodyYawsX[index] = LowerYaw;

			if (ent->GetVelocity().Length2D() > 0.1f && ent->GetFlags() & FL_ONGROUND)
			{
				flTemp[index] = LowerYaw;
				OldLowerBodyYawsX[index] = LowerYaw;
			}
			else
			{
				flTemp[index] = OldLowerBodyYawsX[index];
			}
		}
		else
		{
			flTemp[index] = LowerYaw - OldYawDeltasX[index];
		}

		float FixedResolve = flTemp[index];

		if (G::LastLBYUpdate)
		{
			flYaw = FixedResolve;
		}
		else
		{
			if (LastLBYUpdateTime <= 0.57f && LastLBYUpdateTime >= 0.27f) // if it wasnt long ago that lby updated, just check nearby
			{
				resolved_yaw[index] = FixedResolve + U::RandomFloat(-35, 35); // -35.0 - 35.0
			}
			else // if its getting old info, check in a wider area
			{
				resolved_yaw[index] = FixedResolve + U::RandomFloat(-90, 90);
			}
			flYaw = resolved_yaw[index];
		}

		ent->GetHeadRotation()->y = flYaw;
	}
}

std::vector<int> CPlayerList::Players = {};
std::vector<PlayerAA> player_data;
std::vector< CPlayer > Players;

float DoYawAAA(CBaseEntity* pEnt) {
	float yaw = pEnt->GetHeadRotation()->y;

	yaw -= 180.f;

	return yaw;
}


float LBYResolver(CBaseEntity* Player,bool lby2)
{
	QAngle angle;

	angle.y = Player->GetHeadRotation()->y;

	if (G::LastLBYUpdate)
		angle.y -= lby2 ? 180.f : 90.f;

	return angle.y;
}

FrameStageNotifyFn oFrameStageNotify;
void __stdcall Hooks::FrameStageNotify(ClientFrameStage_t stage)
{
	if (!G::LocalPlayer || G::LocalPlayer->GetHealth() <= 0)
		return oFrameStageNotify(stage);

	G::FSNLBY = *G::LocalPlayer->GetLowerBodyYawTarget();
	G::LastLBYUpdate = LBYthings::NextLBYUpdate();
	// Resolver
	if ((Vars.Ragebot.Antiaim.Resolver || Vars.Ragebot.ResolverHelper) && I::Engine->IsInGame() && stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		if (G::LocalPlayer && G::LocalPlayer->GetAlive())
		{
			for (int i = 0; i <= I::Globals->maxClients; i++)
			{
				CBaseEntity* Entity = I::ClientEntList->GetClientEntity(i);

				if (!Entity
					|| Entity == G::LocalPlayer
					|| Entity->GetTeam() == G::LocalPlayer->GetTeam()
					|| Entity->GetHealth() <= 0
					|| Entity->GetDormant()
					|| Entity->GetImmune())
					continue;

				player_info_t entityInformation;
				I::Engine->GetPlayerInfo(i, &entityInformation);

				if (std::find(CPlayerList::Players.begin(), CPlayerList::Players.end(), entityInformation.userid) != CPlayerList::Players.end())
					continue;

				player_data.push_back(PlayerAA(Entity, *Entity->GetHeadRotation()));

				if (Vars.Ragebot.Antiaim.Resolver == 1)
					Entity->GetHeadRotation()->y = *Entity->GetLowerBodyYawTarget();
				else if (Vars.Ragebot.Antiaim.Resolver == 2)
					LBYthings::lbydelta(Entity);

				if (Entity->GetHeadRotation()->y > 180.0f)
					Entity->GetHeadRotation()->y -= 360.0f;
				if (Entity->GetHeadRotation()->y < -180.0f)
					Entity->GetHeadRotation()->y += 360.0f;

				if (Vars.Players.AAA.PAngle[i] > 0)
				{
					if (Vars.Players.AAA.PAngle[i] == 1)
						Entity->GetHeadRotation()->x = 89.f;
					else if (Vars.Players.AAA.PAngle[i] == 2)
						Entity->GetHeadRotation()->x = -89.f;
					else if (Vars.Players.AAA.PAngle[i] == 3)
						Entity->GetHeadRotation()->x = 0.f;
					else if (Vars.Players.AAA.PAngle[i] == 4)
						Entity->GetHeadRotation()->x = 70.f;
				}

				QAngle eyeAngles = *Entity->GetHeadRotation();

				if (Vars.Players.AAA.YAngle[i] > 0/*&& (std::find(CPlayerList::FixPitch.begin(), CPlayerList::FixPitch.end(), entityInformation.userid) == CPlayerList::FixPitch.end())*/)
				{
					if (Vars.Players.AAA.YAngle[i] == 1)
						Entity->GetHeadRotation()->y = Entity->GetHeadRotation()->y - 179.f;
					else if (Vars.Players.AAA.YAngle[i] == 2)
					{
						if (Entity->GetHeadRotation()->y > 180 && Entity->GetHeadRotation()->y < -180)
							Entity->GetHeadRotation()->y = 90.0f;
					}
					else if (Vars.Players.AAA.YAngle[i] > 2)
					{
						bool lby2 = Vars.Players.AAA.YAngle[i] == 4 ? true : false;
						Entity->GetHeadRotation()->y = LBYResolver(Entity, lby2);
					}
				}

				if (Vars.Ragebot.ResolverHelper > 0)
				{
					if (Vars.Ragebot.ResolverHelper == 1)
					{
						if (Entity->GetHeadRotation()->x < -179.648438f || Entity->GetHeadRotation()->x > 179.648438f)
							Entity->GetHeadRotation()->x = -10.0f;

						if (Entity->GetHeadRotation()->x <= -88.945313f && Entity->GetHeadRotation()->x >= -179.648438f)
							Entity->GetHeadRotation()->x = -89.0f;

						if (Entity->GetHeadRotation()->x >= 88.945313f && Entity->GetHeadRotation()->x <= 179.648438f)
							Entity->GetHeadRotation()->x = 89.0f;

					}
					if (Vars.Ragebot.ResolverHelper > 1 && Vars.Players.AAA.PAngle[i] == 0)
					{
						if (eyeAngles.x < -179.f) eyeAngles.x += 360.f;
						else if (eyeAngles.x > 90.0 || eyeAngles.x < -90.0) eyeAngles.x = 89.f;
						else if (eyeAngles.x > 89.0 && eyeAngles.x < 91.0) eyeAngles.x -= 90.f;
						else if (eyeAngles.x > 179.0 && eyeAngles.x < 181.0) eyeAngles.x -= 180;
						else if (eyeAngles.x > -179.0 && eyeAngles.x < -181.0) eyeAngles.x += 180;
						else if (fabs(eyeAngles.x) == 0) eyeAngles.x = std::copysign(89.0f, eyeAngles.x);
					}
					if (Vars.Ragebot.ResolverHelper > 2)
					{
						if (Vars.Ragebot.Antiaim.Resolver == 1)
						{
							if ((*Entity->GetLowerBodyYawTarget() != Entity->GetHeadRotation()->y) && (Entity->GetVelocity().Length2D() > 0.1f && Entity->GetFlags() & FL_ONGROUND))
							{
								Entity->GetHeadRotation()->y = Entity->GetHeadRotation()->y + U::RandomFloat(-35, 35);
							}
						}
						if (Vars.Ragebot.Antiaim.Resolver == 2)
						{
							//Entity->GetHeadRotation()->y -= U::RandomFloat(-35, 35);
						}
					}
				}
			}
		}
	}
	//* Resolver *//

	QAngle aim_punch_old;
	QAngle view_punch_old;

	QAngle* aim_punch = nullptr;
	QAngle* view_punch = nullptr;

	if (stage == FRAME_RENDER_START)
	{
		if (*(bool*)((DWORD)I::Input + 0xA5)) {
			*(QAngle*)((DWORD)G::LocalPlayer + offsets.deadflag + 4) = G::VisualAngle;
		}
	}

	if (I::Engine->IsInGame() && stage == FRAME_RENDER_START && Vars.Visuals.RemovalsVisualRecoil)
	{
		if (G::LocalPlayer && G::LocalPlayer->GetAlive())
		{
			aim_punch = (QAngle*)((DWORD)G::LocalPlayer + offsets.m_aimPunchAngle);
			view_punch = (QAngle*)((DWORD)G::LocalPlayer + offsets.m_viewPunchAngle);

			aim_punch_old = *aim_punch;
			view_punch_old = *view_punch;

			*aim_punch = QAngle(0, 0, 0);
			*view_punch = QAngle(0, 0, 0);
		}
	}

	//SkinChanger::FrameStageNotify(stage);

	//ThirdPerson::FrameStageNotify(stage);
	SkinChanger::FrameStageNotify(stage);
	oFrameStageNotify(stage);


	if (aim_punch && view_punch && Vars.Visuals.RemovalsVisualRecoil)
	{
		*aim_punch = aim_punch_old;
		*view_punch = view_punch_old;
	}
	//return oFrameStageNotify(stage);
}
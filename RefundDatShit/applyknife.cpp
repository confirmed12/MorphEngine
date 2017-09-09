#include "Cheat.h"
#include "SkinChanger.h"
#include <iostream>
#include <memory>

RecvVarProxyFn oRecvnModelIndex;

void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	// Get the knife view model id's
	int default_t = I::ModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	int default_ct = I::ModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	int iBayonet = I::ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	int iButterfly = I::ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
	int iFlip = I::ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
	int iGut = I::ModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
	int iKarambit = I::ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
	int iM9Bayonet = I::ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	int iHuntsman = I::ModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
	int iFalchion = I::ModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
	int iDagger = I::ModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
	int iBowie = I::ModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
	int iGunGame = I::ModelInfo->GetModelIndex("models/weapons/v_knife_gg.mdl");



	// Get local player (just to stop replacing spectators knifes)
	CBaseEntity* pLocal = I::ClientEntList->GetClientEntity(I::Engine->GetLocalPlayer());
	if (Vars.Skins.SSEnabled && pLocal)
	{

		// If we are alive and holding a default knife(if we already have a knife don't worry about changing)
		if (pLocal->GetAlive() && (
			pData->m_Value.m_Int == default_t ||
			pData->m_Value.m_Int == default_ct ||
			pData->m_Value.m_Int == iBayonet ||
			pData->m_Value.m_Int == iButterfly ||
			pData->m_Value.m_Int == iFlip ||
			pData->m_Value.m_Int == iGunGame ||
			pData->m_Value.m_Int == iGut ||
			pData->m_Value.m_Int == iKarambit ||
			pData->m_Value.m_Int == iM9Bayonet ||
			pData->m_Value.m_Int == iHuntsman ||
			pData->m_Value.m_Int == iFalchion ||
			pData->m_Value.m_Int == iDagger ||
			pData->m_Value.m_Int == iBowie))
		{
			// Set whatever knife we want

			if (Vars.Skins.KnifeModel == 0)
				pData->m_Value.m_Int = G::LocalPlayer->GetTeam() == 2 ? default_t : default_ct;
			else if (Vars.Skins.KnifeModel == 1) {
				pData->m_Value.m_Int = iBayonet;
			}
			else if (Vars.Skins.KnifeModel == 2) {
				pData->m_Value.m_Int = iFlip;
			}
			else if (Vars.Skins.KnifeModel == 3) {
				pData->m_Value.m_Int = iGut;
			}
			else if (Vars.Skins.KnifeModel == 4) {
				pData->m_Value.m_Int = iKarambit;
			}
			else if (Vars.Skins.KnifeModel == 5) {
				pData->m_Value.m_Int = iM9Bayonet;
			}
			else if (Vars.Skins.KnifeModel == 6) {
				pData->m_Value.m_Int = iHuntsman;
			}
			else if (Vars.Skins.KnifeModel == 7) {
				pData->m_Value.m_Int = iButterfly;
			}
			else if (Vars.Skins.KnifeModel == 8) {
				pData->m_Value.m_Int = iFalchion;
			}
			else if (Vars.Skins.KnifeModel == 9) {
				pData->m_Value.m_Int = iDagger;
			}
			else if (Vars.Skins.KnifeModel == 10) {
				pData->m_Value.m_Int = iBowie;
			}

		}
	}

	// Carry on the to original proxy
	oRecvnModelIndex(pData, pStruct, pOut);
}


#define SEQUENCE_DEFAULT_DRAW 0
#define SEQUENCE_DEFAULT_IDLE1 1
#define SEQUENCE_DEFAULT_IDLE2 2
#define SEQUENCE_DEFAULT_LIGHT_MISS1 3
#define SEQUENCE_DEFAULT_LIGHT_MISS2 4
#define SEQUENCE_DEFAULT_HEAVY_MISS1 9
#define SEQUENCE_DEFAULT_HEAVY_HIT1 10
#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB 11
#define SEQUENCE_DEFAULT_LOOKAT01 12

#define SEQUENCE_BUTTERFLY_DRAW 0
#define SEQUENCE_BUTTERFLY_DRAW2 1
#define SEQUENCE_BUTTERFLY_LOOKAT01 13
#define SEQUENCE_BUTTERFLY_LOOKAT03 15

#define SEQUENCE_FALCHION_IDLE1 1
#define SEQUENCE_FALCHION_HEAVY_MISS1 8
#define SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP 9
#define SEQUENCE_FALCHION_LOOKAT01 12
#define SEQUENCE_FALCHION_LOOKAT02 13

#define SEQUENCE_DAGGERS_IDLE1 1
#define SEQUENCE_DAGGERS_LIGHT_MISS1 2
#define SEQUENCE_DAGGERS_LIGHT_MISS5 6
#define SEQUENCE_DAGGERS_HEAVY_MISS2 11
#define SEQUENCE_DAGGERS_HEAVY_MISS1 12

#define SEQUENCE_BOWIE_IDLE1 1

#define	LIFE_ALIVE 0

std::unique_ptr<RecvPropHook> sequenceHook;

RecvVarProxyFn oRecvProxySequence;
void RecvProxy_Sequence(CRecvProxyData* pDataConst, void* pStruct, void* pOut)
{
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
	static int lastSentSeq;
	CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);

	int iSequence = pData->m_Value.m_Int;

	CBaseCombatWeapon* Weapon = static_cast<CBaseCombatWeapon*>(pStruct);
	CBaseEntity* pLocal = I::ClientEntList->GetClientEntity(I::Engine->GetLocalPlayer());

	if (Weapon && Weapon->GetModelIndex() && pLocal)
	{
		std::string rWeapon = I::ModelInfo->GetModelName(Weapon->GetModel());

		if (rWeapon.find(XorStr("v_knife_push.mdl")) != std::string::npos)
		{
			switch (iSequence) {
			case SEQUENCE_DEFAULT_IDLE2:
				iSequence = SEQUENCE_DAGGERS_IDLE1; break;
			case SEQUENCE_DEFAULT_LIGHT_MISS1:
			case SEQUENCE_DEFAULT_LIGHT_MISS2:
				iSequence = RandomInt(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5); break;
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				iSequence = RandomInt(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1); break;
			case SEQUENCE_DEFAULT_HEAVY_HIT1:
			case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
			case SEQUENCE_DEFAULT_LOOKAT01:
				iSequence += 3; break;
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				break;
			default:
				iSequence += 2;
			}
		}
		else if (rWeapon.find(XorStr("v_knife_falchion_advanced.mdl")) != std::string::npos)
		{
			switch (iSequence) {
			case SEQUENCE_DEFAULT_IDLE2:
				iSequence = SEQUENCE_FALCHION_IDLE1; break;
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				iSequence = RandomInt(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP); break;
			case SEQUENCE_DEFAULT_LOOKAT01:
				iSequence = RandomInt(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02); break;
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				break;
			default:
				iSequence--;
			}
		}
		else if (rWeapon.find(XorStr("v_knife_butterfly.mdl")) != std::string::npos)
		{
			switch (iSequence) {
			case SEQUENCE_DEFAULT_DRAW:
				iSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2); break;
			case SEQUENCE_DEFAULT_LOOKAT01:
				iSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03); break;
			default:
				iSequence++;
			}
		}
		else if (rWeapon.find(XorStr("v_knife_survival_bowie.mdl")) != std::string::npos)
		{
			switch (iSequence) {
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				break;
			case SEQUENCE_DEFAULT_IDLE2:
				iSequence = SEQUENCE_BOWIE_IDLE1; break;
			default:
				iSequence--;
			}
		}
		pData->m_Value.m_Int = iSequence;
	}

	if (!oRecvProxySequence)
		oRecvProxySequence = sequenceHook->GetOriginalFunction();

	oRecvProxySequence(pData, pStruct, pOut);
}

RecvVarProxyFn oRecvProxyMusicID;
void Hooked_RecvProxy_MusicID(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	//pData->m_Value.m_Int = Vars.Ragebot.AimstepAmount;

	oRecvProxyMusicID(pData, pStruct, pOut);
}
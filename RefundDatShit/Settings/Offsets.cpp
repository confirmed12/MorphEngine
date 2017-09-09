#pragma once

#include "../SDK.h"

COffsets offsets;

void Offsets::GrabOffsets()
{
	offsets.m_iHealth = U::NetVars->GetOffset( XorStr( "DT_CSPlayer" ), XorStr( "m_iHealth" ) );
	offsets.m_iTeamNum = U::NetVars->GetOffset( XorStr( "DT_CSPlayer" ), XorStr( "m_iTeamNum" ) );
	offsets.m_ArmorValue = U::NetVars->GetOffset( XorStr( "DT_CSPlayer" ), XorStr( "m_ArmorValue" ) );
	offsets.m_bHasHelmet = U::NetVars->GetOffset( XorStr( "DT_CSPlayer" ), XorStr( "m_bHasHelmet" ) );
	offsets.m_iClass = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_iClass"));
	offsets.m_bDormant = 0xE9;
	offsets.m_bGunGameImmunity = U::NetVars->GetOffset( XorStr( "DT_CSPlayer" ), XorStr( "m_bGunGameImmunity" ) );
	offsets.m_lifeState = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_lifeState"));
	offsets.m_iCompetitiveRanking = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_iCompetitiveRanking"));
	//offsets.m_nMusicID = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_nMusicID"));
	offsets.m_flSimulationTime = U::NetVars->GetOffset(XorStr("DT_BaseEntity"), XorStr("m_flSimulationTime"));
	offsets.m_fFlags = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_fFlags"));
	offsets.deadflag = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("deadflag"));
	offsets.m_Local = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_Local"));
	offsets.m_nTickBase = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_nTickBase"));
	//offsets.m_iDefaultFOV = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_iDefaultFOV"));
	offsets.m_nForceBone = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_nForceBone"));
	offsets.m_nSequence = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_nSequence"));
	offsets.m_hMyWeapons = U::NetVars->GetOffset(XorStr("DT_BaseCombatCharacter"), XorStr("m_hMyWeapons"));
	offsets.m_hMyWearables = U::NetVars->GetOffset(XorStr("DT_BaseCombatCharacter"), XorStr("m_hMyWearables"));
	offsets.m_angEyeAngles = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_angEyeAngles"));
	offsets.m_flFlashDuration = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_flFlashDuration"));
	//offsets.m_iPlayerC4 = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_iPlayerC4"));
	offsets.m_iGlowIndex = offsets.m_flFlashDuration + 0x18;
	offsets.m_mBoneMatrix = offsets.m_nForceBone + 0x1C;
	offsets.m_nModelIndex = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_nModelIndex"));
	offsets.m_viewPunchAngle = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_viewPunchAngle"));
	offsets.m_aimPunchAngle = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_aimPunchAngle"));
	offsets.m_vecOrigin = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_vecOrigin"));
	offsets.m_ViewModel = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_hViewModel[0]"));
	offsets.m_bIsScoped = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_bIsScoped"));
	offsets.m_hObserverTarget = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_hObserverTarget"));
	offsets.m_angRotation = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_angEyeAngles[0]"));
	offsets.m_dwGlowObject = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_dwGlowObject"));
	offsets.m_flLowerBodyYawTarget = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_flLowerBodyYawTarget"));
	offsets.m_vecViewOffset = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_vecViewOffset[0]"));
	offsets.m_vecVelocity = U::NetVars->GetOffset(XorStr( "DT_CSPlayer" ), XorStr( "m_vecVelocity[0]" ) );
	offsets.m_szLastPlaceName = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_szLastPlaceName"));
	offsets.m_hActiveWeapon = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_hActiveWeapon"));
	offsets.m_hActiveWeaponz = U::NetVars->GetOffset(XorStr("DT_BaseCombatCharacter"), ("m_hActiveWeapon"));
	offsets.m_fAccuracyPenalty = U::NetVars->GetOffset(XorStr("DT_WeaponCSBase"), XorStr("m_fAccuracyPenalty"));

	offsets.m_Collision = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_Collision"));
	offsets.m_CollisionGroup = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_CollisionGroup"));
	offsets.m_iShotsFired = U::NetVars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_iShotsFired"));
	offsets.m_iWeaponID = U::NetVars->GetOffset(XorStr("DT_WeaponCSBase"), XorStr("m_fAccuracyPenalty")) + 0x30;
	offsets.m_hOwnerEntity = U::NetVars->GetOffset(XorStr("DT_BaseEntity"), XorStr("m_hOwnerEntity"));
	offsets.m_nMoveType = 0x258;
	offsets.m_nHitboxSet = U::NetVars->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_nHitboxSet"));
	offsets.m_bIsBroken = U::NetVars->GetOffset(XorStr("DT_BreakableSurface"), XorStr("m_bIsBroken"));
	offsets.m_flC4Blow = U::NetVars->GetOffset(XorStr("DT_PlantedC4"), XorStr("m_flC4Blow"));


	offsets.m_hOwner = U::NetVars->GetOffset(XorStr("DT_BaseViewModel"), XorStr("m_hOwner"));
	offsets.m_hWeapon = U::NetVars->GetOffset(XorStr("DT_BaseViewModel"), XorStr("m_hWeapon"));

	offsets.m_bReloadVisuallyComplete = U::NetVars->GetOffset(XorStr("DT_WeaponCSBase"), XorStr("m_bReloadVisuallyComplete"));
	offsets.m_flNextPrimaryAttack = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_flNextPrimaryAttack"));
	offsets.m_nFallbackPaintKit = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_nFallbackPaintKit"));
	offsets.m_nFallbackSeed = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_nFallbackSeed"));
	offsets.m_flFallbackWear = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_flFallbackWear"));
	offsets.m_nFallbackStatTrak = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_nFallbackStatTrak"));
	offsets.m_AttributeManager = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_AttributeManager"));
	offsets.m_nViewModelIndex = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_nViewModelIndex"));
	offsets.m_iWorldModelIndex = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_iWorldModelIndex"));
	offsets.m_Item = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_Item"));
	offsets.m_iEntityLevel = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_iEntityLevel"));
	offsets.m_iItemIDHigh = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_iItemIDHigh"));
	offsets.m_iItemIDLow = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_iItemIDLow"));
	offsets.m_iAccountID = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_iAccountID"));
	offsets.m_iEntityQuality = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_iEntityQuality"));
	offsets.m_iClip1 = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_iClip1"));
	offsets.m_iClip2 = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_iPrimaryReserveAmmoCount"));
	offsets.m_flPostponeFireReadyTime = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_flPostponeFireReadyTime"));
	offsets.m_OriginalOwnerXuidLow = U::NetVars->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_OriginalOwnerXuidLow"));
	offsets.m_OriginalOwnerXuidHigh = U::NetVars->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_OriginalOwnerXuidHigh"));
	offsets.m_iItemDefinitionIndex = U::NetVars->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_iItemDefinitionIndex"));

	offsets.CalcPlayerView = U::FindPattern(strenc("client.dll"), strenc(" 84 C0 75 08 57 8B CE E8 ?? ?? ?? ?? 8B 06"));
	//offsets.GlowManager = *( DWORD* )( Utils::FindPattern( strenc( "client.dll" ), strenc( "A1 ? ? ? ? 3B 81 ? ? ? ? 7E 63")) + 0x5  );
	offsets.d3d9Device = **(DWORD**)(U::FindPattern(strenc("shaderapidx9.dll"), strenc("A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C")) + 0x1);
	offsets.LoadFromBufferEx = U::FindPattern(strenc("client.dll"), strenc("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04"));
	offsets.InitKeyValuesEx = U::FindPattern(strenc("client.dll"), strenc("55 8B EC 51 33 C0 C7 45"));
	offsets.ServerRankRevealAllEx = U::FindPattern(strenc("client.dll"), strenc("55 8B EC 8B 0D ? ? ? ? 68 ? ? ? ? "));
	offsets.ClipTraceToPlayersFn = U::FindPattern(strenc("client.dll"), strenc("53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 10"));
}

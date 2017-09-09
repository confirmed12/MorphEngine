#pragma once

#include "Common.h"

struct COffsets
{
	DWORD m_iHealth;
	DWORD m_iTeamNum;
	DWORD m_bDormant;
	DWORD m_bGunGameImmunity;
	DWORD m_lifeState;
	DWORD deadflag;
	DWORD m_ViewModel;
	DWORD m_fFlags;
	DWORD m_Local;
	DWORD m_nTickBase;
	DWORD m_nForceBone;
	DWORD m_hMyWeapons;
	DWORD m_nSequence;
	DWORD m_iDefaultFOV;
	DWORD m_mBoneMatrix;
	DWORD m_nModelIndex;
	DWORD m_viewPunchAngle;
	DWORD m_aimPunchAngle;
	DWORD m_vecOrigin;
	DWORD m_vecViewOffset;
	DWORD m_vecVelocity;
	DWORD m_szLastPlaceName;
	DWORD m_flNextPrimaryAttack;
	DWORD m_hActiveWeapon;
	DWORD m_hActiveWeaponz;
	DWORD m_fAccuracyPenalty;
	DWORD m_fInaccuracy;
	DWORD m_Collision;
	DWORD m_fSpread;
	DWORD m_iShotsFired;
	DWORD m_iWeaponID;
	DWORD m_nMoveType;
	DWORD m_nHitboxSet;
	DWORD m_bHasHelmet;
	DWORD m_ArmorValue;
	DWORD m_CollisionGroup;
	DWORD m_hMyWearables;
	DWORD m_iClass;
	//DWORD m_dwSensitivity;
	DWORD m_bIsBroken;
	DWORD m_angEyeAngles;
	DWORD m_hOwnerEntity;
	DWORD m_flC4Blow;
	DWORD m_flFlashDuration;
	DWORD m_iGlowIndex;
	DWORD m_flSimulationTime;

	DWORD m_nFallbackPaintKit;
	DWORD m_nFallbackSeed;
	DWORD m_iCompetitiveRanking;
	DWORD m_flFallbackWear;
	DWORD m_nFallbackStatTrak;
	DWORD m_AttributeManager;
	DWORD m_Item;
	DWORD m_iEntityLevel;
	DWORD m_iItemIDHigh;
	DWORD m_iItemIDLow;
	DWORD m_iAccountID;
	DWORD m_iPlayerC4;
	DWORD m_bIsValveDS;
	DWORD m_nMusicID;
	DWORD m_nViewModelIndex;
	DWORD m_iWorldModelIndex;
	DWORD m_iEntityQuality;
	DWORD m_OriginalOwnerXuidLow;
	DWORD m_OriginalOwnerXuidHigh;
	DWORD m_iItemDefinitionIndex;
	DWORD m_iClip1;
	DWORD m_iClip2;
	DWORD m_flPostponeFireReadyTime;
	DWORD m_hWeapon;
	DWORD m_hOwner;
	DWORD m_bReloadVisuallyComplete;
	DWORD m_bIsScoped;
	DWORD m_hObserverTarget;
	DWORD m_angRotation;
	DWORD m_dwGlowObject;
	DWORD m_flLowerBodyYawTarget;

	//sigs
	DWORD CalcPlayerView;
	DWORD d3d9Device;
	DWORD SendPacket;
	DWORD GlowManager;
	DWORD LoadFromBufferEx;
	DWORD InitKeyValuesEx;
	DWORD ServerRankRevealAllEx;
	DWORD Linetosmoke;
	DWORD IsReadyEx;
	DWORD ClipTraceToPlayersFn;
}; 
extern COffsets offsets;

namespace Offsets
{
	extern void GrabOffsets( );
}
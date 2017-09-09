#include "../SDK.h"

int CBaseEntity::GetIndex()
{
	return this->index;
}

int	CBaseEntity::GetHealth( )
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( int* ) ( ( DWORD )this + offsets.m_iHealth );
}

int CBaseEntity::GetTeam( )
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( int* ) ( ( DWORD )this + offsets.m_iTeamNum );
}

int CBaseEntity::GetFlags( )
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( int* ) ( ( DWORD )this + offsets.m_fFlags );
}

int CBaseEntity::GetTickBase( )
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( int* ) ( ( DWORD )this + offsets.m_nTickBase );
}

int CBaseEntity::GetShotsFired()
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( int* ) ( ( DWORD )this + offsets.m_iShotsFired );
}

int CBaseEntity::GetMoveType( )
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( int* ) ( ( DWORD )this + offsets.m_nMoveType );
}

int *CBaseEntity::GetModelIndex()
{
	if (!I::Engine->IsConnected())
		return 0;

	return ( int* )( ( DWORD )this + offsets.m_nModelIndex );
}

int CBaseEntity::GetHitboxSet()
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( int* )( ( DWORD )this + offsets.m_nHitboxSet );
}

int CBaseEntity::GetUserID()
{
	if (!I::Engine->IsConnected())
		return 0;

	return this->GetPlayerInfo().userid;
}

int CBaseEntity::GetArmor()
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( int* )( ( DWORD )this + offsets.m_ArmorValue );
}

int CBaseEntity::PhysicsSolidMaskForEntity()
{
	typedef unsigned int( __thiscall* OriginalFn )( void* );
	return U::GetVFunc<OriginalFn>( this, 154 )( this );
}

int CBaseEntity::GetOwner()
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( int* )( ( DWORD )this + offsets.m_hOwnerEntity );
}

int CBaseEntity::GetGlowIndex()
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( int* )( ( DWORD )this + offsets.m_iGlowIndex );
}

float CBaseEntity::GetBombTimer()
{
	if (!I::Engine->IsConnected())
		return 0.f;

	float bombTime = *( float* )( ( DWORD )this + offsets.m_flC4Blow );
	float returnValue = bombTime - I::Globals->curtime;
	return ( returnValue < 0 ) ? 0.f : returnValue;
}

float CBaseEntity::GetFlashDuration()
{
	if (!I::Engine->IsConnected())
		return 0.f;

	return *( float* )( ( DWORD )this + offsets.m_flFlashDuration );
}

bool CBaseEntity::IsFlashed()
{
	if (!I::Engine->IsConnected())
		return false;

	return GetFlashDuration() > 0 ? true : false;
}


int* CBaseEntity::GetWeapons()
{
	if (!I::Engine->IsConnected())
		return 0;

	return (int*)((DWORD)this + 0x2DE8);
}

int CBaseEntity::GetPlayerC4()
{
	if (!I::Engine->IsConnected())
		return 0;

	return *(int*)((uintptr_t)this + offsets.m_iPlayerC4);
}

bool CBaseEntity::GetAlive()
{
	if (!I::Engine->IsConnected())
		return false;
	if (this->GetHealth() <= 0)
		return false;
	if (this->GetLifeState() == false)
		return false;

	return true;
}

bool CBaseEntity::GetLifeState()
{
	if (!I::Engine->IsConnected())
		return false;

	return (bool)(*(int*)((DWORD)this + offsets.m_lifeState) == 0);
}

QAngle* CBaseEntity::GetVAngles()
{
	return (QAngle*)((uintptr_t)this + offsets.deadflag + 0x4);
}

CBaseEntity* CBaseEntity::GetViewModel()
{
	return (CBaseEntity*)((CBaseEntity*)(int*)this + offsets.m_ViewModel);
}

bool CBaseEntity::GetDormant( )
{
	if (!I::Engine->IsConnected())
		return true;

	return *( bool* ) ( ( DWORD )this + offsets.m_bDormant );
}

bool CBaseEntity::GetImmune( )
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( bool* ) ( ( DWORD )this + offsets.m_bGunGameImmunity );
}

bool CBaseEntity::IsScoped()
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( bool* )( ( DWORD )this + offsets.m_bIsScoped );
}

bool CBaseEntity::HasHelmet()
{
	if (!I::Engine->IsConnected())
		return 0;

	return *( bool* )( ( DWORD )this + offsets.m_bHasHelmet );
}

bool CBaseEntity::IsTargetingLocal()
{
	if (!I::Engine->IsConnected())
		return 0;

	Vector src, dst, forward;
	trace_t tr;

	if( !this )
		return false;

	QAngle viewangle = this->GetEyeAngles();

	M::AngleVectors( viewangle, &forward );
	forward *= 8142.f;
	src = this->GetEyePosition();
	dst = src + forward;

	U::TraceLine( src, dst, MASK_SHOT, this, &tr );

	if( tr.m_pEnt == G::LocalPlayer )
		return true;

	return false;
}

model_t* CBaseEntity::GetModel()
{
	if (!I::Engine->IsConnected())
		return 0;

	return *(model_t**)((DWORD)this + 0x6C);
}

bool CBaseEntity::IsEnemy()
{
	if (!I::Engine->IsConnected())
		return false;

	return ( this->GetTeam() == G::LocalPlayer->GetTeam() || this->GetTeam() == 0 || this->GetIndex() == G::LocalPlayer->GetIndex()) ? false : true;
}

bool CBaseEntity::IsVisible(int bone)
{
	Vector e_vecHead = this->GetBonePosition(bone);
	Vector p_vecHead = G::LocalPlayer->GetEyePosition();

	Ray_t ray;
	trace_t tr;
	ray.Init(p_vecHead, e_vecHead);
	CTraceFilter traceFilter;
	traceFilter.pSkip = G::LocalPlayer;
	I::EngineTrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

	return (tr.m_pEnt == this || tr.IsVisible());
}

bool CBaseEntity::IsBroken()
{
	return *(bool*)((DWORD)this + offsets.m_bIsBroken);
}

QAngle CBaseEntity::GetViewPunch()
{
	return *(QAngle*)((DWORD)this + offsets.m_viewPunchAngle);
}

QAngle CBaseEntity::GetPunch()
{
	return *(QAngle*)((DWORD)this + offsets.m_aimPunchAngle);
}

QAngle CBaseEntity::GetEyeAngles()
{
	return *(QAngle*)((DWORD)this + offsets.m_angEyeAngles);
}

QAngle* CBaseEntity::GetHeadRotation()
{
	return (QAngle*)((DWORD)this + offsets.m_angEyeAngles);//m_angRotation);
}

float* CBaseEntity::GetLowerBodyYawTarget()
{
	return ( float* )(( DWORD )this + offsets.m_flLowerBodyYawTarget);
}

float& CBaseEntity::GetSimulationTime()
{
	return *(float*)((DWORD)this + offsets.m_flSimulationTime);
}

Vector CBaseEntity::GetOrigin()
{
	return *(Vector*)((DWORD)this + offsets.m_vecOrigin);
}

Vector CBaseEntity::GetAbsOrigin() {
	__asm {
		MOV ECX, this
		MOV EAX, DWORD PTR DS : [ECX]
		CALL DWORD PTR DS : [EAX + 0x28]
	}
}

Vector CBaseEntity::GetEyePosition()
{
	Vector origin = this->GetOrigin();
	Vector offset = *(Vector*)((DWORD)this + offsets.m_vecViewOffset);

	return(origin + offset);
}

Vector CBaseEntity::GetBonePosition(int iBone)
{
		matrix3x4_t boneMatrixes[128];
		if (this->SetupBones(boneMatrixes, 128, 0x100, 0))
		{
			matrix3x4_t boneMatrix = boneMatrixes[iBone];
			return Vector(boneMatrix.m_flMatVal[0][3], boneMatrix.m_flMatVal[1][3], boneMatrix.m_flMatVal[2][3]);
		}
		return Vector(0, 0, 0);
}

bool CBaseEntity::SetupBones( matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime )
{
	__asm
	{
		mov edi, this
		lea ecx, dword ptr ds : [edi + 0x4]
		mov edx, dword ptr ds : [ecx]
		push currentTime
		push boneMask
		push nMaxBones
		push pBoneToWorldOut
		call dword ptr ds : [edx + 0x34]
	}
}
/*
bool CBaseEntity::HandleBoneSetup(int boneMask, matrix3x4_t * boneOut)
{
	studiohdr_t* hdr = I::ModelInfo->GetStudioModel(this->GetModel());

	if (!hdr)
		return false;

	CBoneAccessor* boneAccessor = this->GetBoneAccessor();

	if (!boneAccessor)
		return false;

	matrix3x4_t* backup_matrix = boneAccessor->GetBoneArrayForWrite();

	if (!backup_matrix)
		return false;

	Vector origin = this->GetOrigin();
	Vector angles = m_vRealAngles[this->GetIndex()];

	// back up interpolated data.

	Vector backup_absorigin = this->GetAbsOrigin();
	Vector backup_absangles = this->GetEyePosition();

	float backup_poses[24];
	target->GetPoseParameters(backup_poses);

	C_AnimLayer backup_layers[15];
	target->GetAnimLayers(backup_layers);

	// build matrix using un-interpolated server data.

	matrix3x4_t parentTransform;

	g_pMath->AngleMatrix(angles, origin, parentTransform);

	// if you wanna get these right, create your own animation state and feed it uninterpolated data.

	float* poses_slot = m_flPoseParams[target->GetIndex()];
	C_AnimLayer* layers_slot = m_vAnimLayers[target->GetIndex()];

	target->SetAbsOrigin(origin);
	target->SetAbsAngles(angles);
	target->SetPoseParameters(poses_slot);
	target->SetAnimLayers(layers_slot);

	// compute and build bones.

	Vector pos[128];
	Quaternion q[128];

	target->StandardBlendingRules(hdr, pos, q, I::Globals->curtime, boneMask);

	boneAccessor->SetBoneArrayForWrite(boneOut);

	byte computed[0x100] = { 0 };

	target->BuildTransformations(hdr, pos, q, parentTransform, boneMask, computed);

	boneAccessor->SetBoneArrayForWrite(backup_matrix);

	// restore original entity data.

	target->SetAbsOrigin(backup_absorigin);
	target->SetAbsAngles(backup_absangles);
	target->SetPoseParameters(backup_poses);
	target->SetAnimLayers(backup_layers);

	return true;
}
*/
Vector CBaseEntity::GetVelocity()
{
	return *(Vector*)((DWORD)this + offsets.m_vecVelocity);
}

Vector CBaseEntity::GetPredicted(Vector p0)
{
	return M::ExtrapolateTick(p0, this->GetVelocity());
}

ICollideable* CBaseEntity::GetCollideable()
{
	return ( ICollideable* ) ( ( DWORD )this + offsets.m_Collision );
}

player_info_t CBaseEntity::GetPlayerInfo()
{
	player_info_t pinfo;
	I::Engine->GetPlayerInfo( this->index, &pinfo );
	return pinfo;
}

std::string	CBaseEntity::GetName()
{
	return this->GetPlayerInfo().name;
}

std::string	CBaseEntity::GetSteamID()
{
	return this->GetPlayerInfo().name;
}

std::string CBaseEntity::GetLastPlace()
{
	return std::string( ( const char* )this + offsets.m_szLastPlaceName );
}

HANDLE CBaseEntity::GetObserverTargetHandle()
{
	return *(HANDLE*)((DWORD)this + offsets.m_hObserverTarget);
}

void CBaseEntity::PreDataUpdate(DataUpdateType_t updateType)
{
	PVOID pNetworkable = (PVOID)((DWORD)(this) + 0x8);
	typedef void(__thiscall* OriginalFn)(PVOID, int);
	return U::GetVFunc<OriginalFn>(pNetworkable, 6)(pNetworkable, updateType);
}

CBaseCombatWeapon* CBaseEntity::GetWeapon()
{
	if (!I::Engine->IsConnected())
		return nullptr;
	if (!this->GetAlive())
		return nullptr;

	HANDLE weaponData = *(HANDLE* )( ( DWORD )this + offsets.m_hActiveWeapon );
	return ( CBaseCombatWeapon* ) I::ClientEntList->GetClientEntityFromHandle( weaponData );
}

HANDLE CBaseCombatWeapon::GetOwnerHandle()
{
	return *(HANDLE*)((DWORD)this + offsets.m_hOwnerEntity);
}

DWORD CBaseEntity::GetActiveWeapon()
{
	return (DWORD)((uintptr_t)this + offsets.m_hActiveWeaponz);
}

HANDLE CBaseEntity::GetActiveWeaponHANDLE()
{
	return (HANDLE)((uintptr_t)this + offsets.m_hActiveWeaponz);
}

ClientClass* CBaseEntity::GetClientClass()
{
	PVOID pNetworkable = ( PVOID )( ( DWORD )( this ) + 0x8 );
	typedef ClientClass*( __thiscall* OriginalFn )( PVOID );
	return U::GetVFunc<OriginalFn>( pNetworkable, 2 )( pNetworkable );
}

int CBaseEntity::GetCompetitiveRanking(int index)
{
	static DWORD GameResources = (U::FindPattern("client.dll", "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7") + 2);

	if (GameResources != NULL)
	{
		return *(int*)(GameResources + offsets.m_iCompetitiveRanking + index * 4);
	}

	return 0;
}

int CBaseEntity::GetCollisionGroup()
{
	return *( int* ) ( ( DWORD )this + offsets.m_CollisionGroup );
}

int CBaseEntity::GetBoneByName(const char* boneName)
{
	studiohdr_t* pStudioModel = I::ModelInfo->GetStudioModel(this->GetModel() );
	if (!pStudioModel)
		return -1;

	matrix3x4_t pBoneToWorldOut[128];
	if (!this->SetupBones(pBoneToWorldOut, 128, 256, 0))
		return -1;

	for (int i = 0; i < pStudioModel->numbones; i++)
	{
		mstudiobone_t *pBone = pStudioModel->pBone(i);
		if (!pBone)
			continue;

		if (pBone->pszName() && strcmp(pBone->pszName(), boneName) == 0)
			return i;
	}

	return -1;
}

int& CBaseCombatWeapon::GetWeaponID( )
{
	return *( int* ) ( ( DWORD )this + offsets.m_iItemDefinitionIndex);
}

float& CBaseCombatWeapon::GetNextPrimaryAttack( )
{
	return *( float* ) ( ( DWORD )this + offsets.m_flNextPrimaryAttack );
}

float& CBaseCombatWeapon::GetAccuracyPenalty( )
{
	return *( float* ) ( ( DWORD )this + offsets.m_fAccuracyPenalty );
}

float& CBaseCombatWeapon::GetSpread()
{
	return *(float*)((DWORD)this + offsets.m_fSpread);
}

float CBaseCombatWeapon::GetSpreadReal() {
	DWORD dwSpreadVMT = (*reinterpret_cast< PDWORD_PTR* >(this))[439]; //460
	return ((float(__thiscall*)(CBaseCombatWeapon*)) dwSpreadVMT)(this);
}

float CBaseCombatWeapon::GetInaccuracyReal() {
	DWORD dwInaccuracyVMT = (*reinterpret_cast< PDWORD_PTR* >(this))[469]; //459
	return ((float(__thiscall*)(CBaseCombatWeapon*)) dwInaccuracyVMT)(this);
}

float& CBaseCombatWeapon::GetInaccuracy()
{
	return *(float*)((DWORD)this + offsets.m_fInaccuracy);
}

float& CBaseCombatWeapon::GetAllInaccuracy()
{
	return *(float*)((DWORD)this + (offsets.m_fInaccuracy + offsets.m_fSpread));
}

int& CBaseCombatWeapon::GetXUIDLow( )
{
	return *( int* ) ( ( DWORD )this + offsets.m_OriginalOwnerXuidLow );
}

int& CBaseCombatWeapon::GetXUIDHigh( )
{
	return *( int* ) ( ( DWORD )this + offsets.m_OriginalOwnerXuidHigh );
}

int& CBaseCombatWeapon::GetEntityQuality( )
{
	return *( int* ) ( ( DWORD )this + offsets.m_iEntityQuality );
}

int& CBaseCombatWeapon::GetAccountID( )
{
	return *( int* ) ( ( DWORD )this + offsets.m_iAccountID );
}

model_t* CBaseCombatWeapon::GetModel()
{
	return *(model_t**)((DWORD)this + 0x6C);
}

int& CBaseCombatWeapon::GetModelIndex()
{
	return *(int*)((DWORD)this + offsets.m_nModelIndex);
}

int& CBaseCombatWeapon::GetItemIDHigh( )
{
	return *( int* ) ( ( DWORD )this + offsets.m_iItemIDHigh );
}

int& CBaseCombatWeapon::GetItemIDLow()
{
	return *(int*)((DWORD)this + offsets.m_iItemIDLow);
}


int& CBaseCombatWeapon::GetItemDefinitionIndex( )
{
	return *( int* ) ( ( DWORD )this + offsets.m_iItemDefinitionIndex );
}

int& CBaseCombatWeapon::GetFallbackPaintKit( )
{
	return *( int* ) ( ( DWORD )this + offsets.m_nFallbackPaintKit );
}

int& CBaseCombatWeapon::GetViewModelIndex()
{
	return *(int*)((DWORD)this + offsets.m_nViewModelIndex);
}

int& CBaseCombatWeapon::GetWorldModelIndex()
{
	return *(int*)((DWORD)this + offsets.m_iWorldModelIndex);
}

int& CBaseCombatWeapon::GetFallbackStatTrak()
{
	return *( int* ) ( ( DWORD )this + offsets.m_nFallbackStatTrak );
}

float& CBaseCombatWeapon::GetFallbackWear()
{
	return *( float* ) ( ( DWORD )this + offsets.m_flFallbackWear );
}

float& CBaseCombatWeapon::GetflPostponeFireReadyTime()
{
	return *(float*)((DWORD)this + offsets.m_flPostponeFireReadyTime);
}


int CBaseEntity::GetSequence() {
	return *(int*)((DWORD)this + offsets.m_nSequence);
}

WeaponInfo_t* CBaseCombatWeapon::GetCSWpnData()
{
	if( !this ) return nullptr;

	typedef WeaponInfo_t*( __thiscall* OriginalFn )( void* );
	return U::GetVFunc<OriginalFn>( this, 446 )( this );
}

void CBaseEntity::SetModelIndex(int index)
{
	typedef void(__thiscall* OriginalFn)(PVOID, int);
	return U::GetVFunc<OriginalFn>(this, 75)(this, index);
}


bool CBaseCombatWeapon::IsEmpty()
{
	int clip = *( int* )( ( DWORD )this + offsets.m_iClip1 );
	return clip == 0;
}

int CBaseCombatWeapon::Clip()
{
	if (!I::Engine->IsConnected() || !this)
		return 0;

	return *(int*)((DWORD)this + offsets.m_iClip1);
}

int CBaseCombatWeapon::Clip2()
{
	if (!I::Engine->IsConnected() || !this)
		return 0;

	return *(int*)((DWORD)this + offsets.m_iClip2);
}

bool CBaseCombatWeapon::IsReloading()
{
	return *( bool* )( ( DWORD )this + 0x3235 );
}

void CBaseCombatWeapon::UpdateAccuracyPenalty()
{
	DWORD dwInaccuracyVMT = (*reinterpret_cast< PDWORD_PTR* >(this))[470]; //459
	return ((void(__thiscall*)(CBaseCombatWeapon*)) dwInaccuracyVMT)(this);
}

float CBaseCombatWeapon::GetWeaponCone()
{
	if( !this )
		return 0.f;

	typedef float( __thiscall* OriginalFn )( void* );
	return U::GetVFunc<OriginalFn>( this, 477 )( this );
}

float CBaseCombatWeapon::GetWeaponSpread()
{
	if( !this )
		return 0.f;

	typedef float( __thiscall* OriginalFn )( void* );
	return U::GetVFunc<OriginalFn>( this, 478 )( this );
}

bool CBaseCombatWeapon::IsGun()
{
	if( !this )
		return false;

	int id = this->GetWeaponID();
//If your aimbot is broken, this is the reason. Just an FYI.

	switch( id )
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AUG:
	case WEAPON_AWP:
	case WEAPON_FAMAS:
	case WEAPON_G3SG1:
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_UMP45:
	case WEAPON_XM1014:
	case WEAPON_BIZON:
	case WEAPON_MAG7:
	case WEAPON_NEGEV:
	case WEAPON_SAWEDOFF:
	case WEAPON_TEC9:
	case WEAPON_TASER:
	case WEAPON_HKP2000:
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_NOVA:
	case WEAPON_P250:
	case WEAPON_SCAR20:
	case WEAPON_SG556:
	case WEAPON_SSG08:
		return true;
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
		return false;
	case WEAPON_M4A1_SILENCER:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return true;
	default:
		return false;
	}
}


std::string CBaseCombatWeapon::GetWeaponName()
{
	if( !this )
		return "";

	int id = this->GetWeaponID();

	switch( id )
	{
	case WEAPON_DEAGLE:
		return strenc( "Desert Eagle" );
	case WEAPON_ELITE:
		return strenc( "Dual Berettas" );
	case WEAPON_FIVESEVEN:
		return strenc( "Five-SeveN" );
	case WEAPON_GLOCK:
		return strenc( "Glock-18" );
	case WEAPON_AK47:
		return strenc( "AK-47" );
	case WEAPON_AUG:
		return strenc( "AUG" );
	case WEAPON_AWP:
		return strenc( "AWP" );
	case WEAPON_FAMAS:
		return strenc( "FAMAS" );
	case WEAPON_G3SG1:
		return strenc( "G3SG1" );
	case WEAPON_GALILAR:
		return strenc( "Galil" );
	case WEAPON_M249:
		return strenc( "M249" );
	case WEAPON_M4A1:
		return strenc( "M4A1" );
	case WEAPON_MAC10:
		return strenc( "MAC-10" );
	case WEAPON_P90:
		return strenc( "P90" );
	case WEAPON_UMP45:
		return strenc( "UMP-45" );
	case WEAPON_XM1014:
		return strenc( "XM1014" );
	case WEAPON_BIZON:
		return strenc( "PP-Bizon" );
	case WEAPON_MAG7:
		return strenc( "MAG-7" );
	case WEAPON_NEGEV:
		return strenc( "Negev" );
	case WEAPON_SAWEDOFF:
		return strenc( "Sawed-Off" );
	case WEAPON_TEC9:
		return strenc( "Tec-9" );
	case WEAPON_TASER:
		return strenc( "Taser" );
	case WEAPON_HKP2000:
		return strenc( "P2000" );
	case WEAPON_MP7:
		return strenc( "MP7" );
	case WEAPON_MP9:
		return strenc( "MP9" );
	case WEAPON_NOVA:
		return strenc( "Nova" );
	case WEAPON_P250:
		return strenc( "P250" );
	case WEAPON_SCAR20:
		return strenc( "SCAR-20" );
	case WEAPON_SG556:
		return strenc( "SG 553" );
	case WEAPON_SSG08:
		return strenc( "SSG 08" );
	case WEAPON_KNIFE:
		return strenc( "Knife" );
	case WEAPON_FLASHBANG:
		return strenc( "Flashbang" );
	case WEAPON_HEGRENADE:
		return strenc( "HE Grenade" );
	case WEAPON_SMOKEGRENADE:
		return strenc( "Smoke Grenade" );
	case WEAPON_MOLOTOV:
		return strenc( "Molotov" );
	case WEAPON_DECOY:
		return strenc( "Decoy" );
	case WEAPON_INCGRENADE:
		return strenc( "Incendiary Grenade" );
	case WEAPON_C4:
		return strenc( "C4" );
	case WEAPON_KNIFE_T:
		return strenc( "Knife" );
	case WEAPON_M4A1_SILENCER:
		return strenc( "M4A1-S" );
	case WEAPON_USP_SILENCER:
		return strenc( "USP-S" );
	case WEAPON_CZ75A:
		return strenc( "CZ75-Auto" );
	case WEAPON_REVOLVER:
		return strenc( "R8 Revolver" );
	default:
		return strenc( "Knife" );
	}

	return "";
}

bool CBaseCombatWeapon::IsSniper()
{
	if( !this )
		return false;

	switch( this->GetItemDefinitionIndex() )
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AUG:
		return false;
	case WEAPON_AWP:
		return true;
	case WEAPON_FAMAS:
		return false;
	case WEAPON_G3SG1:
		return true;
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_UMP45:
	case WEAPON_XM1014:
	case WEAPON_BIZON:
	case WEAPON_MAG7:
	case WEAPON_NEGEV:
	case WEAPON_SAWEDOFF:
	case WEAPON_TEC9:
	case WEAPON_TASER:
	case WEAPON_HKP2000:
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_NOVA:
	case WEAPON_P250:
		return false;
	case WEAPON_SCAR20:
		return true;
	case WEAPON_SG556:
		return false;
	case WEAPON_SSG08:
		return true;
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return false;
	default:
		return false;
	}
}

bool CBaseCombatWeapon::IsOneShotSniper()
{
	if (!this)
		return false;

	switch (this->GetItemDefinitionIndex())
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AUG:
		return false;
	case WEAPON_AWP:
		return true;
	case WEAPON_FAMAS:
	case WEAPON_G3SG1:
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_UMP45:
	case WEAPON_XM1014:
	case WEAPON_BIZON:
	case WEAPON_MAG7:
	case WEAPON_NEGEV:
	case WEAPON_SAWEDOFF:
	case WEAPON_TEC9:
	case WEAPON_TASER:
	case WEAPON_HKP2000:
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_NOVA:
	case WEAPON_P250:
	case WEAPON_SCAR20:
	case WEAPON_SG556:
		return false;
	case WEAPON_SSG08:
		return true;
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return false;
	default:
		return false;
	}
}

bool CBaseCombatWeapon::IsRifle()
{
	if (!this)
		return false;

	switch (this->GetItemDefinitionIndex())
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
		return false;
	case WEAPON_AK47:
	case WEAPON_AUG:
		return true;
	case WEAPON_AWP:
		return false;
	case WEAPON_FAMAS:
		return true;
	case WEAPON_G3SG1:
		return false;
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
		return true;
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_UMP45:
	case WEAPON_XM1014:
	case WEAPON_BIZON:
	case WEAPON_MAG7:
		return false;
	case WEAPON_NEGEV:
		return true;
	case WEAPON_SAWEDOFF:
		return false;
	case WEAPON_TEC9:
	case WEAPON_TASER:
	case WEAPON_HKP2000:
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_NOVA:
	case WEAPON_P250:
	case WEAPON_SCAR20:
		return false;
	case WEAPON_SG556:
		return true;
	case WEAPON_SSG08:
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
		return false;
	case WEAPON_M4A1_SILENCER:
		return true;
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return false;
	default:
		return false;
	}
}

bool CBaseCombatWeapon::IsSmg()
{
	if (!this)
		return false;

	switch (this->GetItemDefinitionIndex())
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AUG:
	case WEAPON_AWP:
	case WEAPON_FAMAS:
	case WEAPON_G3SG1:
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
		return false;
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_UMP45:
		return true;
	case WEAPON_XM1014:
		return false;
	case WEAPON_BIZON:
		return true;
	case WEAPON_MAG7:
	case WEAPON_NEGEV:
	case WEAPON_SAWEDOFF:
	case WEAPON_TEC9:
	case WEAPON_TASER:
	case WEAPON_HKP2000:
		return false;
	case WEAPON_MP7:
	case WEAPON_MP9:
		return true;
	case WEAPON_NOVA:
	case WEAPON_P250:
	case WEAPON_SCAR20:
	case WEAPON_SG556:
	case WEAPON_SSG08:
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return false;
	default:
		return false;
	}
}

bool CBaseCombatWeapon::IsShotgun()
{
	if (!this)
		return false;

	switch (this->GetItemDefinitionIndex())
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AUG:
	case WEAPON_AWP:
	case WEAPON_FAMAS:
	case WEAPON_G3SG1:
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_UMP45:
		return false;
	case WEAPON_XM1014:
		return true;
	case WEAPON_BIZON:
		return false;
	case WEAPON_MAG7:
		return true;
	case WEAPON_NEGEV:
		return false;
	case WEAPON_SAWEDOFF:
		return true;
	case WEAPON_TEC9:
	case WEAPON_TASER:
	case WEAPON_HKP2000:
	case WEAPON_MP7:
	case WEAPON_MP9:
		return false;
	case WEAPON_NOVA:
		return true;
	case WEAPON_P250:
	case WEAPON_SCAR20:
	case WEAPON_SG556:
	case WEAPON_SSG08:
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return false;
	default:
		return false;
	}
}

bool CBaseCombatWeapon::IsPistol()
{
	if (!this)
		return false;

	switch (this->GetItemDefinitionIndex())
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
		return true;
	case WEAPON_AK47:
	case WEAPON_AUG:
	case WEAPON_AWP:
	case WEAPON_FAMAS:
	case WEAPON_G3SG1:
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_UMP45:
	case WEAPON_XM1014:
	case WEAPON_BIZON:
	case WEAPON_MAG7:
	case WEAPON_NEGEV:
	case WEAPON_SAWEDOFF:
		return false;
	case WEAPON_TEC9:
	case WEAPON_TASER:
	case WEAPON_HKP2000:
		return true;
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_NOVA:
		return false;
	case WEAPON_P250:
		return true;
	case WEAPON_SCAR20:
	case WEAPON_SG556:
	case WEAPON_SSG08:
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
	case WEAPON_M4A1_SILENCER:
		return false;
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return true;
	default:
		return false;
	}
}
bool CBaseCombatWeapon::IsGrenade()
{
	if( !this )
		return false;
		
	switch ( this->GetItemDefinitionIndex() )
	{
		case WEAPON_SMOKEGRENADE:
		case WEAPON_HEGRENADE:
		case WEAPON_INCGRENADE:
		case WEAPON_FLASHBANG:
		case WEAPON_MOLOTOV:
		case WEAPON_DECOY:
			return true;
		default:
			return false;
	}
}

bool CBaseCombatWeapon::IsKnife()
{
	if (!this)
		return false;

	switch ( this->GetItemDefinitionIndex() )
	{
		case WEAPON_KNIFE:
		case WEAPON_KNIFE_T:
		case WEAPON_KNIFE_GUT:
		case WEAPON_KNIFE_FLIP:
		case WEAPON_KNIFE_BAYONET:
		case WEAPON_KNIFE_M9_BAYONET:
		case WEAPON_KNIFE_KARAMBIT:
		case WEAPON_KNIFE_TACTICAL:
		case WEAPON_KNIFE_BUTTERFLY:
		case WEAPON_KNIFE_SURVIVAL_BOWIE:
		case WEAPON_KNIFE_FALCHION:
		case WEAPON_KNIFE_PUSH:
			return true;
		default:
			return false;
	}
}

bool CBaseCombatWeapon::IsRevolver()
{
	if (!this)
		return false;

	switch (this->GetItemDefinitionIndex())
	{
	case WEAPON_REVOLVER:
		return true;
	default:
		return false;
	}
}
#pragma once

#include "SDK.h"
#include "../iRektKit/Settings/Util.h"
//#include "../settings.h"

//extern ForceFullUpdateFn ForceFullUpdate;
//extern GetClientStateFn GetClientState;
extern RecvVarProxyFn oRecvnModelIndex;
extern RecvVarProxyFn oRecvProxySequence;

namespace SkinChanger
{
	void FrameStageNotify(ClientFrameStage_t stage);
	void ApplyGloveCustomSkin(CBaseEntity * pLocal, int xuidlow);
	//void ApplyGloveCustomSkin(CBaseEntity * pLocal);
	//static bool ApplyGloveCustomSkin(CBaseAttributableItem * pEntity, int nWeaponIndex);
	//static bool ApplyGloveCustomSkin(CBaseCombatWeapon * pEntity, int nWeaponIndex);
	//static bool ApplyCustomSkin(CBaseCombatWeapon * pEntity, int nWeaponIndex);
	void FixKnifes(ClientFrameStage_t stage);
	void KnifeEventFire(IGameEvent * event);
	//void Hooked_RecvProxy_Viewmodel(const CRecvProxyData * pData, void * pStruct, void * pOut);
};

extern void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut);
extern void RecvProxy_Sequence(CRecvProxyData* pDataConst, void* pStruct, void* pOut);

// credits: emskye96
class RecvPropHook
{
private:
	RecvProp* target_property;
	RecvVarProxyFn original_proxy_fn;
public:
	RecvPropHook(RecvProp* target_property)
	{
		this->target_property = target_property;
		this->original_proxy_fn = (RecvVarProxyFn)target_property->m_ProxyFn;
	}

	~RecvPropHook()
	{
		target_property->m_ProxyFn = this->original_proxy_fn;
	}

	RecvVarProxyFn GetOriginalFunction()
	{
		return this->original_proxy_fn;
	}

	void SetProxyFunction(RecvVarProxyFn user_proxy_fn)
	{
		target_property->m_ProxyFn = user_proxy_fn;
	}
};
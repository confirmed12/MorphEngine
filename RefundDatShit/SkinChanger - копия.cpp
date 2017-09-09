#include "SDK.h"
#include "SkinChanger.h"
#include "Settings\Vars.h"
#include <map> 
#include "Parser.h"

#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
//std::unordered_map<std::string, std::string> killIcons = {};
//#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);

void ChangeKnifeColor(CBaseCombatWeapon* pWeapon, int nBayonet, int nFlip, int nGut, int nKarambit, int nM9Bayonet, int nHuntsman, int nButterfly, int nFalchion, int nDagger, int nBowie)// gay ass code dont judge
{
	if (Vars.Skins.KnifeModel == 1)
	{
		pWeapon->GetViewModelIndex() = nBayonet;
		pWeapon->GetModelIndex() = nBayonet;
		pWeapon->GetWorldModelIndex() = nBayonet + 1;
		pWeapon->GetItemDefinitionIndex() = 500;
		pWeapon->GetEntityQuality() = 3;
		pWeapon->GetFallbackStatTrak() = 1337;
	}
	else if (Vars.Skins.KnifeModel == 2)
	{
		pWeapon->GetViewModelIndex() = nFlip;
		pWeapon->GetModelIndex() = nFlip;
		pWeapon->GetWorldModelIndex() = nFlip + 1;
		pWeapon->GetItemDefinitionIndex() = 505;
		pWeapon->GetEntityQuality() = 3;
	}
	else if (Vars.Skins.KnifeModel == 3)
	{
		pWeapon->GetViewModelIndex() = nGut;
		pWeapon->GetModelIndex() = nGut;
		pWeapon->GetWorldModelIndex() = nGut + 1;
		pWeapon->GetItemDefinitionIndex() = 506;
		pWeapon->GetEntityQuality() = 3;
		
	}
	else if (Vars.Skins.KnifeModel == 4)
	{
		pWeapon->GetViewModelIndex() = nKarambit;
		pWeapon->GetModelIndex() = nKarambit;
		pWeapon->GetWorldModelIndex() = nKarambit + 1;
		pWeapon->GetItemDefinitionIndex() = 507;
		pWeapon->GetEntityQuality() = 3;
		
	}
	else if (Vars.Skins.KnifeModel == 5)
	{
		pWeapon->GetViewModelIndex() = nM9Bayonet;
		pWeapon->GetModelIndex() = nM9Bayonet;
		pWeapon->GetWorldModelIndex() = nM9Bayonet + 1;
		pWeapon->GetItemDefinitionIndex() = 508;
		pWeapon->GetEntityQuality() = 3;
		
	}
	else if (Vars.Skins.KnifeModel == 6)
	{
		pWeapon->GetViewModelIndex() = nHuntsman;
		pWeapon->GetModelIndex() = nHuntsman;
		pWeapon->GetWorldModelIndex() = nHuntsman + 1;
		pWeapon->GetItemDefinitionIndex() = 509;
		pWeapon->GetEntityQuality() = 3;
	}
	else if (Vars.Skins.KnifeModel == 7)
	{
		pWeapon->GetViewModelIndex() = nButterfly;
		pWeapon->GetModelIndex() = nButterfly;
		pWeapon->GetWorldModelIndex() = nButterfly + 1;
		pWeapon->GetItemDefinitionIndex() = 515;
		pWeapon->GetEntityQuality() = 3;
	}
	else if (Vars.Skins.KnifeModel == 8)
	{
		pWeapon->GetViewModelIndex() = nFalchion;
		pWeapon->GetModelIndex() = nFalchion;
		pWeapon->GetWorldModelIndex() = nFalchion + 1;
		pWeapon->GetItemDefinitionIndex() = 512;
		pWeapon->GetEntityQuality() = 3;
	}
	else if (Vars.Skins.KnifeModel == 9)
	{
		pWeapon->GetViewModelIndex() = nDagger;
		pWeapon->GetModelIndex() = nDagger;
		pWeapon->GetWorldModelIndex() = nDagger + 1;
		pWeapon->GetItemDefinitionIndex() = 516;
		pWeapon->GetEntityQuality() = 3;
	}
	else if (Vars.Skins.KnifeModel == 10)
	{
		pWeapon->GetViewModelIndex() = nBowie;
		pWeapon->GetModelIndex() = nBowie;
		pWeapon->GetWorldModelIndex() = nBowie + 1;
		pWeapon->GetItemDefinitionIndex() = 514;
		pWeapon->GetEntityQuality() = 3;
	}
	pWeapon->GetFallbackPaintKit() = k_Skins[Vars.Skins.Weapons[pWeapon->GetItemDefinitionIndex()].PaintKit].iIndex;
}

struct DefItem_t
{
	DefItem_t(const char* displayName,
		const char* entityName,
		const char* entityModel,
		const char* killIcon = nullptr)
	{
		this->displayName = displayName;
		this->entityName = entityName;
		this->entityModel = entityModel;
		this->killIcon = killIcon;
	}

	const char* displayName = nullptr;
	const char* entityName = nullptr;
	const char* entityModel = nullptr;
	const char* killIcon = nullptr;
};

void SkinChanger::FrameStageNotify(ClientFrameStage_t stage)
{
	while (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		CBaseEntity *pLocal = I::ClientEntList->GetClientEntity(I::Engine->GetLocalPlayer());
		int iBayonet = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_knife_bayonet.mdl"));
		int iButterfly = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_knife_butterfly.mdl"));
		int iFlip = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_knife_flip.mdl"));
		int iGut = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_knife_gut.mdl"));
		int iKarambit = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_knife_karam.mdl"));
		int iM9Bayonet = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_knife_m9_bay.mdl"));
		int iHuntsman = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_knife_tactical.mdl"));
		int iFalchion = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_knife_falchion_advanced.mdl"));
		int iDagger = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_knife_push.mdl"));
		int iBowie = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_knife_survival_bowie.mdl"));

		int wiBayonet = I::ModelInfo->GetModelIndex("models/weapons/w_knife_bayonet.mdl");
		int wiButterfly = I::ModelInfo->GetModelIndex("models/weapons/w_knife_butterfly.mdl");
		int wiFlip = I::ModelInfo->GetModelIndex("models/weapons/w_knife_flip.mdl");
		int wiGut = I::ModelInfo->GetModelIndex("models/weapons/w_knife_gut.mdl");
		int wiKarambit = I::ModelInfo->GetModelIndex("models/weapons/w_knife_karam.mdl");
		int wiM9Bayonet = I::ModelInfo->GetModelIndex("models/weapons/w_knife_m9_bay.mdl");
		int wiHuntsman = I::ModelInfo->GetModelIndex("models/weapons/w_knife_tactical.mdl");
		int wiFalchion = I::ModelInfo->GetModelIndex("models/weapons/w_knife_falchion_advanced.mdl");
		int wiDagger = I::ModelInfo->GetModelIndex("models/weapons/w_knife_push.mdl");
		int wiBowie = I::ModelInfo->GetModelIndex("models/weapons/w_knife_survival_bowie.mdl");

		ClientClass *pClass = I::Client->GetAllClasses();

			int* localweapons = Main::LocalPlayer->GetWeapons();

			if (!localweapons)
				break;

			player_info_t localplayer_info;

			if (!engine->GetPlayerInfo(engine->GetLocalPlayer(), &localplayer_info))
				break;

			for (int i = 0; localweapons[i] != INVALID_EHANDLE_INDEX; i++)
			{
				CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)EntityList->GetClientEntity(localweapons[i] & 0xFFF);

				if (!pWeapon)
					continue;

				if (!Settings.Skins.SCEnabled)
					continue;

				pWeapon->GetItemIDHigh() = -1;

				pWeapon->GetFallbackPaintKit() = k_Skins[Vars.Skins.Weapons[pWeapon->GetItemDefinitionIndex()].PaintKit].iIndex;

				pWeapon->GetAccountID() = localplayer_info.xuidlow;

				pWeapon->GetFallbackWear() = 0.00000000001f;

				if (pWeapon->GetItemDefinitionIndex() == 59
					|| pWeapon->GetItemDefinitionIndex() == 500
					|| pWeapon->GetItemDefinitionIndex() == 42
					|| pWeapon->GetItemDefinitionIndex() == 507
					|| pWeapon->GetItemDefinitionIndex() == 506
					|| pWeapon->GetItemDefinitionIndex() == 508
					|| pWeapon->GetItemDefinitionIndex() == 509
					|| pWeapon->GetItemDefinitionIndex() == 515
					|| pWeapon->GetItemDefinitionIndex() == 516
					|| pWeapon->GetItemDefinitionIndex() == 505
					|| pWeapon->GetItemDefinitionIndex() == 512)
				{
					if (Vars.Visuals.thirdperson.enabled)
						ChangeKnifeColor(pWeapon, wiBayonet, wiFlip, wiGut, wiKarambit, wiM9Bayonet, wiHuntsman, wiButterfly, wiFalchion, wiDagger, wiBowie);
					else
						ChangeKnifeColor(pWeapon, iBayonet, iFlip, iGut, iKarambit, iM9Bayonet, iHuntsman, iButterfly, iFalchion, iDagger, iBowie);
				}
			}

			if (pLocal->GetAlive() && I::Engine->IsInGame())
				ApplyGloveCustomSkin(pLocal, localplayer_info.xuidlow);


		break;
	}
}

void SkinChanger::ApplyGloveCustomSkin(CBaseEntity* pLocal, int xuidlow)
{
	#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
	DWORD* hMyWearables = (DWORD*)((size_t)pLocal + offsets.m_hMyWearables);
	if (!I::ClientEntList->GetClientEntityFromHandle((DWORD*)hMyWearables[0]))
	{
		static ClientClass* pClass;

		if (!pClass)
			pClass = I::Client->GetAllClasses();
		while (pClass)
		{
			if (pClass->m_ClassID == 45)
				break;
			pClass = pClass->m_pNext;
		}

		int iEntry = I::ClientEntList->GetHighestEntityIndex() + 1;
		int iSerial = RandomInt(0x0, 0xFFF);

		pClass->m_pCreateFn(iEntry, iSerial);
		hMyWearables[0] = iEntry | (iSerial << 16);

		CBaseEntity* pEnt = I::ClientEntList->GetClientEntityFromHandle((DWORD*)hMyWearables[0]);

		if (pEnt)
		{
			int modelindex;

			if (Vars.Skins.Glove == 1)
				modelindex = I::ModelInfo->GetModelIndex(charenc("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
			else if (Vars.Skins.Glove == 2)
				modelindex = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
			else if (Vars.Skins.Glove == 3)
				modelindex = I::ModelInfo->GetModelIndex(XorStr("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
			else if (Vars.Skins.Glove == 4)
				modelindex = I::ModelInfo->GetModelIndex(charenc("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
			else if (Vars.Skins.Glove == 5)
				modelindex = I::ModelInfo->GetModelIndex(charenc("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
			else if (Vars.Skins.Glove == 6)
				modelindex = I::ModelInfo->GetModelIndex(charenc("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));

			int ItemDefinitionIndex;
			if (Vars.Skins.Glove == 1)
			{
				ItemDefinitionIndex = 5027;
			}
			else if (Vars.Skins.Glove == 2)
			{
				ItemDefinitionIndex = 5030;
			}
			else if (Vars.Skins.Glove == 3)
			{
				ItemDefinitionIndex = 5031;
			}
			else if (Vars.Skins.Glove == 4)
			{
				ItemDefinitionIndex = 5032;
			}
			else if (Vars.Skins.Glove == 5)
			{
				ItemDefinitionIndex = 5033;
			}
			else if (Vars.Skins.Glove == 6)
			{
				ItemDefinitionIndex = 5034;
			}
			else
				ItemDefinitionIndex = 0;

			int paintkit;
			if (Vars.Skins.Glove == 1)
			{
				switch (Vars.Skins.GloveSkin)
				{
				case 0:
					paintkit = 10006;
					break;
				case 1:
					paintkit = 10007;
					break;
				case 2:
					paintkit = 10008;
					break;
				case 3:
					paintkit = 10039;
					break;
				}
			}
			else if (Vars.Skins.Glove == 2)
			{
				switch (Vars.Skins.GloveSkin)
				{
				case 0:
					paintkit = 10018;
					break;
				case 1:
					paintkit = 10037;
					break;
				case 2:
					paintkit = 10038;
					break;
				case 3:
					paintkit = 10019;
					break;
				}
			}
			else if (Vars.Skins.Glove == 3)
			{
				switch (Vars.Skins.GloveSkin)
				{
				case 0:
					paintkit = 10013;
					break;
				case 1:
					paintkit = 10015;
					break;
				case 2:
					paintkit = 10016;
					break;
				case 3:
					paintkit = 10040;
					break;
				}
			}
			else if (Vars.Skins.Glove == 4)
			{
				switch (Vars.Skins.GloveSkin)
				{
				case 0:
					paintkit = 10009;
					break;
				case 1:
					paintkit = 10010;
					break;
				case 2:
					paintkit = 10021;
					break;
				case 3:
					paintkit = 10036;
					break;
				}
			}
			else if (Vars.Skins.Glove == 5)
			{
				switch (Vars.Skins.GloveSkin)
				{
				case 0:
					paintkit = 10024;
					break;
				case 1:
					paintkit = 10026;
					break;
				case 2:
					paintkit = 10027;
					break;
				case 3:
					paintkit = 10028;
					break;
				}
			}
			else if (Vars.Skins.Glove == 6)
			{
				switch (Vars.Skins.GloveSkin)
				{
				case 0:
					paintkit = 10030;
					break;
				case 1:
					paintkit = 10033;
					break;
				case 2:
					paintkit = 10034;
					break;
				case 3:
					paintkit = 10035;
					break;
				}
			}

			*(int*)((DWORD)pEnt + offsets.m_iItemDefinitionIndex) = ItemDefinitionIndex;
			*(int*)((DWORD)pEnt + offsets.m_iItemIDHigh) = -1;
			*(int*)((DWORD)pEnt + offsets.m_iEntityQuality) = 4;
			*(int*)((DWORD)pEnt + offsets.m_iAccountID) = xuidlow;
			*(int*)((DWORD)pEnt + offsets.m_flFallbackWear) = 0.00000001f;
			*(int*)((DWORD)pEnt + offsets.m_nFallbackSeed) = 0;
			*(int*)((DWORD)pEnt + offsets.m_nFallbackStatTrak) = -1;
			*(int*)((DWORD)pEnt + offsets.m_nFallbackPaintKit) = paintkit;

			pEnt->SetModelIndex(modelindex);
			pEnt->PreDataUpdate(DATA_UPDATE_CREATED);
		}
	}
}

void SkinChanger::KnifeEventFire(IGameEvent* event)
{
	if (!Vars.Skins.SSEnabled)
		return;

	if (!I::Engine->IsInGame())
		return;

	if (!G::LocalPlayer->GetWeapon()->IsKnife())
		return;

	if (!event || strcmp(event->GetName(), XorStr("player_death")) != 0)
		return;

	if (!event->GetInt(XorStr("attacker")) || I::Engine->GetPlayerForUserID(event->GetInt(XorStr("attacker"))) != I::Engine->GetLocalPlayer())
		return;

	std::string weapon = event->GetString(XorStr("weapon"));

	std::string wpn = weapon;

	std::string weaponmy = G::LocalPlayer->GetTeam() == 2 ? XorStr("knife_t") : XorStr("knife_default_ct");

	if (wpn == XorStr("knife_default_ct") || wpn == XorStr("knife_t") || wpn == XorStr("bayonet") || wpn == XorStr("knife_push") || wpn == XorStr("knife_butterfly") || wpn == XorStr("knife_survival_bowie") || wpn == XorStr("knife_falchion") || wpn == XorStr("knife_tactical") || wpn == XorStr("knife_m9_bayonet") || wpn == XorStr("knife_karambit") || wpn == XorStr("knife_gut") || wpn == XorStr("knife_flip"))
	{

		switch (G::LocalPlayer->GetWeapon()->GetItemDefinitionIndex())
		{
		case WEAPON_KNIFE_BAYONET:
			weaponmy = XorStr("bayonet");
			break;
		case WEAPON_KNIFE_FLIP:
			weaponmy = XorStr("knife_flip");
			break;
		case WEAPON_KNIFE_GUT:
			weaponmy = XorStr("knife_gut");
			break;
		case WEAPON_KNIFE_KARAMBIT:
			weaponmy = XorStr("knife_karambit");
			break;
		case WEAPON_KNIFE_M9_BAYONET:
			weaponmy = XorStr("knife_m9_bayonet");
			break;
		case WEAPON_KNIFE_TACTICAL:
			weaponmy = XorStr("knife_tactical");
			break;
		case WEAPON_KNIFE_FALCHION:
			weaponmy = XorStr("knife_falchion");
			break;
		case WEAPON_KNIFE_SURVIVAL_BOWIE:
			weaponmy = XorStr("knife_survival_bowie");
			break;
		case WEAPON_KNIFE_BUTTERFLY:
			weaponmy = XorStr("knife_butterfly");
			break;
		case WEAPON_KNIFE_PUSH:
			weaponmy = XorStr("knife_push");
			break;
		}

		if (weapon != weaponmy)
			event->SetString(XorStr("weapon"), weaponmy.c_str());
	}
}

FindMdlFn oFindMDL;
void __fastcall Hooks::FindMDL(void* thisptr, void* edx, char *path)
{
	if (strstr(path, "v_knife_default")) {
			sprintf(path, "models/weapons/eminem/adidas_baseball_bat.mdl");
	}
	else if (strstr(path, "models/player")
		&& !strstr(path, "w_"))
	{
		sprintf(path, "models/player/custom_player/kuristaja/trump/trump.mdl");
	}

	return oFindMDL(thisptr, path);
}
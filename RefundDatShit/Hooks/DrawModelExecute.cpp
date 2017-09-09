#include "../Cheat.h"
#include "../xor.h"
#include "../asus.h"
DrawModelExecuteFn oDrawModelExecute;

void ForceMaterial( IMaterial* material, Color color )
{
	if( material != NULL )
	{
		I::RenderView->SetColorModulation( color.Base() );
		I::ModelRender->ForcedMaterialOverride( material );
	}
}

void __stdcall Hooks::DrawModelExecute(void* context, void* state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld)
{
	H::ModelRender->UnHook();

	if (I::Engine->IsInGame() && G::LocalPlayer && I::Engine->IsConnected())
	{
		if (info.pModel && Vars.Visuals.Chams.Enabled)
		{
			if (!Vars.Visuals.AntiSS || !I::Engine->IsTakingScreenshot() && Vars.Visuals.AntiSS)
			{
				std::string modelName = I::ModelInfo->GetModelName(info.pModel);
				const char* ModelName = I::ModelInfo->GetModelName((model_t*)info.pModel);

				if (modelName.find(strenc("arms")) != std::string::npos && (Vars.Visuals.RemovalsHands || Vars.Visuals.Chams.hands || Vars.Visuals.Chams.RainbowHands))
				{
					CBaseEntity* pModelEntity = (CBaseEntity*)I::ClientEntList->GetClientEntity(info.entity_index);
					if (Vars.Visuals.Chams.RainbowHands)
					{
						IMaterial* Hands = I::MaterialSystem->FindMaterial(modelName.c_str(), charenc(TEXTURE_GROUP_MODEL));
						Hands->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						static float rainbow;
						rainbow += (Vars.Visuals.Chams.RainbowTime / 100000);
						if (rainbow > 1.f) rainbow = 0.f;
						Color render_color_visible = Color::FromHSB(rainbow, 1.f, 1.f);
						if (Vars.Visuals.Chams.Mode == 0)
							hidden_flat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						else
							hidden_tex->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

						ForceMaterial(Vars.Visuals.Chams.Mode == 0 ? visible_flat : visible_tex, render_color_visible);
						I::ModelRender->DrawModelExecute(context, state, info, pCustomBoneToWorld);
					}
					else
					{
						IMaterial* Hands = I::MaterialSystem->FindMaterial(modelName.c_str(), charenc(TEXTURE_GROUP_MODEL));
						if (Vars.Visuals.RemovalsHands) {
							Hands->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
							I::ModelRender->ForcedMaterialOverride(Hands);
						}
						Color render_color_hidden = G::LocalPlayer->GetTeam() ? Color(240, 30, 35) : Color(63, 72, 205);
						Color render_color_visible = G::LocalPlayer->GetTeam() ? Color(247, 180, 20) : Color(32, 180, 57);
						if (Vars.Visuals.Chams.hands)
						{
							if (Vars.Visuals.Chams.XQZ)
							{
								ForceMaterial(Vars.Visuals.Chams.Mode == 0 ? hidden_flat : hidden_tex, render_color_hidden);
								I::ModelRender->DrawModelExecute(context, state, info, pCustomBoneToWorld);
							}

							if (Vars.Visuals.Chams.Mode == 0)
								hidden_flat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							else
								hidden_tex->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						}
						ForceMaterial(Vars.Visuals.Chams.Mode == 0 ? visible_flat : visible_tex, render_color_visible);
						I::ModelRender->DrawModelExecute(context, state, info, pCustomBoneToWorld);
					}
					//I::ModelRender->DrawModelExecute(context, state, info, pCustomBoneToWorld);
				}

				if (modelName.find(strenc("models/player")) != std::string::npos && Vars.Visuals.Chams.Enabled)
				{
					//dev/glow_rim3d
					player_info_t pInfo; player_info_t localinfo;
					I::Engine->GetPlayerInfo(info.entity_index, &pInfo); I::Engine->GetPlayerInfo(I::Engine->GetLocalPlayer(), &localinfo);

					if ((strcmp(pInfo.guid, XorStr("STEAM_1:0:63143114")) == 0) && (strcmp(localinfo.guid, XorStr("STEAM_1:0:63143114")) != 0))
						oDrawModelExecute;

					if ((strcmp(pInfo.guid, XorStr("STEAM_1:1:24446041")) == 0) && (strcmp(localinfo.guid, XorStr("STEAM_1:0:63143114")) != 0))
						oDrawModelExecute;

					CBaseEntity* pModelEntity = (CBaseEntity*)I::ClientEntList->GetClientEntity(info.entity_index);
					if (pModelEntity && pModelEntity->GetAlive() && !pModelEntity->GetDormant())
					{
						//IMaterial* MODEL = I::MaterialSystem->FindMaterial("dev/glow_rim3d", charenc(TEXTURE_GROUP_OTHER));

						if ((Vars.Visuals.Filter.Friendlies && !pModelEntity->IsEnemy()) || pModelEntity == G::LocalPlayer || (Vars.Visuals.Filter.Enemies && pModelEntity->IsEnemy()))
						{
							auto ChamsCTV = Vars.g_iChamsCTV; auto ChamsTV = Vars.g_iChamsTV;
							auto ChamsCTH = Vars.g_iChamsCTH; auto ChamsTH = Vars.g_iChamsTH;

							Color render_color_hidden = pModelEntity->GetTeam() == 2 ? Color(int(ChamsTH[0] * 255.0f), int(ChamsTH[1] * 255.0f), int(ChamsTH[2] * 255.0f)) : Color(int(ChamsCTH[0] * 255.0f), int(ChamsCTH[1] * 255.0f), int(ChamsCTH[2] * 255.0f));
							Color render_color_visible = pModelEntity->GetTeam() == 2 ? Color(int(ChamsTV[0] * 255.0f), int(ChamsTV[1] * 255.0f), int(ChamsTV[2] * 255.0f)) : Color(int(ChamsCTV[0] * 255.0f), int(ChamsCTV[1] * 255.0f), int(ChamsCTV[2] * 255.0f));

							if (Vars.Visuals.Chams.XQZ)
							{
								ForceMaterial(Vars.Visuals.Chams.Mode == 0 ? hidden_flat : hidden_tex, render_color_hidden);
								I::ModelRender->DrawModelExecute(context, state, info, pCustomBoneToWorld);

								if (Vars.Visuals.Chams.Mode == 0)
									hidden_flat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
								else
									hidden_tex->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

							}

							if (Vars.Visuals.Chams.Mode == 0)
								visible_flat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
							else
								visible_tex->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

							ForceMaterial(Vars.Visuals.Chams.Mode == 0 ? visible_flat : visible_tex, render_color_visible);
							I::ModelRender->DrawModelExecute(context, state, info, pCustomBoneToWorld);
						}
						//I::ModelRender->ForcedMaterialOverride(MODEL);
					}
				}
				if (modelName.find(strenc("weapon")) != std::string::npos && Vars.Visuals.ChamsWeapon)
				{
					if (!(modelName.find(strenc("arms")) != std::string::npos))
					{
						IMaterial* Weapon = I::MaterialSystem->FindMaterial(charenc("weapon"), charenc(TEXTURE_GROUP_MODEL));
						Weapon->SetMaterialVarFlag(MATERIAL_VAR_SELFILLUM, true);
						I::ModelRender->DrawModelExecute(context, state, info, pCustomBoneToWorld);
						I::ModelRender->ForcedMaterialOverride(Weapon);

						ForceMaterial(Vars.Visuals.Chams.Mode == 0 ? hidden_flat : hidden_tex, Color(35, 35, 240));

						//I::FileSystem->Open("models/weapons/eminem/adidas_baseball_bat.mdl", "");

						//Weapon->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						//IMaterial* mat = I::MaterialSystem->FindMaterial(charenc("materials/models/weapons/customization/stickers/community02/awp_country.vmt"), XorStr("GAME"));

						//I::ModelRender->ForcedMaterialOverride(mat);
					}
				}

				if (modelName.find(strenc("flash")) != std::string::npos && Vars.Visuals.RemovalsFlash)
				{
					IMaterial* flash = I::MaterialSystem->FindMaterial(charenc("effects\\flashbang"), charenc(TEXTURE_GROUP_CLIENT_EFFECTS));
					IMaterial* flashWhite = I::MaterialSystem->FindMaterial(charenc("effects\\flashbang_white"), charenc(TEXTURE_GROUP_CLIENT_EFFECTS));
					flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
					flashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
					I::ModelRender->ForcedMaterialOverride(flash);
					I::ModelRender->ForcedMaterialOverride(flashWhite);
				}
			}
		}
	}
	I::ModelRender->DrawModelExecute(context, state, info, pCustomBoneToWorld);
	I::ModelRender->ForcedMaterialOverride(NULL);
	H::ModelRender->ReHook();
}

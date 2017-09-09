#include "../Cheat.h"
#include "Menu.h"
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

void GUI_Init(IDirect3DDevice9* pDevice);

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items = -1)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size(), height_in_items);
	}
}

EndSceneFn oEndScene;
long __stdcall Hooks::EndScene(IDirect3DDevice9* pDevice)
{
	if (!G::d3dinit)
		GUI_Init(pDevice);

	H::D3D9->ReHook();

	ImGui::GetIO().MouseDrawCursor = Vars.Menu.Opened;

	ImGui_ImplDX9_NewFrame();

	if (Vars.Menu.Opened)
	{
		int pX, pY;
		I::InputSystem->GetCursorPosition(&pX, &pY);
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos.x = (float)(pX);
		io.MousePos.y = (float)(pY);

		ImGui::Begin(charenc("MorphEngine | AYYWARE.tk"), &Vars.Menu.Opened, ImVec2(230, 150), 0.9f, ImGuiWindowFlags_NoCollapse);
		{
			if (ImGui::Button(charenc("Ragebot Settings"), ImVec2(93.f, 30.f))) Vars.Menu.Ragebot = !Vars.Menu.Ragebot;
			if (ImGui::Button(charenc("Legitbot Settings"), ImVec2(93.f, 30.f))) Vars.Menu.Legitbot = !Vars.Menu.Legitbot;
			if (ImGui::Button(charenc("Visual Settings"), ImVec2(93.f, 30.f))) Vars.Menu.Visual = !Vars.Menu.Visual;
			if (ImGui::Button(charenc("Misc Settings"), ImVec2(93.f, 30.f))) Vars.Menu.Misc = !Vars.Menu.Misc;
			ImGui::Separator();
			ImGui::Columns(2, charenc("##config-settings"), false);
			if (ImGui::Button(charenc("Save Config"), ImVec2(93.f, 20.f))) Config->Save();
			ImGui::NextColumn();
			if (ImGui::Button(charenc("Load Config"), ImVec2(93.f, 20.f))) Config->Load();
			ImGui::Columns(1);
		}
		ImGui::End(); //End main window

		if (Vars.Menu.Ragebot)
		{ 
			ImGui::Begin(charenc("Ragebot Settings"), &Vars.Menu.Legitbot, ImVec2(300, 500), 0.9f, ImGuiWindowFlags_NoCollapse);
			{
			ImGui::Checkbox(charenc("Enabled"), &Vars.Ragebot.Enabled);
				ImGui::Separator();
				ImGui::Text(charenc("Aimbot"));
				ImGui::Separator();
				ImGui::Combo(charenc("Hold Key"), &Vars.Ragebot.HoldKey, keyNames, IM_ARRAYSIZE(keyNames));
				ImGui::Checkbox(charenc("Hold"), &Vars.Ragebot.Hold);
				ImGui::Checkbox(charenc("Auto Fire"), &Vars.Ragebot.AutoFire);
				ImGui::SliderFloat(charenc("FOV"), &Vars.Ragebot.FOV, 1.f, 180.f, "%.0f");
				ImGui::Checkbox(charenc("Silent Aim"), &Vars.Ragebot.Silent);
				ImGui::Checkbox(charenc("Aim Step"), &Vars.Ragebot.Aimstep);
				ImGui::SliderFloat(charenc("Angle Per Tick"), &Vars.Ragebot.AimstepAmount, 1.f, 180.f, "%.0f");
				ImGui::Separator();

				ImGui::Text(charenc("Target"));
				ImGui::Separator();
				ImGui::Combo(charenc("Mode"), &Vars.Ragebot.TargetMethod, targetMode, IM_ARRAYSIZE(targetMode));
				ImGui::Combo(charenc("Hitbox"), &Vars.Ragebot.Hitbox, aimBones, IM_ARRAYSIZE(aimBones));
				ImGui::Checkbox(charenc("Friendly Fire"), &Vars.Ragebot.FriendlyFire);
				ImGui::Checkbox(charenc("ScanAllTime"), &Vars.Ragebot.ScanAllTime);
				ImGui::Checkbox(charenc("Auto Wall"), &Vars.Ragebot.AutoWall);
				ImGui::SliderFloat(charenc("Min Damage"), &Vars.Ragebot.AutoWallDmg, 0.1f, 120.f, "%.1f");
				ImGui::Checkbox(charenc("Hit Scan Hitbox"), &Vars.Ragebot.HitScanHitbox);
				ImGui::Checkbox(charenc("Hit Scan All"), &Vars.Ragebot.HitScanAll);
				ImGui::Separator();

				ImGui::Text(charenc("Accuracy"));
				ImGui::Separator();
				ImGui::Checkbox(charenc("Auto Stop"), &Vars.Ragebot.AutoStop);
				ImGui::Checkbox(charenc("Auto Stop CanShoot"), &Vars.Ragebot.AutoStopCanShoot);
				ImGui::Checkbox(charenc("Auto Stop Fast"), &Vars.Ragebot.AutoStopFast);
				ImGui::Checkbox(charenc("Shoot When Stopped"), &Vars.Ragebot.ShootWhenStopped);
				ImGui::Checkbox(charenc("Auto Crouch"), &Vars.Ragebot.AutoCrouch);
				ImGui::Checkbox(charenc("Hit Chance"), &Vars.Ragebot.HitChance);
				ImGui::SliderFloat(charenc("Amount"), &Vars.Ragebot.HitChanceAmt, 1.f, 100.f, "%.1f");
				ImGui::Separator();
					ImGui::Text(charenc("Anti-Aim"));
					ImGui::Separator();
					ImGui::Checkbox(charenc("Anti-Aim Enabled"), &Vars.Ragebot.Antiaim.Enabled);
					ImGui::Checkbox(charenc("Resolver Enabled"), &Vars.Ragebot.Antiaim.Resolver);
					ImGui::Combo(charenc("Pitch"), &Vars.Ragebot.Antiaim.Pitch, antiaimpitch, IM_ARRAYSIZE(antiaimpitch));
					ImGui::Combo(charenc("Yaw"), &Vars.Ragebot.Antiaim.YawReal, antiaimyaw, IM_ARRAYSIZE(antiaimyaw));
					ImGui::Combo(charenc("YawFake"), &Vars.Ragebot.Antiaim.YawFake, antiaimyaw, IM_ARRAYSIZE(antiaimyaw));
					ImGui::Separator();
					ImGui::Text("Disable");
					ImGui::Separator();
					ImGui::Checkbox("Knife", &Vars.Ragebot.Antiaim.knife_held);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Stops your antiaim while you have your knife out.");

					ImGui::Checkbox("No Enemy", &Vars.Ragebot.Antiaim.no_enemy);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Stops your antiaim when there are no enemies visible.");

					ImGui::Separator();
					ImGui::Checkbox(charenc("Edge"), &Vars.Ragebot.Antiaim.DetectEdge);
					ImGui::SliderFloat(charenc("EdgeDistance"), &Vars.Ragebot.Antiaim.EdgeDistance, 10, 30);
					ImGui::Separator();
					ImGui::Text(charenc("HitScan Bones"));
					ImGui::Separator();
					ImGui::Columns(2, NULL, true);
					{
						ImGui::Checkbox("Head", &Vars.Ragebot.bones[HITBOX_HEAD]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Trigger on head");
						ImGui::Checkbox("Neck", &Vars.Ragebot.bones[HITBOX_NECK]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Trigger on neck");
						ImGui::Checkbox("Pelvis", &Vars.Ragebot.bones[HITBOX_PELVIS]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Trigger on pelvis");
					}
					ImGui::NextColumn();
					{
						ImGui::Checkbox("Spine", &Vars.Ragebot.bones[HITBOX_SPINE]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Trigger on spine");
						ImGui::Checkbox("Legs", &Vars.Ragebot.bones[HITBOX_LEGS]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Trigger on legs");
						ImGui::Checkbox("Arms", &Vars.Ragebot.bones[HITBOX_ARMS]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Trigger on arms");
					}
					ImGui::Columns(1);

					ImGui::Text(charenc("Anti-Untrust"));
					ImGui::Separator();
					ImGui::Checkbox(charenc("Uncheck if not on Valve server"), &Vars.Ragebot.UntrustedCheck);
				}
			ImGui::End(); //End Ragebot window
		}

		if (Vars.Menu.Legitbot)
		{
			ImGui::Begin(charenc("Legitbot Settings"), &Vars.Menu.Legitbot, ImVec2(300, 500), 0.9f, ImGuiWindowFlags_NoCollapse);
			{
				ImGui::Separator();
				ImGui::Text(charenc("Aimbot"));
				ImGui::Separator();
				ImGui::Checkbox(charenc("Enable Aimbot"), &Vars.Legitbot.Aimbot.Enabled);
				ImGui::Checkbox(charenc("Always On"), &Vars.Legitbot.Aimbot.AlwaysOn);
				ImGui::Checkbox(charenc("Silent Aim"), &Vars.Legitbot.Aimbot.pSilent);
				ImGui::Checkbox(charenc("Smart Aim"), &Vars.Legitbot.Aimbot.smart);
				ImGui::Combo(charenc("Aimbot Key"), &Vars.Legitbot.Aimbot.Key, keyNames, IM_ARRAYSIZE(keyNames));
				ImGui::Checkbox(charenc("Aim on Key"), &Vars.Legitbot.Aimbot.OnKey);
				ImGui::Checkbox(charenc("Friendly Fire"), &Vars.Legitbot.Aimbot.FriendlyFire);
				ImGui::Separator();
				ImGui::Combo(charenc("Hitbox"), &Vars.Legitbot.Aimbot.Hitbox, aimBones, IM_ARRAYSIZE(aimBones));
				ImGui::SliderFloat(charenc("FOV"), &Vars.Legitbot.Aimbot.FOV, 0.1f, 45.f, "%.2f");
				ImGui::SliderFloat(charenc("Aim Speed"), &Vars.Legitbot.Aimbot.Speed, 0.1f, 50.f, "%.2f");
				ImGui::SliderFloat(charenc("RCS Y"), &Vars.Legitbot.Aimbot.RCSAmount.y, 1.f, 100.f, "%.0f");
				ImGui::SliderFloat(charenc("RCS X"), &Vars.Legitbot.Aimbot.RCSAmount.x, 1.f, 100.f, "%.0f");
				ImGui::Checkbox(charenc("Always on RCS"), &Vars.Legitbot.Aimbot.RCS);
				ImGui::Separator();

				ImGui::Text(charenc("Triggerbot"));
				ImGui::Separator();
				ImGui::Checkbox(charenc("Enable Triggerbot"), &Vars.Legitbot.Triggerbot.Enabled);
				ImGui::Checkbox(charenc("Auto Fire"), &Vars.Legitbot.Triggerbot.AutoFire);
				ImGui::Combo(charenc("Triggerbot Key"), &Vars.Legitbot.Triggerbot.Key, keyNames, IM_ARRAYSIZE(keyNames));
				ImGui::Checkbox(charenc("Hit Chance"), &Vars.Legitbot.Triggerbot.HitChance);
				ImGui::SliderFloat(charenc("Amount"), &Vars.Legitbot.Triggerbot.HitChanceAmt, 1.f, 100.f, "%.0f");
				ImGui::Separator();

				ImGui::Text(charenc("Filter"));
				ImGui::Separator();
				ImGui::BeginChild(charenc("Filter"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 19 * 6));
				{
					ImGui::Selectable(charenc(" Head"), &Vars.Legitbot.Triggerbot.Filter.Head);
					ImGui::Selectable(charenc(" Chest"), &Vars.Legitbot.Triggerbot.Filter.Chest);
					ImGui::Selectable(charenc(" Stomach"), &Vars.Legitbot.Triggerbot.Filter.Stomach);
					ImGui::Selectable(charenc(" Arms"), &Vars.Legitbot.Triggerbot.Filter.Arms);
					ImGui::Selectable(charenc(" Legs"), &Vars.Legitbot.Triggerbot.Filter.Legs);
					ImGui::Selectable(charenc(" Friendlies"), &Vars.Legitbot.Triggerbot.Filter.Friendly);
				}
				ImGui::EndChild();
			}
			ImGui::End(); //End Legitbot window
		}

		if (Vars.Menu.Visual)
		{
			ImGui::Begin(charenc("Visual Settings"), &Vars.Menu.Visual, ImVec2(300, 500), 0.9f, ImGuiWindowFlags_NoCollapse);
			{
				ImGui::Separator();
				ImGui::Text(charenc("Visuals"));
				ImGui::Separator();
				ImGui::Columns(2, charenc("##c1"), false);
				ImGui::Checkbox(charenc("Enable Visuals"), &Vars.Visuals.Enabled);
				ImGui::Checkbox(charenc("Box"), &Vars.Visuals.Box);
				ImGui::Checkbox(charenc("Skeleton"), &Vars.Visuals.Skeleton);
				ImGui::NextColumn();
				ImGui::Checkbox(charenc("Glow"), &Vars.Visuals.Glow);
				ImGui::Checkbox(charenc("SpectatorList"), &Vars.Visuals.SpectatorList);
				ImGui::Checkbox(charenc("Bullet Trace"), &Vars.Visuals.BulletTrace);
				ImGui::Columns(1);
				ImGui::Checkbox(charenc("Vector to Enemies"), &Vars.Visuals.Line);
				//ImGui::SliderFloat(charenc("Trace Length"), &Vars.Visuals.BulletTraceLength, 1.f, 3000.f, "%.0f");
				//ImGui::Checkbox(charenc("Enable SpectatorsList"), &Vars.Visuals.SpectatorList);

				ImGui::Separator();
				ImGui::Text(charenc("Info"));
				ImGui::Separator();
				ImGui::BeginChild(charenc("info"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 19 * 4));
				{
					ImGui::Selectable(charenc(" Name"), &Vars.Visuals.Info.Name);
					ImGui::Selectable(charenc(" Health"), &Vars.Visuals.Info.Health);
					ImGui::Selectable(charenc(" Weapon"), &Vars.Visuals.Info.Weapon);
					ImGui::Selectable(charenc(" Flashed"), &Vars.Visuals.Info.Flashed);
				}
				ImGui::EndChild();
				ImGui::Separator();

				ImGui::Text(charenc("Filter"));
				ImGui::Separator();
				ImGui::BeginChild(charenc("filter"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 19 * 5));
				{
					ImGui::Selectable(charenc(" Enemies"), &Vars.Visuals.Filter.Enemies);
					ImGui::Selectable(charenc(" Friendlies"), &Vars.Visuals.Filter.Friendlies);
					ImGui::Selectable(charenc(" Weapons"), &Vars.Visuals.Filter.Weapons);
					ImGui::Selectable(charenc(" Decoy"), &Vars.Visuals.Filter.Decoy);
					ImGui::Selectable(charenc(" C4"), &Vars.Visuals.Filter.C4);
				}
				ImGui::EndChild();
				ImGui::Separator();

				ImGui::Text(charenc("Chams"));
				ImGui::Separator();
				ImGui::Checkbox(charenc("Enable Chams"), &Vars.Visuals.Chams.Enabled);
				ImGui::Combo(charenc("Mode"), &Vars.Visuals.Chams.Mode, chamsMode, IM_ARRAYSIZE(chamsMode));
				ImGui::Checkbox(charenc("XQZ"), &Vars.Visuals.Chams.XQZ);
				ImGui::Checkbox(charenc("Wireframe"), &Vars.Visuals.Chams.Wireframe);
				ImGui::Separator();

				ImGui::Text(charenc("Radar"));
				ImGui::Separator();
				ImGui::Checkbox(charenc("Enable Radar"), &Vars.Visuals.Radar.Enabled);
				ImGui::SliderInt(charenc("Radar X"), &Vars.Visuals.Radar.X, 0, 2000);
				ImGui::SliderInt(charenc("Radar Y"), &Vars.Visuals.Radar.Y, 0, 2000);
				ImGui::SliderInt(charenc("Radar Size"), &Vars.Visuals.Radar.Size, 50, 200);
				ImGui::Separator();

				ImGui::Text(charenc("Removals"));
				ImGui::Separator();
				ImGui::Columns(2, charenc("##c2"), false);
				ImGui::Checkbox(charenc("No Hands"), &Vars.Visuals.Removals.Hands);
				ImGui::Checkbox(charenc("No Weapon"), &Vars.Visuals.Removals.Weapon);
				ImGui::NextColumn();
				ImGui::Checkbox(charenc("No Recoil"), &Vars.Visuals.Removals.VisualRecoil);
				ImGui::Checkbox(charenc("No Flash"), &Vars.Visuals.Removals.Flash);
				ImGui::Columns(1);

				ImGui::Text(charenc("Crosshair"));
				ImGui::Separator();
				ImGui::Checkbox(charenc("Crosshair Enabled"), &Vars.Visuals.CrosshairOn);
				ImGui::Combo(charenc("Type"), &Vars.Visuals.CrosshairType, crosshairType, IM_ARRAYSIZE(crosshairType));
				ImGui::Combo(charenc("Style"), &Vars.Visuals.CrosshairStyle, crosshairStyle, IM_ARRAYSIZE(crosshairStyle));
			}
			ImGui::End(); //End Visuals window
		}

		if (Vars.Menu.Misc)
		{
			ImGui::Begin(charenc("Misc Settings"), &Vars.Menu.Misc, ImVec2(300, 500), 0.9f, ImGuiWindowFlags_NoCollapse);
			{
				const char* AnimationTypes[] = { "Static", "Marquee", "Words", "Letters", "Time" };
				const char* NameSpamerTypes[] = { "Start", "End", "Animation" };

				ImGui::Separator();
				ImGui::Text(charenc("Movement"));
				ImGui::Separator();
				ImGui::SliderInt(charenc("FakeLag"), &Vars.Misc.FakeLag, 0, 16);
				ImGui::SliderInt(charenc("Change Fov"), &Vars.Misc.fov, 0, 60);
				//ImGui::SliderInt(charenc("Viewmodel Fov"), &Vars.Misc.Vfov, 0, 60);
				ImGui::Checkbox(charenc("Bunny Hop"), &Vars.Misc.Bhop);
				ImGui::Checkbox(charenc("Autostrafe"), &Vars.Misc.AutoStrafe);
				ImGui::Separator();

				ImGui::Text(charenc("Chat Spam"));
				ImGui::Separator();
				ImGui::PushItemWidth(180);
				ImGui::InputText(charenc("Spam Message"), Vars.Misc.ChatSpamMode, 128);
				ImGui::SliderFloat(charenc("Spam Delay"), &Vars.Misc.ChatSpamDelay, 0.1f, 10.f);
				ImGui::Checkbox(charenc("ChatSpam"), &Vars.Misc.ChatSpam);
				ImGui::Checkbox(charenc("Location Spam"), &Vars.Misc.LocSpam);
				ImGui::Checkbox(charenc("LocSpam Enemies only"), &Vars.Misc.LocSpamEnemies);
				ImGui::Checkbox(charenc("LocSpam teamchat only"), &Vars.Misc.LocSpamChat); //ChatSpamDelay
				ImGui::Checkbox(charenc("Headshot say"), &Vars.Misc.TapSay);
				ImGui::Checkbox(charenc("say .Refund. on kill"), &Vars.Misc.SoundShit);
				ImGui::Checkbox(charenc("say Bomb Info"), &Vars.Misc.bombinfo);
				ImGui::Separator();
				ImGui::Text(charenc("Random Shit"));
				ImGui::Separator();
				ImGui::Checkbox(charenc("Ranks"), &Vars.Misc.Ranks);
				ImGui::Checkbox(charenc("Auto Accept"), &Vars.Misc.AutoAccept);
				ImGui::Checkbox(charenc("Watermark"), &Vars.Misc.Watermark);
				ImGui::Checkbox(charenc("AirStuck"), &Vars.Misc.AirStuck);
				ImGui::Combo(charenc("AirStuck Key"), &Vars.Misc.AirStuckKey, keyNames, IM_ARRAYSIZE(keyNames));
				ImGui::Separator();
				ImGui::Text("Clantag");
				ImGui::Separator();
				ImGui::Checkbox("Clantag Enabled", &Vars.Misc.ClantagChanger.enabled);
				ImGui::Columns(2, NULL, true);
				{
					//ImGui::PushItemWidth(-1);
					if (ImGui::InputText("##CLANTAGTEXT", Vars.Misc.ClantagChanger.value, 30))
						E::ClantagChanger->UpdateClanTagCallback();
					//ImGui::PopItemWidth();
					ImGui::Text("Animation Speed");
				}
				ImGui::NextColumn();
				{
					//ImGui::PushItemWidth(-1);
					if (ImGui::Combo("##ANIMATIONTYPE", &Vars.Misc.ClantagChanger.type, AnimationTypes, IM_ARRAYSIZE(AnimationTypes)))
						E::ClantagChanger->UpdateClanTagCallback();
					if (ImGui::SliderInt("##ANIMATIONSPEED", &Vars.Misc.ClantagChanger.animation_speed, 0, 2000))
						E::ClantagChanger->UpdateClanTagCallback();
					//ImGui::PopItemWidth();
				}
				ImGui::Columns(1);

				ImGui::Separator();
				ImGui::Text("Nickname");
				ImGui::Separator();
				static char nickname[128] = "";
				static char nickseparator[128] = "";
				static bool init_nick = false;
				if (!init_nick)
				{
					sprintf(nickname, "�AIMWARE.net");
					sprintf(nickseparator, "\\n");
					init_nick = true;
				}

				ImGui::Columns(2, NULL, true);
				{
					//ImGui::PushItemWidth(-1);
					ImGui::Text("Text");
					ImGui::InputText("##NICKNAMETEXT", nickname, 127);
					ImGui::Text("Separator");
					ImGui::InputText("##NICKNAMESEPARATOR", nickseparator, 127);
					//ImGui::PopItemWidth();
				}
				ImGui::NextColumn();
				{
					if (ImGui::Button("Set Nickname", ImVec2(-1, 0)))
					{
						std::string ctWithEscapesProcessed = std::string(nickname);
						U::StdReplaceStr(ctWithEscapesProcessed, "\\n", "\n");

						E::Misc->ChangeName(ctWithEscapesProcessed.c_str());
					}


					ImGui::Combo(charenc("SeparatorType"), &Vars.Misc.NameChanger.separator_animation, NameSpamerTypes, IM_ARRAYSIZE(NameSpamerTypes));

					if (ImGui::Button("NickSpamer", ImVec2(-1, 0)))
					{
						E::NameChanger->nickname = nickname;
						E::NameChanger->separator = nickseparator;
						Vars.Misc.NameChanger.enabled = true;
						//Vars.Misc.NameChanger.last_blank = true;
					}
				}
				ImGui::Columns(1);


				ImGui::Separator();
				ImGui::Text(charenc("Netvar Dump"));
				ImGui::Separator();
				if (ImGui::Button(charenc("Save Dump"))) NetVarManager->DumpNetvars();
			}
			ImGui::End(); //End Misc window
		}
	}

	ImGui::Render();

	return oEndScene(pDevice);
}

ResetFn oReset;
long __stdcall Hooks::Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (!G::d3dinit)
		return oReset(pDevice, pPresentationParameters);

	ImGui_ImplDX9_InvalidateDeviceObjects();

	auto hr = oReset(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();


	return hr;
}

void GUI_Init(IDirect3DDevice9* pDevice)
{
	ImGui_ImplDX9_Init(G::Window, pDevice);

	ImColor mainColor = ImColor(54, 54, 54, 255);
	ImColor bodyColor = ImColor(24, 24, 24, 240);
	ImColor fontColor = ImColor(255, 255, 255, 255);

	ImGuiStyle& style = ImGui::GetStyle();

	ImVec4 mainColorHovered = ImVec4(mainColor.Value.x + 0.1f, mainColor.Value.y + 0.1f, mainColor.Value.z + 0.1f, mainColor.Value.w);
	ImVec4 mainColorActive = ImVec4(mainColor.Value.x + 0.2f, mainColor.Value.y + 0.2f, mainColor.Value.z + 0.2f, mainColor.Value.w);
	ImVec4 menubarColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w - 0.8f);
	ImVec4 frameBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .1f);
	ImVec4 tooltipBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .05f);

	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(8, 8);
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ChildWindowRounding = 0.0f;
	style.FramePadding = ImVec2(4, 3);
	style.FrameRounding = 0.0f;
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 3.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	style.AntiAliasedShapes = true;
	style.CurveTessellationTol = 1.25f;

	style.Colors[ImGuiCol_Text] = fontColor;
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = bodyColor;
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(.0f, .0f, .0f, .0f);
	style.Colors[ImGuiCol_PopupBg] = tooltipBgColor;
	style.Colors[ImGuiCol_Border] = mainColor;
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = frameBgColor;
	style.Colors[ImGuiCol_FrameBgHovered] = mainColorHovered;
	style.Colors[ImGuiCol_FrameBgActive] = mainColorActive;
	style.Colors[ImGuiCol_TitleBg] = mainColor;
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = mainColor;
	style.Colors[ImGuiCol_MenuBarBg] = menubarColor;
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(frameBgColor.x + .05f, frameBgColor.y + .05f, frameBgColor.z + .05f, frameBgColor.w);
	style.Colors[ImGuiCol_ScrollbarGrab] = mainColor;
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ScrollbarGrabActive] = mainColorActive;
	style.Colors[ImGuiCol_ComboBg] = frameBgColor;
	style.Colors[ImGuiCol_CheckMark] = mainColor;
	style.Colors[ImGuiCol_SliderGrab] = mainColorHovered;
	style.Colors[ImGuiCol_SliderGrabActive] = mainColorActive;
	style.Colors[ImGuiCol_Button] = mainColor;
	style.Colors[ImGuiCol_ButtonHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ButtonActive] = mainColorActive;
	style.Colors[ImGuiCol_Header] = mainColor;
	style.Colors[ImGuiCol_HeaderHovered] = mainColorHovered;
	style.Colors[ImGuiCol_HeaderActive] = mainColorActive;
	style.Colors[ImGuiCol_Column] = mainColor;
	style.Colors[ImGuiCol_ColumnHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ColumnActive] = mainColorActive;
	style.Colors[ImGuiCol_ResizeGrip] = mainColor;
	style.Colors[ImGuiCol_ResizeGripHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ResizeGripActive] = mainColorActive;
	style.Colors[ImGuiCol_CloseButton] = mainColor;
	style.Colors[ImGuiCol_CloseButtonHovered] = mainColorHovered;
	style.Colors[ImGuiCol_CloseButtonActive] = mainColorActive;
	style.Colors[ImGuiCol_PlotLines] = mainColor;
	style.Colors[ImGuiCol_PlotLinesHovered] = mainColorHovered;
	style.Colors[ImGuiCol_PlotHistogram] = mainColor;
	style.Colors[ImGuiCol_PlotHistogramHovered] = mainColorHovered;
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	G::d3dinit = true;
}

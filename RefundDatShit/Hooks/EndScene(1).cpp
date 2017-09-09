#include "../Cheat.h"
#include "Menu.h"
#include "../Parser.h"
#include "../SkinChanger.h"
#include "../ImGUI/imgui_internal.h"
#include "../ImGUI/imgui.h"
#include <locale>

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

void GUI_Init(IDirect3DDevice9* pDevice);

void color()
{
	auto bColor = Vars.g_fBColor;
	auto mColor = Vars.g_fMColor;
	auto tColor = Vars.g_fTColor;
	//int(enemyColor[0] * 255.0f), int(enemyColor[1] * 255.0f), int(enemyColor[2] * 255.0f), 255

	ImColor mainColor = ImColor(int(mColor[0] * 255.0f), int(mColor[1] * 255.0f), int(mColor[2] * 255.0f), 255);
	ImColor bodyColor = ImColor(int(bColor[0] * 255.0f), int(bColor[1] * 255.0f), int(bColor[2] * 255.0f), 255);
	ImColor fontColor = ImColor(int(tColor[0] * 255.0f), int(tColor[1] * 255.0f), int(tColor[2] * 255.0f), 255);

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

	style.Colors[ImGuiCol_Tab] = mainColor;
	style.Colors[ImGuiCol_TabHovered] = mainColorHovered;
	style.Colors[ImGuiCol_TabActive] = mainColorActive;
	style.Colors[ImGuiCol_TabText] = ImColor(fontColor.Value.x - .6f, fontColor.Value.y - .6f, fontColor.Value.z - .6f, fontColor.Value.w);
	style.Colors[ImGuiCol_TabTextActive] = fontColor;
	style.Colors[ImGuiCol_TabSelected] = bodyColor;

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
}

char* KeyStrings[254] = { "No key", "Left Mouse", "Right Mouse", "Control+Break", "Middle Mouse", "Mouse 4", "Mouse 5",
nullptr, "Backspace", "TAB", nullptr, nullptr, nullptr, "ENTER", nullptr, nullptr, "SHIFT", "CTRL", "ALT", "PAUSE",
"CAPS LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "ESC", nullptr, nullptr, nullptr, nullptr, "SPACEBAR",
"PG UP", "PG DOWN", "END", "HOME", "Left", "Up", "Right", "Down", nullptr, "Print", nullptr, "Print Screen", "Insert",
"Delete", nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", "Left Windows", "Right Windows", nullptr, nullptr, nullptr, "NUM 0", "NUM 1", "NUM 2", "NUM 3", "NUM 4", "NUM 5", "NUM 6",
"NUM 7", "NUM 8", "NUM 9", "*", "+", "_", "-", ".", "/", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
"F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, "NUM LOCK", "SCROLL LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, "LSHIFT", "RSHIFT", "LCONTROL", "RCONTROL", "LMENU", "RMENU", nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "Next Track", "Previous Track", "Stop", "Play/Pause", nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[{", "\\|", "}]", "'\"", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

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

	void KeyBindButton(int Key)
	{
		static bool shouldListen = false;
		static int key = 0;
		if (shouldListen)
		{
			for (int i = 0; i < 255; i++)
			{
				if (GetAsyncKeyState(i))
				{
					if (i == VK_ESCAPE)
					{
						shouldListen = false;
						key = -1;
					}

					key = i;
					shouldListen = false;
				}

			}
		}
		if (ImGui::Button(KeyStrings[key], ImVec2(-1, 0)))
		{
			shouldListen = true;
		}

		Key = key;
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items = -1)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size(), height_in_items);
	}
}

void DrawConfigTab()
{
	ImGui::Checkbox(XorStr("Enable Skins"), &Vars.Skins.SSEnabled);


	if (Vars.Skins.SSEnabled)
	{
		ImGui::BeginChild(XorStr("##SKINS"), ImVec2(0, 0), true);
		{
			ImGui::PushItemWidth(270);
			if (G::LocalPlayer->GetAlive())
				ImGui::Text(XorStr("Current Weapon: %s"), G::LocalPlayer->GetWeapon()->GetWeaponName().c_str());
			else 
				ImGui::Text(XorStr("Invalid weapon/Isnt Alive"));


			ImGui::Combo(XorStr("Paint Kit"), &Vars.Skins.Weapons[Vars.wpn].PaintKit, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_Skins[idx].strName.c_str();
				return true;
			}, nullptr, k_Skins.size(), 10);
			//static int pkit = 0;
			/*ImGui::Combo(XorStr("glove Kit"), &pkit, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_Gloves[idx].strName.c_str();
				return true;
			}, nullptr, k_Gloves.size(), 10);*/
			//k_Skins[Vars.Skins.Weapons[pWeapon->GetItemDefinitionIndex()].PaintKit].iIndex;
			//ImGui::Text("%d", k_Gloves[pkit].iIndex);


			ImGui::Separator();
			//ImGui::Combo(XorStr("Knife Skin"), &Vars.Skins.KnifeSkin, KnifeSkin, IM_ARRAYSIZE(KnifeSkin));
			ImGui::Combo(XorStr("Knife Model"), &Vars.Skins.KnifeModel, charenc("No Knife\0\rBayonet\0\rFlip Knife\0\rGut Knife\0\rKarambit\0\rM9 Bayonet\0\rHuntsman Knife\0\rButterfly Knife\0\rFalchion Knife\0\rShadow Daggers\0\rBowie Knife\0\0"), -1, ImVec2(130, 0));
			ImGui::Separator();

			if (ImGui::Combo(XorStr("Glove Model"), &Vars.Skins.Glove, XorStr("No Glove\0\rBloodhound\0\rSport\0\rDriver\0\rWraps\0\rMoto\0\rSpecialist\0\0"), -1, ImVec2(130, 0)))
				U::CL_FullUpdate();

			const char* gstr;
			if (Vars.Skins.Glove == 1)
			{
				gstr = XorStr("Charred\0\rSnakebite\0\rBronzed\0\rGuerilla\0\0");
			}
			else if (Vars.Skins.Glove == 2)
			{
				gstr = XorStr("Hedge Maze\0\rPandoras Box\0\rSuperconductor\0\rArid\0\0");
			}
			else if (Vars.Skins.Glove == 3)
			{
				gstr = XorStr("Lunar Weave\0\rConvoy\0\rCrimson Weave\0\rDiamondback\0\0");
			}
			else if (Vars.Skins.Glove == 4)
			{
				gstr = XorStr("Leather\0\rSpruce DDPAT\0\rSlaughter\0\rBadlands\0\0");
			}
			else if (Vars.Skins.Glove == 5)
			{
				gstr = XorStr("Eclipse\0\rSpearmint\0\rBoom!\0\rCool Mint\0\0");
			}
			else if (Vars.Skins.Glove == 6)
			{
				gstr = XorStr("Forest DDPAT\0\rCrimson Kimono\0\rEmerald Web\0\rFoundation\0\0");
			}
			else
				gstr = XorStr("Select Glove type!\0\0");

			if (ImGui::Combo(XorStr("##2"), &Vars.Skins.GloveSkin, gstr, -1, ImVec2(130, 0)))
				U::CL_FullUpdate();

			if (ImGui::Button(XorStr("Full Update"), ImVec2(93.f, 20.f))) {
				U::CL_FullUpdate();
			}

			//ImGui::SliderFloat(XorStr("Wear"), &Vars.Skins.Weapons[Vars.wpn].Wear, 0.00000001f, 1.f);

			//ImGui::SliderInt(XorStr("StatTrak"), &Vars.Skins.Weapons[Vars.wpn].StatTrk, 0, 99999);
		}
		ImGui::EndChild();
	}
}
/*
void RageTab()
{
	ImGui::Checkbox(XorStr("RageBot Enabled"), &Vars.Ragebot.Enabled);
	ImGui::Separator();
	ImGui::Text(XorStr("Rage Aimbot"));
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Combo(XorStr("Key"), &Vars.Ragebot.HoldKey, keyNames, IM_ARRAYSIZE(keyNames));
		ImGui::Checkbox(XorStr("On Key"), &Vars.Ragebot.Hold);
		ImGui::Checkbox(XorStr("Auto Fire"), &Vars.Ragebot.AutoFire);
		ImGui::SliderFloat(XorStr("FOV"), &Vars.Ragebot.FOV, 1.f, 180.f, "%.0f");
		ImGui::Checkbox(XorStr("Silent"), &Vars.Ragebot.Silent);
		ImGui::Checkbox(XorStr("pSilent"), &Vars.Ragebot.pSilent);
		ImGui::Combo(XorStr("No Recoil"), &Vars.Ragebot.RCS, RcsType, IM_ARRAYSIZE(RcsType));
		ImGui::SliderFloat(XorStr("NoRecoil Force"), &Vars.Ragebot.RCShow, 1.f, 100.f, "%.0f");
		ImGui::Checkbox(XorStr("Aim Step"), &Vars.Ragebot.Aimstep);
		ImGui::SliderFloat(XorStr("Angle Per Tick"), &Vars.Ragebot.AimstepAmount, 1.f, 180.f, "%.0f");
	}
	ImGui::NextColumn();
	{
		ImGui::Combo(XorStr("Mode"), &Vars.Ragebot.TargetMethod, targetMode, IM_ARRAYSIZE(targetMode));
		ImGui::Combo(XorStr("Hitbox"), &Vars.Ragebot.Hitbox, aimBones, IM_ARRAYSIZE(aimBones));
		ImGui::Checkbox(XorStr("Friendly Fire"), &Vars.Ragebot.FriendlyFire);
		ImGui::Checkbox(XorStr("Auto Wall"), &Vars.Ragebot.AutoWall);
		ImGui::SliderFloat(XorStr("Min Damage"), &Vars.Ragebot.AutoWallDmg, 0.1f, 120.f, "%.1f");
		ImGui::Checkbox(XorStr("HitScan"), &Vars.Ragebot.HitScan);
		//ImGui::Checkbox(XorStr("HS ONLY HITSCAN"), &Vars.Ragebot.HVHhs);
	}
	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::Text(XorStr("Accuracy"));
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(XorStr("Auto Stop"), &Vars.Ragebot.AutoStop);
		ImGui::Checkbox(XorStr("Auto Scope"), &Vars.Ragebot.AutoScope);
		ImGui::Checkbox(XorStr("AutoStop until can shoot"), &Vars.Ragebot.AutoStopCanShoot);
	}
	ImGui::NextColumn();
	{
		ImGui::Checkbox(XorStr("AutoStop fast"), &Vars.Ragebot.AutoStopFast);
		ImGui::Checkbox(XorStr("Shoot When stopped"), &Vars.Ragebot.ShootWhenStopped);
		ImGui::Checkbox(XorStr("Auto Crouch"), &Vars.Ragebot.AutoCrouch);
	}
	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::Checkbox(XorStr("Hit Chance"), &Vars.Ragebot.HitChance);
	ImGui::SliderFloat(XorStr("Amount"), &Vars.Ragebot.HitChanceAmt, 1.f, 100.f, "%.1f");
	ImGui::Separator();
	ImGui::Text(XorStr("Anti-Aim"));
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(XorStr("Anti-Aim Enabled"), &Vars.Ragebot.Antiaim.Enabled);
		ImGui::Checkbox(XorStr("Use Choked Yaw"), &Vars.Ragebot.Antiaim.FakeYaw);
		ImGui::Checkbox(XorStr("At Target"), &Vars.Ragebot.Antiaim.AtPlayer);
//		ImGui::Checkbox(XorStr("Resolver Enabled"), &Vars.Ragebot.Antiaim.Resolver);
		ImGui::Combo(XorStr("Pitch"), &Vars.Ragebot.Antiaim.Pitch, antiaimpitch, IM_ARRAYSIZE(antiaimpitch));
		ImGui::Combo(XorStr("Yaw"), &Vars.Ragebot.Antiaim.YawReal, antiaimyaw, IM_ARRAYSIZE(antiaimyaw));
		ImGui::Combo(XorStr("Yaw Choked"), &Vars.Ragebot.Antiaim.YawFake, antiaimyaw, IM_ARRAYSIZE(antiaimyaw));
		//ImGui::Combo(XorStr("Zeta Test"), &Vars.Ragebot.Antiaim.Zeta, antiaimzeta, IM_ARRAYSIZE(antiaimzeta));
	}
	ImGui::NextColumn();
	{
		ImGui::SliderInt(XorStr("Spin Speed"), &Vars.Ragebot.Antiaim.spinspeed, 1,1000);
		ImGui::SliderInt(XorStr("Speed on Fake"), &Vars.Ragebot.Antiaim.spinspeedz, 1, 1000);
		bool unt = Vars.Ragebot.UntrustedCheck;
		if (!unt)
			ImGui::Text(XorStr("Enable AntiUntrusted to use custom aa"));
		else
		{
			ImGui::Checkbox(XorStr("Custom Pitch"), &Vars.Ragebot.Antiaim.PitchCustom);
			ImGui::SliderFloat(XorStr("Real Pitch"), &Vars.Ragebot.Antiaim.PCReal, unt ? -89 : -180, unt ? 89 : 180);
			ImGui::SliderFloat(XorStr("Fake Pitch"), &Vars.Ragebot.Antiaim.PCFake, unt ? -89 : -180, unt ? 89 : 180);
			ImGui::Checkbox(XorStr("Custom Yaw"), &Vars.Ragebot.Antiaim.YawCustom);
			ImGui::SliderFloat(XorStr("Real Yaw"), &Vars.Ragebot.Antiaim.YCReal, unt ? -179 : -360, unt ? 179 : 360);
			ImGui::SliderFloat(XorStr("Fake Yaw"), &Vars.Ragebot.Antiaim.YCFake, unt ? -179 : -360, unt ? 179 : 360);
		}
	}
	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::Text(XorStr("Disable on..."));
	ImGui::Separator();
	ImGui::Checkbox(XorStr("Knife"), &Vars.Ragebot.Antiaim.knife_held);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(XorStr("Stops your antiaim while you have your knife out."));

	ImGui::Checkbox(XorStr("No Enemy"), &Vars.Ragebot.Antiaim.no_enemy);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(XorStr("Stops your antiaim when there are no enemies visible."));

	ImGui::Separator();
	ImGui::Checkbox(XorStr("Edge"), &Vars.Ragebot.Antiaim.DetectEdge);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(XorStr("If someone edge, you can enable this thing and cheat will find best hitbox to kill him."));
	ImGui::SliderFloat(XorStr("EdgeDistance"), &Vars.Ragebot.Antiaim.EdgeDistance, 10, 30);
	ImGui::Separator();
	ImGui::Text(XorStr("HitScan Bones"));
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(XorStr("Head"), &Vars.Ragebot.bones[HITBOX_HEAD]);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(XorStr("HitScan head"));
		ImGui::Checkbox(XorStr("Neck"), &Vars.Ragebot.bones[HITBOX_NECK]);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(XorStr("HitScan neck"));
		ImGui::Checkbox(XorStr("Pelvis"), &Vars.Ragebot.bones[HITBOX_PELVIS]);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(XorStr("HitScan pelvis"));
	}
	ImGui::NextColumn();
	{
		ImGui::Checkbox(XorStr("Spine"), &Vars.Ragebot.bones[HITBOX_SPINE]);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(XorStr("HitScan spine"));
		ImGui::Checkbox(XorStr("Legs"), &Vars.Ragebot.bones[HITBOX_LEGS]);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(XorStr("HitScan legs"));
		ImGui::Checkbox(XorStr("Arms"), &Vars.Ragebot.bones[HITBOX_ARMS]);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(XorStr("HitScan arms"));
	}
	ImGui::Columns(1);

	ImGui::Separator();
	ImGui::Checkbox(XorStr("Anti-Untrusted"), &Vars.Ragebot.UntrustedCheck);
}

void LegitTab()
{
	ImGui::Text(XorStr("Legitbot Aimbot"));
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(XorStr("Enable Legit Aimbot"), &Vars.Legitbot.Aimbot.Enabled);
		ImGui::Checkbox(XorStr("Ignore Key"), &Vars.Legitbot.Aimbot.AlwaysOn);
		ImGui::Combo(XorStr("Aimbot Key"), &Vars.Legitbot.Aimbot.Key, keyNames, IM_ARRAYSIZE(keyNames));
		ImGui::Checkbox(XorStr("Aim on Key"), &Vars.Legitbot.Aimbot.OnKey);
		ImGui::Checkbox(XorStr("Friendly Fire"), &Vars.Legitbot.Aimbot.FriendlyFire);
	}
	ImGui::NextColumn();
	{
		ImGui::Combo(XorStr("Weapon Group"), &Vars.Legitbot.Aimbot.group, wepGroups, IM_ARRAYSIZE(wepGroups));
		switch (Vars.Legitbot.Aimbot.group)
		{
		case 0:
			ImGui::Checkbox(XorStr("Silent Aim"), &Vars.Legitbot.pistols.Silent);
			ImGui::Checkbox(XorStr("pSilent Aim"), &Vars.Legitbot.pistols.pSilent);
			ImGui::Combo(XorStr("Hitbox"), &Vars.Legitbot.pistols.Hitbox, aimBones, IM_ARRAYSIZE(aimBones));
			ImGui::SliderFloat(XorStr("FOV"), &Vars.Legitbot.pistols.FOV, 0.1f, 45.f, "%.2f");
			ImGui::SliderFloat(XorStr("Smooth"), &Vars.Legitbot.pistols.Speed, 0.1f, 50.f, "%.2f");
			//ImGui::SliderFloat(XorStr("Aim Delay"), &Vars.Legitbot.pistols.Delay, 0.f, 1000.f, "%.1f");
			ImGui::SliderFloat(XorStr("RCS Y"), &Vars.Legitbot.pistols.RCSAmountY, 1.f, 100.f, "%.0f");
			ImGui::SliderFloat(XorStr("RCS X"), &Vars.Legitbot.pistols.RCSAmountX, 1.f, 100.f, "%.0f");
			ImGui::Checkbox(XorStr("RCS Enabled"), &Vars.Legitbot.pistols.RCS);
			break;
		case 1:
			ImGui::Checkbox(XorStr("Silent Aim"), &Vars.Legitbot.rifles.Silent);
			ImGui::Checkbox(XorStr("pSilent Aim"), &Vars.Legitbot.rifles.pSilent);
			ImGui::Combo(XorStr("Hitbox"), &Vars.Legitbot.rifles.Hitbox, aimBones, IM_ARRAYSIZE(aimBones));
			ImGui::SliderFloat(XorStr("FOV"), &Vars.Legitbot.rifles.FOV, 0.1f, 45.f, "%.2f");
			ImGui::SliderFloat(XorStr("Smooth"), &Vars.Legitbot.rifles.Speed, 0.1f, 50.f, "%.2f");
			//ImGui::SliderFloat(XorStr("Aim Delay"), &Vars.Legitbot.Aimbot.Delay, 0.f, 1000.f, "%.1f");
			ImGui::SliderFloat(XorStr("RCS Y"), &Vars.Legitbot.rifles.RCSAmountY, 1.f, 100.f, "%.0f");
			ImGui::SliderFloat(XorStr("RCS X"), &Vars.Legitbot.rifles.RCSAmountX, 1.f, 100.f, "%.0f");
			ImGui::Checkbox(XorStr("RCS Enabled"), &Vars.Legitbot.rifles.RCS);
			break;
		case 2:
			ImGui::Checkbox(XorStr("Silent Aim"), &Vars.Legitbot.smg.Silent);
			ImGui::Checkbox(XorStr("pSilent Aim"), &Vars.Legitbot.smg.pSilent);
			ImGui::Combo(XorStr("Hitbox"), &Vars.Legitbot.smg.Hitbox, aimBones, IM_ARRAYSIZE(aimBones));
			ImGui::SliderFloat(XorStr("FOV"), &Vars.Legitbot.smg.FOV, 0.1f, 45.f, "%.2f");
			ImGui::SliderFloat(XorStr("Smooth"), &Vars.Legitbot.smg.Speed, 0.1f, 50.f, "%.2f");
			//ImGui::SliderFloat(XorStr("Aim Delay"), &Vars.Legitbot.smg.Delay, 0.f, 1000.f, "%.1f");
			ImGui::SliderFloat(XorStr("RCS Y"), &Vars.Legitbot.smg.RCSAmountY, 1.f, 100.f, "%.0f");
			ImGui::SliderFloat(XorStr("RCS X"), &Vars.Legitbot.smg.RCSAmountX, 1.f, 100.f, "%.0f");
			ImGui::Checkbox(XorStr("RCS Enabled"), &Vars.Legitbot.smg.RCS);
			break;
		case 3:
			ImGui::Checkbox(XorStr("Silent Aim"), &Vars.Legitbot.shotguns.Silent);
			ImGui::Checkbox(XorStr("pSilent Aim"), &Vars.Legitbot.shotguns.pSilent);
			ImGui::Combo(XorStr("Hitbox"), &Vars.Legitbot.shotguns.Hitbox, aimBones, IM_ARRAYSIZE(aimBones));
			ImGui::SliderFloat(XorStr("FOV"), &Vars.Legitbot.shotguns.FOV, 0.1f, 45.f, "%.2f");
			ImGui::SliderFloat(XorStr("Smooth"), &Vars.Legitbot.shotguns.Speed, 0.1f, 50.f, "%.2f");
			//ImGui::SliderFloat(XorStr("Aim Delay"), &Vars.Legitbot.Aimbot.Delay, 0.f, 1000.f, "%.1f");
			ImGui::SliderFloat(XorStr("RCS Y"), &Vars.Legitbot.shotguns.RCSAmountY, 1.f, 100.f, "%.0f");
			ImGui::SliderFloat(XorStr("RCS X"), &Vars.Legitbot.shotguns.RCSAmountX, 1.f, 100.f, "%.0f");
			ImGui::Checkbox(XorStr("RCS Enabled"), &Vars.Legitbot.shotguns.RCS);
			break;
		case 4:
			ImGui::Checkbox(XorStr("Silent Aim"), &Vars.Legitbot.snipers.Silent);
			ImGui::Checkbox(XorStr("pSilent Aim"), &Vars.Legitbot.snipers.pSilent);
			ImGui::Combo(XorStr("Hitbox"), &Vars.Legitbot.snipers.Hitbox, aimBones, IM_ARRAYSIZE(aimBones));
			ImGui::SliderFloat(XorStr("FOV"), &Vars.Legitbot.snipers.FOV, 0.1f, 45.f, "%.2f");
			ImGui::SliderFloat(XorStr("Smooth"), &Vars.Legitbot.snipers.Speed, 0.1f, 50.f, "%.2f");
			//ImGui::SliderFloat(XorStr("Aim Delay"), &Vars.Legitbot.Aimbot.Delay, 0.f, 1000.f, "%.1f");
			ImGui::SliderFloat(XorStr("RCS Y"), &Vars.Legitbot.snipers.RCSAmountY, 1.f, 100.f, "%.0f");
			ImGui::SliderFloat(XorStr("RCS X"), &Vars.Legitbot.snipers.RCSAmountX, 1.f, 100.f, "%.0f");
			ImGui::Checkbox(XorStr("RCS Enabled"), &Vars.Legitbot.snipers.RCS);
			ImGui::Checkbox(XorStr("FastZoom"), &Vars.Legitbot.snipers.fastzoom);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("Switch wep"), &Vars.Legitbot.snipers.fastzoomswitch);
			break;
		}
	}
	ImGui::Columns(1);

	ImGui::Separator();
	ImGui::Text(XorStr("Triggerbot"));
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(XorStr("Enable Triggerbot"), &Vars.Legitbot.Triggerbot.Enabled);
		ImGui::Checkbox(XorStr("Auto Fire"), &Vars.Legitbot.Triggerbot.AutoFire);
		ImGui::Checkbox(XorStr("Auto Wall"), &Vars.Legitbot.Triggerbot.AutoWall);
		ImGui::Combo(XorStr("Triggerbot Key"), &Vars.Legitbot.Triggerbot.Key, keyNames, IM_ARRAYSIZE(keyNames));
		ImGui::Checkbox(XorStr("Hit Chance"), &Vars.Legitbot.Triggerbot.HitChance);
		ImGui::SliderFloat(XorStr("Amount"), &Vars.Legitbot.Triggerbot.HitChanceAmt, 1.f, 100.f, "%.0f");
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XorStr("Filter"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 19 * 6));
		{
			ImGui::Selectable(XorStr("Trigger When On Head"), &Vars.Legitbot.Triggerbot.Filter.Head);
			ImGui::Selectable(XorStr("Trigger When On Chest"), &Vars.Legitbot.Triggerbot.Filter.Chest);
			ImGui::Selectable(XorStr("Trigger When On Stomach"), &Vars.Legitbot.Triggerbot.Filter.Stomach);
			ImGui::Selectable(XorStr("Trigger When On Arms"), &Vars.Legitbot.Triggerbot.Filter.Arms);
			ImGui::Selectable(XorStr("Trigger When On Legs"), &Vars.Legitbot.Triggerbot.Filter.Legs);
			ImGui::Selectable(XorStr("Trigger On Teammates"), &Vars.Legitbot.Triggerbot.Filter.Friendly);
		}
		ImGui::EndChild();
	}
	ImGui::Columns(1);
	ImGui::Separator();
}

void VisualsTab()
{
	ImGui::Text(XorStr("Visuals"));
	ImGui::Separator();
	ImGui::Columns(2, XorStr("##c1"), false);
	ImGui::Checkbox(XorStr("Enable Visuals"), &Vars.Visuals.Enabled);
	ImGui::Checkbox(XorStr("2D Box"), &Vars.Visuals.Box);
	ImGui::Checkbox(XorStr("3D Box"), &Vars.Visuals.esp3d);
	ImGui::Checkbox(XorStr("Skeleton"), &Vars.Visuals.Skeleton);
	ImGui::NextColumn();
	ImGui::Checkbox(XorStr("Glow"), &Vars.Visuals.Glow);
	ImGui::Checkbox(XorStr("SpectatorList"), &Vars.Visuals.SpectatorList);
	ImGui::Checkbox(XorStr("Bullet Trace"), &Vars.Visuals.BulletTrace);
	ImGui::Columns(1);
	ImGui::Checkbox(XorStr("Vector to Enemies"), &Vars.Visuals.Line);
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::BeginChild(XorStr("info"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 19 * 4));
		{
			ImGui::Selectable(XorStr(" Show Name"), &Vars.Visuals.InfoName);
			ImGui::Selectable(XorStr(" Show Health"), &Vars.Visuals.InfoHealth);
			ImGui::Selectable(XorStr(" Show Weapon"), &Vars.Visuals.InfoWeapon);
			ImGui::Selectable(XorStr(" Show If Flashed"), &Vars.Visuals.InfoFlashed);
		}
		ImGui::EndChild();
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XorStr("filter"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 19 * 5));
			{
				ImGui::Selectable(XorStr(" Show Enemies"), &Vars.Visuals.Filter.Enemies);
				ImGui::Selectable(XorStr(" Show Teammates"), &Vars.Visuals.Filter.Friendlies);
				ImGui::Selectable(XorStr(" Show Weapons"), &Vars.Visuals.Filter.Weapons);
				ImGui::Selectable(XorStr(" Show Decoy"), &Vars.Visuals.Filter.Decoy);
				ImGui::Selectable(XorStr(" Show C4"), &Vars.Visuals.Filter.C4);
			}
		}
	}
	ImGui::EndChild();
	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::Text(XorStr("Chams / Radar"));
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(XorStr("Enable Chams"), &Vars.Visuals.Chams.Enabled);
		ImGui::Combo(XorStr("Mode"), &Vars.Visuals.Chams.Mode, chamsMode, IM_ARRAYSIZE(chamsMode));
		ImGui::Checkbox(XorStr("XQZ"), &Vars.Visuals.Chams.XQZ);
		ImGui::Checkbox(XorStr("Hands"), &Vars.Visuals.Chams.hands);
		ImGui::SameLine();
		ImGui::Checkbox(XorStr("Weapon"), &Vars.Visuals.ChamsWeapon);
		ImGui::Checkbox(XorStr("Rainbow Hands"), &Vars.Visuals.Chams.RainbowHands);
		ImGui::SameLine();
		ImGui::SliderFloat(XorStr("Speed"), &Vars.Visuals.Chams.RainbowTime, 1.f, 150.f, "%.0f");
	}
	ImGui::NextColumn();
	{
		ImGui::Checkbox(XorStr("Enable Radar"), &Vars.Visuals.Radar.Enabled);
		/*ImGui::SliderInt(XorStr("Radar X"), &Vars.Visuals.Radar.X, 0, 2000);
		ImGui::SliderInt(XorStr("Radar Y"), &Vars.Visuals.Radar.Y, 0, 2000);
		ImGui::SliderInt(XorStr("Radar Size"), &Vars.Visuals.Radar.Size, 50, 200);
		ImGui::SliderInt(XorStr("Radar Range"), &Vars.Visuals.Radar.range, 1, 100);
	}
	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::Text(XorStr("Removals"));
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(XorStr("No Vis Recoil"), &Vars.Visuals.RemovalsVisualRecoil);
		ImGui::Checkbox(XorStr("No Flash"), &Vars.Visuals.RemovalsFlash);
	}
	ImGui::NextColumn(); {
		ImGui::Checkbox(XorStr("Crosshair Enabled"), &Vars.Visuals.CrosshairOn);
		ImGui::Checkbox(XorStr("Show Spread"), &Vars.Visuals.CrosshairSpread);
		ImGui::Checkbox(XorStr("Show FOV"), &Vars.Visuals.CrosshairFOV);
		ImGui::Checkbox(XorStr("Show Recoil"), &Vars.Visuals.CrosshairType);
		ImGui::Combo(XorStr("Crosshair Style"), &Vars.Visuals.CrosshairStyle, crosshairStyle, IM_ARRAYSIZE(crosshairStyle));
	}
	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::Checkbox(XorStr("ThirdPerson"), &Vars.Visuals.thirdperson.enabled);
	ImGui::SliderFloat(XorStr("ThirdPerson dist"), &Vars.Visuals.thirdperson.distance, 30.f, 200.f, "%.0f");

	if (ImGui::Button(XorStr("ThirdPerson"), ImVec2(93.f, 20.f)))
	{
		static ConVar* Meme = I::Cvar->FindVar(XorStr("thirdperson"));
		Meme->nFlags &= ~FCVAR_CHEAT;

		I::Engine->ExecuteClientCmd(XorStr("thirdperson"));
	}
	ImGui::SameLine();
	if (ImGui::Button(XorStr("FirstPerson"), ImVec2(93.f, 20.f)))
	{
		static ConVar* Meme = I::Cvar->FindVar(XorStr("firstperson"));
		Meme->nFlags &= ~FCVAR_CHEAT;
		I::Engine->ExecuteClientCmd(XorStr("firstperson"));
	}

	ImGui::Separator();
}

void MiscTab()
{
	ImGui::Text(XorStr("Misc"));
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::SliderInt(XorStr("FakeLag"), &Vars.Misc.FakeLags, 0, 16);
		ImGui::SliderInt(XorStr("Change Fov"), &Vars.Misc.fov, -70, 70);
	}
	ImGui::NextColumn();
	{
		ImGui::Checkbox(XorStr("Bunny Hop"), &Vars.Misc.Bhop);
		ImGui::Combo(XorStr("Autostrafe"), &Vars.Misc.AutoStrafe, strafes, IM_ARRAYSIZE(strafes));
	}
	ImGui::Columns(1);
	ImGui::Separator();
//	ImGui::Combo(XorStr("Color Nick"), &Vars.Misc.Cname, cNametype, IM_ARRAYSIZE(cNametype));
	ImGui::Checkbox(XorStr("Name Steal"), &Vars.Misc.NameSteal);
	ImGui::SameLine();
	ImGui::SliderFloat(XorStr("Interval"), &Vars.Misc.NameStealDelay, 0.1f, 10.f);
	ImGui::Separator();
	ImGui::Text(XorStr("Chat Spam"));
	ImGui::Separator();
	ImGui::InputText(XorStr("Spam Message"), Vars.Misc.ChatSpamMode, 128);
	ImGui::SliderFloat(XorStr("Spam Delay"), &Vars.Misc.ChatSpamDelay, 0.1f, 10.f);
	ImGui::Checkbox(XorStr("ChatSpam"), &Vars.Misc.ChatSpam);
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(XorStr("Location Spam"), &Vars.Misc.LocSpam);
		ImGui::Checkbox(XorStr("LocSpam Enemies only"), &Vars.Misc.LocSpamEnemies);
		ImGui::Checkbox(XorStr("LocSpam teamchat only"), &Vars.Misc.LocSpamChat); //ChatSpamDelay
	}
	ImGui::NextColumn();
	{
		ImGui::Checkbox(XorStr("Headshot say"), &Vars.Misc.TapSay);
		ImGui::Checkbox(XorStr("say .Refund. on kill"), &Vars.Misc.SoundShit);
		ImGui::Checkbox(XorStr("say Bomb Info"), &Vars.Misc.bombinfo);
	}
	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::Text(XorStr("Misc Functions"));
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(XorStr("Ranks"), &Vars.Misc.Ranks);
		ImGui::Checkbox(XorStr("Auto Accept"), &Vars.Misc.AutoAccept);
	}
	ImGui::NextColumn();
	{
		ImGui::Checkbox(XorStr("Watermark"), &Vars.Misc.Watermark);
		ImGui::Checkbox(XorStr("Air Stuck"), &Vars.Misc.AirStuck);
		ImGui::Combo(XorStr("Air Stuck Key"), &Vars.Misc.AirStuckKey, keyNames, IM_ARRAYSIZE(keyNames));
	}
	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::Text(XorStr("Clantag"));
	ImGui::Separator();
	ImGui::Checkbox(XorStr("Clantag Enabled"), &Vars.Misc.ClantagChanger.enabled);
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		if (ImGui::InputText("##CLANTAGTEXT", Vars.Misc.ClantagChanger.value, 30))
			E::ClantagChanger->UpdateClanTagCallback();
		ImGui::Text(XorStr("Animation Speed"));
	}
	ImGui::NextColumn();
	{
		if (ImGui::Combo("##ANIM", &Vars.Misc.ClantagChanger.type, AnimationTypes, IM_ARRAYSIZE(AnimationTypes)))
			E::ClantagChanger->UpdateClanTagCallback();
		if (ImGui::SliderInt("##ANIMSPEED", &Vars.Misc.ClantagChanger.animation_speed, 0, 2000))
			E::ClantagChanger->UpdateClanTagCallback();
	}
	ImGui::Columns(1);

	ImGui::Separator();
	ImGui::Text(XorStr("Nickname"));
	ImGui::Separator();
	static char nickname[128] = "";
	static char nickseparator[128] = "";
	static bool init_nick = false;
	if (!init_nick)
	{
		sprintf(nickname, "MorphEngine.cc");
		sprintf(nickseparator, "\\n");
		init_nick = true;
	}

	ImGui::Columns(2, NULL, true);
	{
		ImGui::Text(XorStr("Text"));
		ImGui::InputText("##NICTEXT", nickname, 127);
		ImGui::Text(XorStr("Separator"));
		ImGui::InputText("##NICTOR", nickseparator, 127);
	}
	ImGui::NextColumn();
	{
		if (ImGui::Button(XorStr("Set Nickname"), ImVec2(-1, 0)))
		{
			std::string ctWithEscapesProcessed = std::string(nickname);
			U::StdReplaceStr(ctWithEscapesProcessed, "\\n", "\n");

			E::Misc->ChangeName(ctWithEscapesProcessed.c_str());
		}


		ImGui::Combo(XorStr("SeparatorType"), &Vars.Misc.NameChangerseparator_animation, NameSpamerTypes, IM_ARRAYSIZE(NameSpamerTypes));

		if (ImGui::Button(XorStr("NickSpamer"), ImVec2(-1, 0)))
		{
			E::NameChanger->nickname = nickname;
			E::NameChanger->separator = nickseparator;
			Vars.Misc.NameChangerenabled = true;
			//Vars.Misc.NameChanger.last_blank = true;
		}
	}
	ImGui::Columns(1);
	ImGui::Separator();
	//ImGui::Text(XorStr("DEBUG FUNCTIONS"));
	//ImGui::Separator();
	//if (ImGui::Button(XorStr("Save Dump"))) NetVarManager->DumpNetvars();
}


void SkinsTab()
{
	//ImGui::Separator();
	ImGui::Checkbox(XorStr("Enable Skins"), &Vars.Skins.SSEnabled);

	ImGui::Separator();
	ImGui::PushItemWidth(190);
	ImGui::Combo(XorStr("M4A1-S"), &Vars.Skins.M41S, M4A1Ss, IM_ARRAYSIZE(M4A1Ss));
	ImGui::Combo(XorStr("M4A4"), &Vars.Skins.M4A4, M4A1s, IM_ARRAYSIZE(M4A1s));
	ImGui::Combo(XorStr("AK47"), &Vars.Skins.AK47, AK47s, IM_ARRAYSIZE(AK47s));
	ImGui::Combo(XorStr("AWP"), &Vars.Skins.AWP, AWPs, IM_ARRAYSIZE(AWPs));
	ImGui::Combo(XorStr("Glock"), &Vars.Skins.Glock, GLOCKs, IM_ARRAYSIZE(GLOCKs));
	ImGui::Combo(XorStr("Berettas"), &Vars.Skins.BERETTAS, Berettass, IM_ARRAYSIZE(Berettass));
	ImGui::Combo(XorStr("USP"), &Vars.Skins.USPS, USPSs, IM_ARRAYSIZE(USPSs));
	ImGui::Combo(XorStr("Deagle"), &Vars.Skins.Magnum, DEAGLEs, IM_ARRAYSIZE(DEAGLEs));
	ImGui::Separator();
	//ImGui::Combo(XorStr("Knife Skin"), &Vars.Skins.KnifeSkin, KnifeSkin, IM_ARRAYSIZE(KnifeSkin));
	ImGui::Combo(XorStr("Knife Model"), &Vars.Skins.KnifeModel, Knifes, IM_ARRAYSIZE(Knifes));
	ImGui::SliderInt(XorStr("##KNF"), &Vars.Skins.KnifeSkin, 0, 20);
	ImGui::Separator();

	if (ImGui::Combo(XorStr("Glove Model"), &Vars.Skins.Glove, Gloves, IM_ARRAYSIZE(Gloves)))
			U::CL_FullUpdate();

		const char* gstr;
		if (Vars.Skins.Glove == 1)
		{
			gstr = XorStr("Charred\0\rSnakebite\0\rBronzed\0\rGuerilla\0\0");
		}
		else if (Vars.Skins.Glove == 2)
		{
			gstr = XorStr("Hedge Maze\0\rPandoras Box\0\rSuperconductor\0\rArid\0\0");
		}
		else if (Vars.Skins.Glove == 3)
		{
			gstr = XorStr("Lunar Weave\0\rConvoy\0\rCrimson Weave\0\rDiamondback\0\0");
		}
		else if (Vars.Skins.Glove == 4)
		{
			gstr = XorStr("Leather\0\rSpruce DDPAT\0\rSlaughter\0\rBadlands\0\0");
		}
		else if (Vars.Skins.Glove == 5)
		{
			gstr = XorStr("Eclipse\0\rSpearmint\0\rBoom!\0\rCool Mint\0\0");
		}
		else if (Vars.Skins.Glove == 6)
		{
			gstr = XorStr("Forest DDPAT\0\rCrimson Kimono\0\rEmerald Web\0\rFoundation\0\0");
		}
		else
			gstr = XorStr("Select Glove type!");

		if (ImGui::Combo(XorStr("##2"), &Vars.Skins.GloveSkin, gstr, -1, ImVec2(130, 0)))
			U::CL_FullUpdate();

	//ImGui::Separator(); 

	/*if (ImGui::Button(XorStr("Update"), ImVec2(93.f, 20.f))) {
		I::Engine->ClientCmd_Unrestricted("record x;stop");
	}
	ImGui::SameLine();
	if (ImGui::Button(XorStr("Full Update"), ImVec2(93.f, 20.f))) {
		U::CL_FullUpdate();
	}

	ImGui::SliderFloat(XorStr("Wear"), &Vars.Skins.Wear, 0.01f, 1.f);

	ImGui::SliderInt(XorStr("StatTrak"), &Vars.Skins.StatTrk, 0, 99999);
}
*/
#pragma once

bool get_system_font_path(const std::string& name, std::string& path)
{
	//
	// This code is not as safe as it should be.
	// Assumptions we make:
	//  -> GetWindowsDirectoryA does not fail.
	//  -> The registry key exists.
	//  -> The subkeys are ordered alphabetically
	//  -> The subkeys name and data are no longer than 260 (MAX_PATH) chars.
	//

	char buffer[MAX_PATH];
	HKEY registryKey;

	GetWindowsDirectoryA(buffer, MAX_PATH);
	std::string fontsFolder = buffer + std::string("\\Fonts\\");

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &registryKey)) {
		return false;
	}

	uint32_t valueIndex = 0;
	char valueName[MAX_PATH];
	uint8_t valueData[MAX_PATH];
	std::wstring wsFontFile;

	do {
		uint32_t valueNameSize = MAX_PATH;
		uint32_t valueDataSize = MAX_PATH;
		uint32_t valueType;

		auto error = RegEnumValueA(
			registryKey,
			valueIndex,
			valueName,
			reinterpret_cast<DWORD*>(&valueNameSize),
			0,
			reinterpret_cast<DWORD*>(&valueType),
			valueData,
			reinterpret_cast<DWORD*>(&valueDataSize));

		valueIndex++;

		if (error == ERROR_NO_MORE_ITEMS) {
			RegCloseKey(registryKey);
			return false;
		}

		if (error || valueType != REG_SZ) {
			continue;
		}

		if (_strnicmp(name.data(), valueName, name.size()) == 0) {
			path = fontsFolder + std::string((char*)valueData, valueDataSize);
			RegCloseKey(registryKey);
			return true;
		}
	} while (true);

	return false;
}

	bool legitTab = true;
	bool visualsTab = false;
	bool radarTab = false;
	bool rageTab = false;
	bool configTab = false;
	bool miscTab = false;

	bool visEspTab = true;
	bool visChamsTab = false;
	bool visHandsTab = false;
	bool visMiscTab = false;

	int windowWidth = 800;
	int windowHeight = 425;
	int curWidth = windowWidth;
	int curHeight = windowHeight;
	int curX = 40;
	int curY = 70;
	int tabHeight = 30;

	LONGLONG LastFrameTime;
	float AndeltaTime;

	LONGLONG Anmilliseconds_now() {
		static LARGE_INTEGER s_frequency;
		static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
		if (s_use_qpc) {
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);
			return (1000LL * now.QuadPart) / s_frequency.QuadPart;
		}
		else {
			return GetTickCount();
		}
	}

	float GetAnDeltaTime()
	{
		LONGLONG ms = Anmilliseconds_now();
		float ret = ms - LastFrameTime;
		LastFrameTime = ms;
		return ret;
		//return 1000.0f / ImGui::GetIO().Framerate;
	}

	int CalcTabWidth(int tabs)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		//return ((windowWidth - (style.WindowPadding.x * 2)) / tabs) - (style.ItemSpacing.x * tabs);
		return (windowWidth - ((style.WindowPadding.x) + (style.ItemSpacing.x * tabs))) / tabs;
		//return windowWidth / tabs;
	}

	void SelectTab(bool* tab)
	{
		legitTab = false;
		visualsTab = false;
		radarTab = false;
		rageTab = false;
		configTab = false;
		miscTab = false;

		*tab = true;
	}

	void SelectVisualsSubTab(bool* tab)
	{
		visEspTab = false;
		visChamsTab = false;
		visHandsTab = false;
		visMiscTab = false;

		*tab = true;
	}

	std::vector<std::string> configs;

	void GetConfigMassive()
	{
		//get all files on folder

		configs.clear();

		static char path[MAX_PATH];
		std::string szPath1;

		if (!SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path)))
			return;

		szPath1 = std::string(path) + XorStr("\\Morph\\*");
		

		WIN32_FIND_DATA FindFileData;
		HANDLE hf;
		configs.push_back("default.ini");

		hf = FindFirstFile(szPath1.c_str(), &FindFileData);
		if (hf != INVALID_HANDLE_VALUE) {
			do {
				std::string filename = FindFileData.cFileName;

				if (filename == ".")
					continue;

				if (filename == "..")
					continue;

				if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					if (filename.find(".ini") != std::string::npos)
					{
						configs.push_back(std::string(filename));
					}
				}
			} while (FindNextFile(hf, &FindFileData) != 0);
			FindClose(hf);
		}
	}

	bool DrawTab(const char* name, ImVec2 tabsize, bool *boole, bool forceTabs = true)
	{
		bool ret = false;
		if (ImGui::Tab(name, tabsize, *boole))
		{
			ret = true;
			*boole = true;
			if (forceTabs) SelectTab(boole);
		}

		//ImGui::SameLine(0);
		return ret;
	}

	void DrawLegitTab()
	{
		ImGui::Columns(2, NULL, true,true);
		{
			ImGui::Checkbox(XorStr("Enable Legit Aimbot"), &Vars.Legitbot.Aimbot.Enabled);
			ImGui::Checkbox(XorStr("Ignore Key"), &Vars.Legitbot.Aimbot.AlwaysOn);
			ImGui::Combo(XorStr("Aimbot Key"), &Vars.Legitbot.Aimbot.Key, keyNames, IM_ARRAYSIZE(keyNames));
			//ImGui::KeyBindButton(Vars.Legitbot.Aimbot.Key);
			ImGui::Checkbox(XorStr("Aim on Key"), &Vars.Legitbot.Aimbot.OnKey);
			ImGui::Checkbox(XorStr("Friendly Fire"), &Vars.Legitbot.Aimbot.FriendlyFire);
			ImGui::Checkbox(XorStr("Smoke Check"), &Vars.Legitbot.Aimbot.SmokeCheck);
			ImGui::Checkbox(XorStr("AutoPistol"), &Vars.Legitbot.Aimbot.AutoPistol);
			//ImGui::SliderInt(XorStr("Delay"), &Vars.Legitbot.delay, 1, 100);
		}
		ImGui::NextColumn();
		{
			if (G::LocalPlayer->GetAlive() && G::LocalPlayer->GetWeapon()->IsGun()) {
				ImGui::Text("Current Weapon: %s", G::LocalPlayer->GetWeapon()->GetWeaponName().c_str());
				int curweapon = G::LocalPlayer->GetWeapon()->GetItemDefinitionIndex();
				ImGui::Checkbox(XorStr("pSilent Aim"), &Vars.Legitbot.Weapon[curweapon].pSilent);
				ImGui::Combo(XorStr("Hitbox"), &Vars.Legitbot.Weapon[curweapon].Hitbox, charenc("PELVIS\0\r\0\r\0\rHIP\0\rLOWER SPINE\0\rMIDDLE SPINE\0\rUPPER SPINE\0\rNECK\0\rHEAD\0\rNEAREST\0\0"), -1);
				if (Vars.Legitbot.Weapon[curweapon].pSilent)
				ImGui::SliderFloat(XorStr("pSilent FOV"), &Vars.Legitbot.Weapon[curweapon].PFOV, 0.1f, 3.f, "%.2f");
				ImGui::SliderFloat(XorStr("FOV"), &Vars.Legitbot.Weapon[curweapon].FOV, 0.1f, 50.f, "%.2f");
				if (!Vars.Legitbot.Weapon[curweapon].AdaptiveSmooth)
				ImGui::SliderFloat(XorStr("Smooth"), &Vars.Legitbot.Weapon[curweapon].Speed, 0.1f, 100.f, "%.2f");
				//ImGui::Checkbox(XorStr("Adaptive Smooth"), &Vars.Legitbot.Weapon[curweapon].AdaptiveSmooth);
				if (!G::LocalPlayer->GetWeapon()->IsSniper()) {
					ImGui::SliderFloat(XorStr("RCS Y"), &Vars.Legitbot.Weapon[curweapon].RCSAmountY, 1.f, 100.f, "%.0f",1.f,ImVec2(80.5f,30));
					ImGui::SameLine();
					ImGui::SliderFloat(XorStr("RCS X"), &Vars.Legitbot.Weapon[curweapon].RCSAmountX, 1.f, 100.f, "%.0f", 1.f, ImVec2(80.5f, 30));
				}
				//ImGui::Checkbox(XorStr("RCS Enabled"), &Vars.Legitbot.Weapon[curweapon].RCS);

				if (G::LocalPlayer->GetWeapon()->IsSniper()) {
					ImGui::Checkbox(XorStr("FastZoom"), &Vars.Legitbot.fastzoom);
					ImGui::SameLine();
					ImGui::Checkbox(XorStr("Switch wep"), &Vars.Legitbot.fastzoomswitch);
				}
			}
			else
			{
				ImGui::Text(XorStr("Invalid weapon/Isnt Alive"));
			}
		}
		ImGui::Columns(1);

		ImGui::Separator();
		ImGui::Columns(2, NULL, true, true);
		{
			ImGui::Checkbox(XorStr("Enable Triggerbot"), &Vars.Legitbot.Triggerbot.Enabled);	
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("Ignore Key"), &Vars.Legitbot.Triggerbot.AutoFire);
			if (Vars.Legitbot.Triggerbot.AutoFire == false)
				ImGui::Combo(XorStr("Key"), &Vars.Legitbot.Triggerbot.Key, keyNames, IM_ARRAYSIZE(keyNames));
			ImGui::Checkbox(XorStr("Auto Wall"), &Vars.Legitbot.Triggerbot.AutoWall);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("Hit Chance"), &Vars.Legitbot.Triggerbot.HitChance);

			ImGui::SliderFloat(XorStr("Amount"), &Vars.Legitbot.Triggerbot.HitChanceAmt, 1.f, 100.f, "%.0f");
			ImGui::SliderInt(XorStr("Delay"), &Vars.Legitbot.Triggerbot.Delay, 1, 1000);
			ImGui::Text("");
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XorStr("Filter"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 19 * 6));
			{
				ImGui::Selectable(XorStr("Trigger When On Head"), &Vars.Legitbot.Triggerbot.Filter.Head);
				ImGui::Selectable(XorStr("Trigger When On Chest"), &Vars.Legitbot.Triggerbot.Filter.Chest);
				ImGui::Selectable(XorStr("Trigger When On Stomach"), &Vars.Legitbot.Triggerbot.Filter.Stomach);
				ImGui::Selectable(XorStr("Trigger When On Arms"), &Vars.Legitbot.Triggerbot.Filter.Arms);
				ImGui::Selectable(XorStr("Trigger When On Legs"), &Vars.Legitbot.Triggerbot.Filter.Legs);
				ImGui::Selectable(XorStr("Trigger On Teammates"), &Vars.Legitbot.Triggerbot.Filter.Friendly);
			}
			ImGui::EndChild();
		}
		ImGui::Columns(1);
	}

	void DrawColorsTab()
	{
		/*static int selectedItem = 0;

		ImVec2 lastCursor = ImGui::GetCursorPos();

		ImGui::ListBoxHeader("##0", ImVec2(240, 520));
		for (int i = 0; i < ColorsForPicker.size(); i++)
		{
			bool selected = i == selectedItem;

			if (ImGui::Selectable(ColorsForPicker[i].Name, selected))
				selectedItem = i;
		}
		ImGui::ListBoxFooter();

		float nc = lastCursor.x + 260;
		ImGui::SetCursorPos(ImVec2(nc, lastCursor.y));

		Color col = ColorsForPicker[selectedItem];
		int r = (col.Ccolor[0] * 255.f);
		int g = (col.Ccolor[1] * 255.f);
		int b = (col.Ccolor[2] * 255.f);
		//int a = (*col.Ccolor[3] * 255.f);

		//ImGui::NewLine(); ImGui::SetCursorPosX(nc);
		//ImGui::SliderInt("A", &a, 0, 255, "%.0f", ImVec4(0, 0, 0, 255));

		col.Ccolor[0] = r / 255.0f;
		col.Ccolor[1] = g / 255.0f;
		col.Ccolor[2] = b / 255.0f;
		//col.Ccolor[3] = a / 255.0f;

		ImGui::NewLine(); ImGui::SetCursorPosX(nc);
		ImVec2 curPos = ImGui::GetCursorPos();
		ImVec2 curWindowPos = ImGui::GetWindowPos();
		curPos.x += curWindowPos.x;
		curPos.y += curWindowPos.y;


		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(curPos.x + 255, curPos.y),
			ImVec2(curWindowPos.x + curWidth - 25, curPos.y + 200),
			ImGui::GetColorU32(ImVec4(col.Ccolor[0], col.Ccolor[1], col.Ccolor[2], 255)));

		if (ImGui::ColorPicker("##COLOR_PICKER", col.Ccolor))
			color;

		//ImGui::NewLine(); ImGui::NewLine(); ImGui::SetCursorPosX(nc);*/
	}

	Vector2D RotatePoint(Vector EntityPos, Vector LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck, bool angleInRadians = false)
	{
		float r_1, r_2;
		float x_1, y_1;

		r_1 = -(EntityPos.y - LocalPlayerPos.y);
		r_2 = EntityPos.x - LocalPlayerPos.x;
		float Yaw = angle - 90.0f;

		float yawToRadian = Yaw * (float)(M_PI / 180.0F);
		x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
		y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

		*viewCheck = y_1 < 0;

		x_1 *= zoom;
		y_1 *= zoom;

		int sizX = sizeX / 2;
		int sizY = sizeY / 2;

		x_1 += sizX;
		y_1 += sizY;

		if (x_1 < 5)
			x_1 = 5;

		if (x_1 > sizeX - 5)
			x_1 = sizeX - 5;

		if (y_1 < 5)
			y_1 = 5;

		if (y_1 > sizeY - 5)
			y_1 = sizeY - 5;


		x_1 += posX;
		y_1 += posY;


		return Vector2D(x_1, y_1);


		/*if (!angleInRadians)
		angle = (float)(angle * (M_PI / 180));
		float cosTheta = (float)cos(angle);
		float sinTheta = (float)sin(angle);
		Vector2 returnVec = Vector2(
		cosTheta * (pointToRotate.x - centerPoint.x) - sinTheta * (pointToRotate.y - centerPoint.y),
		sinTheta * (pointToRotate.x - centerPoint.x) + cosTheta * (pointToRotate.y - centerPoint.y)
		);
		returnVec += centerPoint;
		return returnVec / zoom;*/
	}

	bool EntityIsInvalid(CBaseEntity* Entity)
	{
		//HANDLE obs = Entity->GetObserverTargetHandle();
		//CBaseEntity *pTarget = I::ClientEntList->GetClientEntityFromHandle(obs);
		if (!Entity)
			return true;
		//if (Entity == pTarget)
		//return true;
		if (Entity->GetHealth() <= 0)
			return true;
		if (Entity->GetDormant())
			return true;

		return false;
	}

	void DrawRadar()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 oldPadding = style.WindowPadding;
		float oldAlpha = style.Colors[ImGuiCol_WindowBg].w;
		style.WindowPadding = ImVec2(0, 0);
		style.Colors[ImGuiCol_WindowBg].w = 1.f;
		if (ImGui::Begin(XorStr("Radar"), &Vars.Visuals.Radar.Enabled, ImVec2(200, 200), 0.4F, ImGuiWindowFlags_NoTitleBar |/*ImGuiWindowFlags_NoResize | */ImGuiWindowFlags_NoCollapse))
		{
			ImVec2 siz = ImGui::GetWindowSize();
			ImVec2 pos = ImGui::GetWindowPos();

			ImDrawList* DrawList = ImGui::GetWindowDrawList();

			DrawList->AddRect(ImVec2(pos.x - 6, pos.y - 6), ImVec2(pos.x + siz.x + 6, pos.y + siz.y + 6), Color::Black().GetU32(), 0.0F, -1, 1.5f);
			//_drawList->AddRect(ImVec2(pos.x - 2, pos.y - 2), ImVec2(pos.x + siz.x + 2, pos.y + siz.y + 2), Color::Black().GetU32(), 0.0F, -1, 1);

			//_drawList->AddRect(ImVec2(pos.x - 2, pos.y - 2), ImVec2(pos.x + siz.x + 2, pos.y + siz.y + 2), Color::Black().GetU32(), 0.0F, -1, 2);
			//_drawList->AddRect(ImVec2(pos.x - 2, pos.y - 2), ImVec2(pos.x + siz.x + 2, pos.y + siz.y + 2), Color::Silver().GetU32(), 0.0F, -1, 1.1f);

			ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
			windowDrawList->AddLine(ImVec2(pos.x + (siz.x / 2), pos.y + 0), ImVec2(pos.x + (siz.x / 2), pos.y + siz.y), Color::Black().GetU32(), 1.5f);
			windowDrawList->AddLine(ImVec2(pos.x + 0, pos.y + (siz.y / 2)), ImVec2(pos.x + siz.x, pos.y + (siz.y / 2)), Color::Black().GetU32(), 1.5f);

			// Rendering players

			if (I::Engine->IsInGame() && I::Engine->IsConnected())
			{
				//CPlayer* pLocalEntity = Interface.EntityList->GetClientEntity<CPlayer>(Interface.Engine->GetLocalPlayer());
				if (G::LocalPlayer)
				{
					Vector LocalPos = G::LocalPlayer->GetEyePosition();
					QAngle ang;
					I::Engine->GetViewAngles(ang);
					for (int i = 0; i < I::Engine->GetMaxClients(); i++) {
						CBaseEntity* pBaseEntity = I::ClientEntList->GetClientEntity(i);

						player_info_t pInfo;
						I::Engine->GetPlayerInfo(i, &pInfo);

						if (EntityIsInvalid(pBaseEntity))
							continue;

						CBaseEntity* observerTarget = I::ClientEntList->GetClientEntityFromHandle(G::LocalPlayer->GetObserverTargetHandle());

						bool bIsEnemy = (G::LocalPlayer->GetTeam() != pBaseEntity->GetTeam() || pBaseEntity == observerTarget || pBaseEntity == G::LocalPlayer) ? true : false ;

						if (Vars.Visuals.Radar.EnemyOnly && !bIsEnemy)
							continue;

						bool viewCheck = false;
						Vector2D EntityPos = RotatePoint(pBaseEntity->GetOrigin(), LocalPos, pos.x, pos.y, siz.x, siz.y, ang.y, Vars.Visuals.Radar.range, &viewCheck);

						//ImU32 clr = (bIsEnemy ? (isVisibled ? Color::LightGreen() : Color::Blue()) : Color::White()).GetU32();
						ImU32 clr = (bIsEnemy ? Color::Red() : Color::LightBlue()).GetU32();
						static bool drawname = true;

						if (pBaseEntity == observerTarget || pBaseEntity == G::LocalPlayer)
						{
							clr = Color::White().GetU32();
							drawname = false;
						}
						else
							drawname = true;

						int s = 2;

						windowDrawList->AddRect(ImVec2(EntityPos.x - s, EntityPos.y - s),
							ImVec2(EntityPos.x + s, EntityPos.y + s),
							clr);

						RECT TextSize = D::GetTextSize(F::ESP,pInfo.name);

						if (drawname && Vars.Visuals.Radar.Nicks)
							windowDrawList->AddText(ImVec2(EntityPos.x - (TextSize.left / 2), EntityPos.y - s), Color::White().GetU32(), pInfo.name);
					}
				}
			}
		}
		ImGui::End();
		style.WindowPadding = oldPadding;
		style.Colors[ImGuiCol_WindowBg].w = oldAlpha;
	}

	void DrawRageTab()
	{
		//ImGui::SameLine();
		//ImGui::GetCurrentWindow()->DC.CursorPos.y += 6;
		//int tabWidth = CalcTabWidth(4);

		/*if (DrawTab(XorStr("ESP"), tabWidth, tabHeight, visEspTab, false)) SelectVisualsSubTab(&visEspTab);
		if (DrawTab(XorStr("Chams"), tabWidth, tabHeight, visChamsTab, false)) SelectVisualsSubTab(&visChamsTab);
		if (DrawTab(XorStr("Hands"), tabWidth, tabHeight, visHandsTab, false)) SelectVisualsSubTab(&visHandsTab);
		if (DrawTab(XorStr("Misc##0"), tabWidth, tabHeight, visMiscTab, false)) SelectVisualsSubTab(&visMiscTab);*/
		//ImGui::NewLine();

		//ImVec2 siz = ImVec2(185, curHeight - ImGui::GetCursorPosY() - 40);
		//ImVec2 csize = ImVec2(siz.x - 28, 0);
		//ImVec2 asize = ImVec2(csize.x - 10, 0);

		static bool accuracy1tab = true;
		static bool accuracy2tab;

		static bool target1tab = true;
		static bool target2tab;

		static bool aa1tab = true;
		static bool aa2tab;

		ImGui::Columns(2, NULL, true, false);
		{
			//ImGui::Text(XorStr("-AimBot"));
			//ImGui::Separator();
			ImGui::Checkbox(XorStr("Ragebot Enabled"), &Vars.Ragebot.Enabled);
			ImGui::Checkbox(XorStr("On Key"), &Vars.Ragebot.Hold);
			if (Vars.Ragebot.Hold)
				ImGui::Combo(XorStr("Key"), &Vars.Ragebot.HoldKey, keyNames, IM_ARRAYSIZE(keyNames));
			ImGui::Checkbox(XorStr("Auto Fire"), &Vars.Ragebot.AutoFire);
			ImGui::Checkbox(XorStr("AutoPistol"), &Vars.Ragebot.AutoPistol);
			ImGui::SliderFloat(XorStr("FOV"), &Vars.Ragebot.FOV, 1.f, 180.f, "%.0f");
			//ImGui::Checkbox(XorStr("Aim Step"), &Vars.Ragebot.Aimstep);
			//ImGui::SliderFloat(XorStr("##AnglePer1Tick"), &Vars.Ragebot.AimstepAmount, 1.f, 180.f, "%.0f");
			//ImGui::Separator();
			ImGui::Text(XorStr("-Enable on VAC servers")); 
			ImGui::SameLine();
			//ImGui::Separator();
			ImGui::Checkbox(XorStr("Anti-Untrusted"), &Vars.Ragebot.UntrustedCheck);
		}
		ImGui::NextColumn();
		{
			if (DrawTab(XorStr("Accuracy"), ImVec2(100, 30), &accuracy1tab, false))
			{
				accuracy1tab = true;
				accuracy2tab = false;
			}
			ImGui::SameLine();
			if (DrawTab(XorStr("Accuracy Adjust"), ImVec2(100, 30), &accuracy2tab, false))
			{
				accuracy1tab = false;
				accuracy2tab = true;
			}

			if (accuracy1tab) {
				ImGui::Text(XorStr("Resolver"));
				ImGui::Combo(XorStr("##RESOLV"), &Vars.Ragebot.Antiaim.Resolver, XorStr("Disabled\0\rMM\0\rMM Delta\0\0"), -1);
				ImGui::Text(XorStr("Position Adjusment"));
				ImGui::Combo(XorStr("##POSADJUST"), &Vars.Ragebot.ResolverHelper, XorStr("No\0\rLow(Pitch Standalone)\0\rMedium(Pitch Extended)\0\rHigh(Accuracy Boost)\0\0"), -1);
				ImGui::Checkbox(XorStr("Auto Stop"), &Vars.Ragebot.AutoStop);
				ImGui::SameLine();
				
				ImGui::Checkbox(XorStr("Auto Crouch"), &Vars.Ragebot.AutoCrouch);
				ImGui::Checkbox(XorStr("Auto Scope"), &Vars.Ragebot.AutoScope);
			}

			if (accuracy2tab) {
				ImGui::Text(XorStr("RCS"));
				ImGui::Combo(XorStr("##RCSTYPE"), &Vars.Ragebot.RCS, XorStr("Off\0\rpRCS\0\rMaximum\0\0"), -1, ImVec2(85, 0));
				
				if (Vars.Ragebot.RCS == 2) {
					ImGui::SameLine();
					ImGui::SliderFloat(XorStr("##RCSForce"), &Vars.Ragebot.RCShow, 1.f, 100.f, "%.0f");
				}
				ImGui::Checkbox(XorStr("Hit Chance"), &Vars.Ragebot.HitChance);
				ImGui::SameLine();
				ImGui::SliderFloat(XorStr("##HCAmount"), &Vars.Ragebot.HitChanceAmt, 1.f, 100.f, "%.1f");
				ImGui::Checkbox(XorStr("Silent"), &Vars.Ragebot.Silent);
				ImGui::SameLine();
				ImGui::Checkbox(XorStr("pSilent"), &Vars.Ragebot.pSilent);
			}
		}
		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::Columns(2, NULL, true, true);
		{
			if (DrawTab(XorStr("Target"), ImVec2(100, 30), &target1tab, false))
			{
				target1tab = true;
				target2tab = false;
			}
			ImGui::SameLine();
			if (DrawTab(XorStr("Target Adjust"), ImVec2(100, 30), &target2tab, false))
			{
				target1tab = false;
				target2tab = true;
			}

			if (target1tab) {
				ImGui::Combo(XorStr("Hitbox"), &Vars.Ragebot.Hitbox, XorStr("PELVIS\0\r\0\r\0\rHIP\0\rLOWER SPINE\0\rMIDDLE SPINE\0\rUPPER SPINE\0\rNECK\0\rHEAD\0\0"), -1, ImVec2(100, 0));//XorStr("PELVIS\0\r\0\r\0\rHIP\0\rLOWER SPINE\0\rMIDDLE SPINE\0\rUPPER SPINE\0\rNECK\0\rHEAD\0\rNEAREST\0\0")
				ImGui::Checkbox(XorStr("Friendly Fire"), &Vars.Ragebot.FriendlyFire);
				ImGui::SameLine();
				ImGui::Checkbox(XorStr("Auto Wall"), &Vars.Ragebot.AutoWall);
				ImGui::Text(XorStr("Min Damage"));
				ImGui::SameLine();
				ImGui::SliderFloat(XorStr("##MinDamage"), &Vars.Ragebot.AutoWallDmg, 0.1f, 120.f, "%.1f");
				ImGui::Combo(XorStr("HitScan"), &Vars.Ragebot.HitScan, charenc("Off\0\rSelected\0\0"), -1, ImVec2(110.f, 20.f));
				ImGui::SameLine();
				ImGui::Checkbox(XorStr("MultiPoint"), &Vars.Ragebot.MultiPoints);
				ImGui::Text(XorStr("Scale"));
				ImGui::SameLine();
				ImGui::SliderFloat(XorStr("##MultiPointscale"), &Vars.Ragebot.PointScale, 0.f, 1.f, "%.01f");
			}
			if (target2tab) {

				ImGui::Text(XorStr("Disable AntiAim When"));

				ImGui::Checkbox(XorStr("Knife In Hands"), &Vars.Ragebot.Antiaim.knife_held);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XorStr("Stops your antiaim while you have your knife out."));
				ImGui::SameLine();
				ImGui::Checkbox(XorStr("No Enemy Visible"), &Vars.Ragebot.Antiaim.no_enemy);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XorStr("Stops your antiaim when there are no enemies visible."));
				ImGui::Text(XorStr("HitScan Bones"));
				ImGui::Selectable(XorStr("Head"), &Vars.Ragebot.bones[HITBOX_HEAD],0, ImVec2(93.f, 20.f));
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XorStr("HitScan head"));
				ImGui::SameLine();
				ImGui::Selectable(XorStr("Neck"), &Vars.Ragebot.bones[HITBOX_NECK], 0, ImVec2(93.f, 20.f));
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XorStr("HitScan neck"));
				ImGui::Selectable(XorStr("Pelvis"), &Vars.Ragebot.bones[HITBOX_PELVIS], 0, ImVec2(93.f, 20.f));
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XorStr("HitScan pelvis"));
				ImGui::SameLine();
				ImGui::Selectable(XorStr("Spine"), &Vars.Ragebot.bones[HITBOX_SPINE], 0, ImVec2(93.f, 20.f));
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XorStr("HitScan spine"));
				ImGui::Selectable(XorStr("Legs"), &Vars.Ragebot.bones[HITBOX_LEGS], 0, ImVec2(93.f, 20.f));
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XorStr("HitScan legs"));
				ImGui::SameLine();
				ImGui::Selectable(XorStr("Arms"), &Vars.Ragebot.bones[HITBOX_ARMS], 0, ImVec2(93.f, 20.f));
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(XorStr("HitScan arms"));
			}

		}
		ImGui::NextColumn();
		{

			if (DrawTab(XorStr("Anti-Aims"), ImVec2(100, 30), &aa1tab, false))
			{
				aa1tab = true;
				aa2tab = false;
			}
			ImGui::SameLine();
			if (DrawTab(XorStr("Anti-Aims adjust"), ImVec2(120, 30), &aa2tab, false))
			{
				aa1tab = false;
				aa2tab = true;
			}
			//ImGui::Text(XorStr("-AntiAim"));
			//ImGui::Separator();
			if (aa1tab)
			{
				ImGui::Checkbox(XorStr("Enabled"), &Vars.Ragebot.Antiaim.Enabled);
				ImGui::SameLine();
				ImGui::Checkbox(XorStr("Use Choked Yaw"), &Vars.Ragebot.Antiaim.FakeYaw);
				ImGui::Checkbox(XorStr("At Target"), &Vars.Ragebot.Antiaim.AtPlayer);
				//ImGui::SameLine();
				//ImGui::Checkbox(XorStr("Shuffle(TEST)"), &Vars.Ragebot.Antiaim.Shuffle);
				//ImGui::SliderInt(XorStr("##SpinSpeed"), &Vars.Ragebot.Antiaim.Zeta, 1, 4);
				ImGui::Combo(XorStr("Pitch"), &Vars.Ragebot.Antiaim.Pitch, charenc("Unselected\0\rUP\0\rDOWN\0\rJITTER\0\rZERO\0\rEmotion\0\rDOWN JITTER\0\rDOWN2\0\0"), -1, ImVec2(110.f, 20.f));
				ImGui::Combo(XorStr("Yaw"), &Vars.Ragebot.Antiaim.YawReal, charenc("Unselected\0\rSPIN\0\rDestroy LBY\0\rSIDEWAYS1\0\rSIDEWAYS2\0\rBACKWARDS\0\rLEFT\0\rRIGHT\0\rZERO\0\rFAKESPIN\0\rLowerBody Based\0\rLag FAKESPIN\0\rMEMESPIN\0\rFAKE 3\0\0"), -1, ImVec2(110.f, 20.f));
				ImGui::Text(XorStr("Yaw Choke"));
				ImGui::Combo(XorStr("##Choke"), &Vars.Ragebot.Antiaim.YawFake, charenc("Unselected\0\rSPIN\0\rJITTER\0\rSIDEWAYS1\0\rSIDEWAYS2\0\rBACKWARDS\0\rLEFT\0\rRIGHT\0\rZERO\0\rFAKESPIN\0\rLowerBody Based\0\rLag FAKESPIN\0\rMEMESPIN\0\rFAKE 3\0\0"), -1, ImVec2(110.f, 20.f));
			}
			if (aa2tab)
			{
				/*if (ImGui::TreeNode("Spin Adjust")) {
					ImGui::Text(XorStr("Spin Speed")); ImGui::SameLine();
					ImGui::SliderInt(XorStr("##SpinSpeed"), &Vars.Ragebot.Antiaim.spinspeed, 1, 1000);
					ImGui::Text(XorStr("Speed on Fake")); ImGui::SameLine();
					ImGui::SliderInt(XorStr("##SpinSpeedFake"), &Vars.Ragebot.Antiaim.spinspeedz, 1, 1000);
				}*/

				bool unt = Vars.Ragebot.UntrustedCheck;
				//ImGui::Text(XorStr("Custom AA Adjustment"));
				ImGui::Checkbox(XorStr("Custom Pitch"), &Vars.Ragebot.Antiaim.PitchCustom); ImGui::SameLine(); ImGui::Checkbox(XorStr("Custom Yaw"), &Vars.Ragebot.Antiaim.YawCustom);
				ImGui::Text(XorStr("Real Pitch")); ImGui::SameLine();
				ImGui::SliderFloat(XorStr("##REALPITCH"), &Vars.Ragebot.Antiaim.PCReal, unt ? -89 : -180, unt ? 89 : 180);
				ImGui::Text(XorStr("Fake Pitch")); ImGui::SameLine();
				ImGui::SliderFloat(XorStr("##FAKEPITCH"), &Vars.Ragebot.Antiaim.PCFake, unt ? -89 : -180, unt ? 89 : 180);
				ImGui::Text(XorStr("Real Yaw")); ImGui::SameLine();
				ImGui::SliderFloat(XorStr("##RealYaw"), &Vars.Ragebot.Antiaim.YCReal, unt ? -179 : -360, unt ? 179 : 360);
				ImGui::Text(XorStr("Fake Yaw")); ImGui::SameLine();
				ImGui::SliderFloat(XorStr("##FakeYaw"), &Vars.Ragebot.Antiaim.YCFake, unt ? -179 : -360, unt ? 179 : 360);
			}
			//ImGui::Separator();
			/*ImGui::Text(XorStr("-AntiAim Adjusment"));
			//ImGui::Separator();
			ImGui::Text(XorStr("Spin Speed"));
			ImGui::SliderInt(XorStr("##SpinSpeed"), &Vars.Ragebot.Antiaim.spinspeed, 1, 1000);
			ImGui::Text(XorStr("Speed on Fake"));
			ImGui::SliderInt(XorStr("##SpinSpeedFake"), &Vars.Ragebot.Antiaim.spinspeedz, 1, 1000);
			bool unt = Vars.Ragebot.UntrustedCheck;
			/*ImGui::Separator();
			ImGui::Text(XorStr("-Custom AntiAim"));
			ImGui::Separator();
			if (!unt)
				ImGui::Text(XorStr("Enable AntiUntrusted to use custom AA"));
			else
			{
				ImGui::Checkbox(XorStr("Custom Pitch"), &Vars.Ragebot.Antiaim.PitchCustom);
				ImGui::Text(XorStr("Real Pitch"));
				ImGui::SliderFloat(XorStr("##REALPITCH"), &Vars.Ragebot.Antiaim.PCReal, unt ? -89 : -180, unt ? 89 : 180);
				ImGui::Text(XorStr("Fake Pitch"));
				ImGui::SliderFloat(XorStr("##FAKEPITCH"), &Vars.Ragebot.Antiaim.PCFake, unt ? -89 : -180, unt ? 89 : 180);
				ImGui::Checkbox(XorStr("Custom Yaw"), &Vars.Ragebot.Antiaim.YawCustom);
				ImGui::Text(XorStr("Real Yaw"));
				ImGui::SliderFloat(XorStr("##RealYaw"), &Vars.Ragebot.Antiaim.YCReal, unt ? -179 : -360, unt ? 179 : 360);
				ImGui::Text(XorStr("Fake Yaw"));
				ImGui::SliderFloat(XorStr("##FakeYaw"), &Vars.Ragebot.Antiaim.YCFake, unt ? -179 : -360, unt ? 179 : 360);
			}*/
		}
		ImGui::Columns(1);
	}

	void DrawVisualsTab()
	{
		//ImGui::SameLine();
		//ImGui::GetCurrentWindow()->DC.CursorPos.y += 6;
		//int tabWidth = CalcTabWidth(4);

		/*if (DrawTab(XorStr("ESP"), tabWidth, tabHeight, visEspTab, false)) SelectVisualsSubTab(&visEspTab);
		if (DrawTab(XorStr("Chams"), tabWidth, tabHeight, visChamsTab, false)) SelectVisualsSubTab(&visChamsTab);
		if (DrawTab(XorStr("Hands"), tabWidth, tabHeight, visHandsTab, false)) SelectVisualsSubTab(&visHandsTab);
		if (DrawTab(XorStr("Misc##0"), tabWidth, tabHeight, visMiscTab, false)) SelectVisualsSubTab(&visMiscTab);*/
		//ImGui::NewLine();

		//ImVec2 siz = ImVec2(185, curHeight - ImGui::GetCursorPosY() - 40);
		//ImVec2 csize = ImVec2(siz.x - 28, 0);
		//ImVec2 asize = ImVec2(csize.x - 10, 0);

		static bool visuals1tab = true;
		static bool visuals2tab;

		static bool chams1tab = true;
		static bool chams2tab;

		static bool misc1tab = true;
		static bool misc2tab;

		ImGui::Columns(2, NULL, true, true);
		{
			ImGui::Checkbox(XorStr("Enable Visuals"), &Vars.Visuals.Enabled);
			ImGui::Checkbox(XorStr("2D Box"), &Vars.Visuals.Box);
			ImGui::Checkbox(XorStr("3D Box"), &Vars.Visuals.esp3d);
			ImGui::Checkbox(XorStr("Skeleton"), &Vars.Visuals.Skeleton);
			ImGui::Checkbox(XorStr("Bullet Trace"), &Vars.Visuals.BulletTrace);
			ImGui::Checkbox(XorStr("Anti-Screenshot"), &Vars.Visuals.AntiSS);
		}
		ImGui::NextColumn();
		{
			if (DrawTab(XorStr("Info"), ImVec2(100, 30), &visuals1tab, false))
			{
				visuals1tab = true;
				visuals2tab = false;
			}
			ImGui::SameLine();
			if (DrawTab(XorStr("Filters"), ImVec2(100, 30), &visuals2tab, false))
			{
				visuals1tab = false;
				visuals2tab = true;
			}
			if (visuals1tab)
			{
				ImGui::Selectable(XorStr(" Show Name"), &Vars.Visuals.InfoName);
				ImGui::Selectable(XorStr(" Show Health"), &Vars.Visuals.InfoHealth);
				ImGui::Selectable(XorStr(" Show Weapon"), &Vars.Visuals.InfoWeapon);
				ImGui::Selectable(XorStr(" Show If Flashed"), &Vars.Visuals.InfoFlashed);
			}
			if (visuals2tab) {
				ImGui::Selectable(XorStr(" Show Enemies"), &Vars.Visuals.Filter.Enemies);
				ImGui::Selectable(XorStr(" Show Teammates"), &Vars.Visuals.Filter.Friendlies);
				ImGui::Selectable(XorStr(" Show Weapons"), &Vars.Visuals.Filter.Weapons);
				ImGui::Selectable(XorStr(" Show Decoy"), &Vars.Visuals.Filter.Decoy);
				ImGui::Selectable(XorStr(" Show C4"), &Vars.Visuals.Filter.C4);
			}
		} 
		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::Columns(2, NULL, true, true);
		{
			ImGui::Checkbox(XorStr("Chams Enable"), &Vars.Visuals.Chams.Enabled);
			ImGui::SameLine();
			ImGui::Combo(XorStr("Mode"), &Vars.Visuals.Chams.Mode, XorStr("Flat\0\rTextured\0\0"), -1,ImVec2(70,0));
			ImGui::Checkbox(XorStr("XQZ"), &Vars.Visuals.Chams.XQZ);
			ImGui::Checkbox(XorStr("Hands"), &Vars.Visuals.Chams.hands);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("Weapon"), &Vars.Visuals.ChamsWeapon);
			ImGui::Checkbox(XorStr("Rainbow Hands"), &Vars.Visuals.Chams.RainbowHands);
			ImGui::Text(XorStr("Speed"));
			ImGui::SameLine();
			ImGui::SliderFloat(XorStr("##CHMSSpeed"), &Vars.Visuals.Chams.RainbowTime, 1.f, 150.f, "%.0f");
	
			ImGui::Text(XorStr("Removals"));
			ImGui::Checkbox(XorStr("No Vis Recoil"), &Vars.Visuals.RemovalsVisualRecoil);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("No Flash"), &Vars.Visuals.RemovalsFlash);
			ImGui::Checkbox(XorStr("No Scope"), &Vars.Visuals.NoScope);
		}
		ImGui::NextColumn();
		{
			if (DrawTab(XorStr("Misc"), ImVec2(100, 30), &misc1tab, false))
			{
				misc1tab = true;
				misc2tab = false;
			}
			ImGui::SameLine();
			if (DrawTab(XorStr("Crosshair"), ImVec2(100, 30), &misc2tab, false))
			{
				misc1tab = false;
				misc2tab = true;
			}

			if (misc1tab) {
				ImGui::Text(XorStr("Override Fov"));
				ImGui::SameLine();
				ImGui::SliderInt(XorStr("##OverrideFov"), &Vars.Misc.fov, -70, 70);
				ImGui::Text(XorStr("ViewModel Fov"));
				ImGui::SameLine();
				ImGui::SliderInt(XorStr("##ViewFov"), &Vars.Visuals.ViewmodelFov, -70, 70);
				ImGui::Checkbox(XorStr("SpectatorList"), &Vars.Visuals.SpectatorList);
				ImGui::Combo(XorStr("Vector to Enemies"), &Vars.Visuals.Line, XorStr("Off\0\rCentered\0\rDown\0\0"), -1, ImVec2(70, 0));
				ImGui::Checkbox(XorStr("ThirdPerson"), &Vars.Visuals.thirdperson.enabled);
				ImGui::Text(XorStr("Distance"));
				ImGui::SameLine();
				ImGui::SliderFloat(XorStr("##THRDPERSDIST"), &Vars.Visuals.thirdperson.distance, 30.f, 200.f, "%.0f");
			}
			if (misc2tab) {
				ImGui::Checkbox(XorStr("Crosshair Enabled"), &Vars.Visuals.CrosshairOn);
				ImGui::Checkbox(XorStr("Show Spread"), &Vars.Visuals.CrosshairSpread);
				ImGui::Checkbox(XorStr("Show FOV"), &Vars.Visuals.CrosshairFOV);
				ImGui::Checkbox(XorStr("Show Recoil"), &Vars.Visuals.CrosshairType);
				ImGui::Checkbox(XorStr("Rainbow Color"), &Vars.Visuals.CrosshairRainbow);
				ImGui::Text(XorStr("Crosshair Style"));
				ImGui::SameLine();
				ImGui::Combo(XorStr("##CRSHRStyle"), &Vars.Visuals.CrosshairStyle, XorStr("Plus\0\rCircle\0\rBox\0\0"), -1, ImVec2(70, 0));
			}
		} 
		ImGui::Columns(1);
	}

	void DrawRadarTab()
	{
		ImGui::Checkbox(XorStr("Enable Radar"), &Vars.Visuals.Radar.Enabled);
		ImGui::SliderInt(XorStr("Radar Range"), &Vars.Visuals.Radar.range, 1, 100);
		ImGui::Checkbox(XorStr("Only Enemy"), &Vars.Visuals.Radar.EnemyOnly);
		ImGui::Checkbox(XorStr("Show Nicknames"), &Vars.Visuals.Radar.Nicks);

		if (ImGui::ColorEdit3(XorStr("Body clr"), Vars.g_fBColor) ||
			ImGui::ColorEdit3(XorStr("Main clr"), Vars.g_fMColor) ||
			ImGui::ColorEdit3(XorStr("Text clr"), Vars.g_fTColor)) color();

		ImGui::ColorEdit3(XorStr("Chams CT Visible"), Vars.g_iChamsCTV);
		ImGui::ColorEdit3(XorStr("Chams CT Hidden"), Vars.g_iChamsCTH);
		ImGui::ColorEdit3(XorStr("Chams T Visible"), Vars.g_iChamsTV);
		ImGui::ColorEdit3(XorStr("Chams T Hidden"), Vars.g_iChamsTH);

		//SkinsTab();
	}

	void DrawChangerTab()
	{
		/*ImVec2 lPos = ImGui::GetCursorPos();
		if (ImGui::Checkbox(XorStr("Skin Changer##0"), &Options::SkinChanger::EnabledSkin)) U::FullUpdate();

		if (ImGui::Checkbox(XorStr("Knife Changer"), &Options::SkinChanger::EnabledKnife)) U::FullUpdate();
		if (Options::SkinChanger::EnabledKnife)
			if (ImGui::Combo(XorStr("##0"), &Options::SkinChanger::Knife, "Bayonet\0Flip\0Gut\0Karambit\0M9Bayonet\0Huntsman\0Falchion\0Bowie\0Butterfly\0Daggers\0\0", -1, ImVec2(130, 0)))
				U::FullUpdate();

		if (ImGui::Checkbox(XorStr("Glove Changer"), &Options::SkinChanger::EnabledGlove)) U::FullUpdate();
		if (Options::SkinChanger::EnabledGlove)
		{
			if (ImGui::Combo(XorStr("##1"), &Options::SkinChanger::Glove, "Bloodhound\0Sport\0Driver\0Wraps\0Moto\0Specialist\0\0", -1, ImVec2(130, 0)))
				U::FullUpdate();

			const char* gstr;
			if (Options::SkinChanger::Glove == 0)
			{
				gstr = "Charred\0\rSnakebite\0\rBronzed\0\rGuerilla\0\0";
			}
			else if (Options::SkinChanger::Glove == 1)
			{
				gstr = "Hedge Maze\0\rPandoras Box\0\rSuperconductor\0\rArid\0\0";
			}
			else if (Options::SkinChanger::Glove == 2)
			{
				gstr = "Lunar Weave\0\rConvoy\0\rCrimson Weave\0\rDiamondback\0\0";
			}
			else if (Options::SkinChanger::Glove == 3)
			{
				gstr = "Leather\0\rSpruce DDPAT\0\rSlaughter\0\rBadlands\0\0";
			}
			else if (Options::SkinChanger::Glove == 4)
			{
				gstr = "Eclipse\0\rSpearmint\0\rBoom!\0\rCool Mint\0\0";
			}
			else if (Options::SkinChanger::Glove == 5)
			{
				gstr = "Forest DDPAT\0\rCrimson Kimono\0\rEmerald Web\0\rFoundation\0\0";
			}
			else
				gstr = "";

			if (ImGui::Combo(XorStr("##2"), &Options::SkinChanger::GloveSkin, gstr, -1, ImVec2(130, 0)))
				U::FullUpdate();
		}



		//int cw = /*U::SafeWeaponID(); 7;
		int cw = U::SafeWeaponID();
		if (cw == 0)
			return;
		if (U::IsWeaponDefaultKnife(cw))
			return;

		ImGui::SetCursorPos(ImVec2(160, lPos.y));
		ImGui::BeginChild(XorStr("AimChild"), ImVec2(0, 0), true);
		{
			ImGui::Checkbox(XorStr("Enabled"), &weapons[cw].ChangerEnabled);

			std::string skinName = U::GetWeaponNameById(cw);
			if (skinName.compare("") == 0)
				ImGui::InputInt(XorStr("Skin"), &weapons[cw].ChangerSkin);
			else
			{
				std::string skinStr = "";
				int curItem = -1;
				int s = 0;
				for (auto skin : G::weaponSkins[skinName])
				{
					int pk = G::skinMap[skin].paintkit;
					if (pk == weapons[cw].ChangerSkin)
						curItem = s;

					skinStr += G::skinNames[G::skinMap[skin].tagName].c_str();
					skinStr.push_back('\0');
					s++;
				}
				skinStr.push_back('\0');
				if (ImGui::Combo(XorStr("Skin"), &curItem, skinStr.c_str()))
				{
					int pk = 0;
					int c = 0;
					for (auto skin : G::weaponSkins[skinName])
					{
						if (curItem == c)
						{
							pk = G::skinMap[skin].paintkit;
							break;
						}

						c++;
					}
					weapons[cw].ChangerSkin = pk;
					if (weapons[cw].ChangerEnabled) U::FullUpdate();
				}
			}
			ImGui::InputText(XorStr("Name"), weapons[cw].ChangerName, 32);
			ImGui::InputInt(XorStr("StatTrak"), &weapons[cw].ChangerStatTrak);
			ImGui::InputInt(XorStr("Seed"), &weapons[cw].ChangerSeed);
			ImGui::SliderFloat(XorStr("Wear"), &weapons[cw].ChangerWear, 0, 1);

			ImGui::NewLine();
			if (ImGui::Button(XorStr("Apply")))
				U::FullUpdate();
		}
		ImGui::EndChild();*/
	}

	void DrawMiscTab()
	{
		ImVec2 siz = ImVec2(303, 174);
		ImVec2 csize = ImVec2(siz.x - 28, 0);
		ImVec2 asize = ImVec2(csize.x - 10, 0);

		ImGui::PushItemWidth(150);

		ImGui::Columns(2, NULL, true, true);
		{
			ImGui::Text(XorStr("Movement"));
			ImGui::Combo(XorStr("FakeLag"), &Vars.Misc.FakeLag, XorStr("Off\0\rFactor\0\rSwitch\0\rAdaptive\0\rAdaptive 2\0\0"), -1);
			ImGui::SliderInt(XorStr("##FAKELAG"), &Vars.Misc.FakeLags, 0, 16);
			ImGui::Checkbox(XorStr("Bunny Hop"), &Vars.Misc.Bhop);
			ImGui::SameLine();
			ImGui::Combo(XorStr("##STRAFER"), &Vars.Misc.AutoStrafe, XorStr("Off\0\rStrafe Helper\0\rFull Strafer\0\0"), -1);
			ImGui::Checkbox(XorStr("Air Stuck"), &Vars.Misc.AirStuck);
			if (Vars.Misc.AirStuck)
			{
				ImGui::SameLine();
				ImGui::Combo(XorStr("Key"), &Vars.Misc.AirStuckKey, keyNames, IM_ARRAYSIZE(keyNames));
			}
			ImGui::Text(XorStr("Miscellaneous"));
			ImGui::Checkbox(XorStr("Ranks Reveal"), &Vars.Misc.Ranks); ImGui::SameLine();
			ImGui::Checkbox(XorStr("Auto Accept"), &Vars.Misc.AutoAccept);
			ImGui::Checkbox(XorStr("Watermark"), &Vars.Misc.Watermark);
		}
		ImGui::NextColumn();
		{
			ImGui::Text(XorStr("Chat Spam"));
			//ImGui::Separator();
			ImGui::Text(XorStr("Message"));
			ImGui::SameLine();
			ImGui::InputText(XorStr("##SpamMessage"), Vars.Misc.ChatSpamMode, 128);
			ImGui::Text(XorStr("Delay"));
			ImGui::SameLine();
			ImGui::SliderFloat(XorStr("##Spam Delay"), &Vars.Misc.ChatSpamDelay, 0.1f, 10.f);
			ImGui::Checkbox(XorStr("ChatSpam"), &Vars.Misc.ChatSpam);
			//ImGui::Separator();
			//ImGui::Text(XorStr("Location Spam"));
			//ImGui::Separator();
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("Location Spam"), &Vars.Misc.LocSpam);
			ImGui::Checkbox(XorStr("LocSpam Enemies only"), &Vars.Misc.LocSpamEnemies);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("LocSpam teamchat only"), &Vars.Misc.LocSpamChat);
			ImGui::Text(XorStr("Event Spam"));
			ImGui::Checkbox(XorStr("On Headshot"), &Vars.Misc.TapSay);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("On Kill"), &Vars.Misc.SoundShit);
			ImGui::Checkbox(XorStr("Bomb Info"), &Vars.Misc.bombinfo);
			//ImGui::Separator();
		}
		ImGui::Columns(1);

		ImGui::Separator();
		/*ImGui::Columns(2, NULL, true, true);
		{
			ImGui::Text(XorStr("Clantag"));
			//ImGui::Separator();
			ImGui::Checkbox(XorStr("Enabled"), &Vars.Misc.ClantagChanger.enabled);
			ImGui::Text(XorStr("Text"));
			static bool init_clan = false;
			if (!init_clan)
			{
				sprintf(Vars.Misc.ClantagChanger.value, "MorphEngine.cc");
				init_clan = true;
			}

			if (ImGui::InputText(XorStr("##CLANTAGTEXT"), Vars.Misc.ClantagChanger.value, 30))
				E::ClantagChanger->UpdateClanTagCallback();
			ImGui::Text(XorStr("Animation Type"));
			if (ImGui::Combo(XorStr("##ANIM"), &Vars.Misc.ClantagChanger.type, XorStr("Static\0\rMarquee\0\rWords\0\rLetters\0\rTime\0\0"), -1))
				E::ClantagChanger->UpdateClanTagCallback();
			ImGui::Text(XorStr("Animation Speed"));
			if (ImGui::SliderInt(XorStr("##ANIMSPEED"), &Vars.Misc.ClantagChanger.animation_speed, 0, 2000))
				E::ClantagChanger->UpdateClanTagCallback();

			//ImGui::Separator();
			ImGui::Text(XorStr("Nickname"));
			//ImGui::Separator();
			static char nickname[128] = "";
			static char nickseparator[128] = "";
			static bool init_nick = false;
			ImGui::Text(XorStr("Text"));
			ImGui::InputText(XorStr("##NICTEXT"), nickname, 127);
			ImGui::Text(XorStr("Separator"));
			ImGui::InputText(XorStr("##NICTOR"), nickseparator, 127);
			if (ImGui::Button(XorStr("Set Nickname"), ImVec2(-1, 0)))
			{
				std::string ctWithEscapesProcessed = std::string(nickname);
				U::StdReplaceStr(ctWithEscapesProcessed, "\\n", "\n");

				E::Misc->ChangeName(ctWithEscapesProcessed.c_str());
			}

			if (!init_nick)
			{
				sprintf(nickname, "MorphEngine.cc");
				sprintf(nickseparator, "\\n");
				init_nick = true;
			}

			ImGui::Text(XorStr("SeparatorType"));
			ImGui::Combo(XorStr("##SeparatorSType"), &Vars.Misc.NameChangerseparator_animation, XorStr("Start\0\rEnd\0\rAnimation\0\0"), -1);

			if (ImGui::Button(XorStr("NickSpamer"), ImVec2(-1, 0)))
			{
				E::NameChanger->nickname = nickname;
				E::NameChanger->separator = nickseparator;
				Vars.Misc.NameChangerenabled = true;
				//Vars.Misc.NameChanger.last_blank = true;
			}*/
			//}
		ImGui::Columns(2, NULL, true, true);
		{
			//ImGui::Text(XorStr("Configs"));
			//ImGui::Separator();
			GetConfigMassive();
			ImGui::Text(XorStr("Config Name"));
			//configs
			static int selectedcfg = 0;
			static std::string cfgname = "default";
			if (ImGui::Combo(XorStr("Parsed Cfgs"), &selectedcfg, [](void* data, int idx, const char** out_text)
			{
				*out_text = configs[idx].c_str();
				return true;
			}, nullptr, configs.size(), 10))
			{
				cfgname = configs[selectedcfg];
				cfgname.erase(cfgname.length() - 4, 4);
				strcpy(Vars.Misc.configname, cfgname.c_str());
			}

			ImGui::InputText(XorStr("Current Cfg"), Vars.Misc.configname, 128);
			if (ImGui::Button(XorStr("Save Config"), ImVec2(93.f, 20.f))) Config->Save();
			ImGui::SameLine();
			if (ImGui::Button(XorStr("Load Config"), ImVec2(93.f, 20.f))) {
				Config->Load(); color();
			}
			ImGui::Text("");
			ImGui::Text("");
			ImGui::Text("");
			ImGui::Text("");
			ImGui::Text("");
			//ImGui::SameLine();
		}
		ImGui::NextColumn();
		{
			ImGui::Text(XorStr("Other"));
			//ImGui::Separator();
			if (ImGui::Button(XorStr("Unload"), ImVec2(93.f, 20.f))) {
				E::Misc->Panic();
			}
			if (ImGui::Button(XorStr("FontFix"), ImVec2(93.f, 20.f)))
				D::SetupFonts();
		}
		ImGui::Columns(1);
	}

	void Render()
	{
		//AndeltaTime = GetAnDeltaTime();

		ImGui::GetIO().MouseDrawCursor = Vars.Menu.Opened;

		//if (Options::Misc::SpectatorList) DrawSpectatorList();
		if (Vars.Menu.Opened)
		{
			E::Visuals->PlayerList();
			/*float deltaSize = 2 * AndeltaTime;
			if (curWidth < windowWidth)
				curWidth += min(windowWidth - curWidth, deltaSize);
			else if (curHeight < windowHeight)
				curHeight += min(windowHeight - curHeight, deltaSize);

			int iScreenWidth, iScreenHeight;
			I::Engine->GetScreenSize(iScreenWidth, iScreenHeight);

			curX = (iScreenWidth / 2) - (curWidth / 2);
			curY = (iScreenHeight / 2) - (windowHeight / 2);*/

			int pX, pY;
			I::InputSystem->GetCursorPosition(&pX, &pY);
			ImGuiIO& io = ImGui::GetIO();
			io.MousePos.x = (float)(pX);
			io.MousePos.y = (float)(pY);
			//VMProtectBeginMutation("MENU_MUT");
			#define BUILDSTAMP ( __DATE__ " / " __TIME__ )
			char nameChar[64];
			sprintf(nameChar, "MorphEngine.cc | Last Build - %s", BUILDSTAMP);

			if (ImGui::Begin(XorStr(nameChar), &Vars.Menu.Opened, ImVec2(windowWidth, windowHeight), 1.0F, /*(notFull ? ImGuiWindowFlags_NoMove : 0) |*/ ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse))
			{
				//if (notFull) return;
				ImVec2 lPos = ImGui::GetCursorPos();
				ImGuiContext* io = ImGui::GetCurrentContext();
				ImGuiStyle& style = ImGui::GetStyle();

				DrawTab(XorStr("RAGE"), ImVec2(145.f, 50.f), &rageTab);
				DrawTab(XorStr("LEGIT"), ImVec2(145.f, 50.f), &legitTab);
				DrawTab(XorStr("VISUALS"), ImVec2(145.f, 50.f), &visualsTab);
				DrawTab(XorStr("RADAR"), ImVec2(145.f, 50.f), &radarTab);
				DrawTab(XorStr("SKINS"), ImVec2(145.f, 50.f), &configTab);
				DrawTab(XorStr("MISC"), ImVec2(145.f, 50.f), &miscTab);

				ImGui::SetCursorPos(ImVec2(160, lPos.y));
				ImGui::BeginChild(XorStr("##Menu-Child"), ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				{
					if (legitTab) DrawLegitTab();
					if (visualsTab) DrawVisualsTab();
					if (rageTab) DrawRageTab();
					if (radarTab) DrawRadarTab();
					if (configTab) DrawConfigTab();
					if (miscTab) DrawMiscTab();
				}
				ImGui::EndChild();
				//VMProtectEnd();
			}
			ImGui::End();
		}
	}

void ColorTab()
{
	//ImGui::Separator();
	if (ImGui::ColorEdit3(XorStr("Body clr"), Vars.g_fBColor) ||
	ImGui::ColorEdit3(XorStr("Main clr"), Vars.g_fMColor) ||
	ImGui::ColorEdit3(XorStr("Text clr"), Vars.g_fTColor)) color();

	ImGui::ColorEdit3(XorStr("Chams CT Visible"), Vars.g_iChamsCTV);
	ImGui::ColorEdit3(XorStr("Chams CT Hidden"), Vars.g_iChamsCTH);
	ImGui::ColorEdit3(XorStr("Chams T Visible"), Vars.g_iChamsTV);
	ImGui::ColorEdit3(XorStr("Chams T Hidden"), Vars.g_iChamsTH);
}

void ConfigTab()
{
	ImGui::InputText(XorStr("Config Name"), Vars.Misc.configname, 128);
	ImGui::PushItemWidth(190);
	ImGui::Columns(2, XorStr("##config-settings"), false);
	if (ImGui::Button(XorStr("Save Config"), ImVec2(93.f, 20.f))) Config->Save();
	ImGui::NextColumn();
	if (ImGui::Button(XorStr("Load Config"), ImVec2(93.f, 20.f))) {
		Config->Load(); color();
	}
	ImGui::Columns(1);
	if (ImGui::Button(XorStr("FontFix"), ImVec2(93.f, 20.f)))
		//D::SetupFonts();
		E::Misc->GameResources();
	ImGui::SameLine();
	if (ImGui::Button(XorStr("Unload"), ImVec2(93.f, 20.f))) {
		E::Misc->Panic();
	}

	//ImGui::SliderInt(XorStr("StatTrak"), &Vars.Misc.alpha, 0, 99999);
}


 
EndSceneFn oEndScene;
long __stdcall Hooks::EndScene(IDirect3DDevice9* pDevice)
{
	if (!G::d3dinit)
		GUI_Init(pDevice);

	H::D3D9->ReHook();

	ImGui::GetIO().MouseDrawCursor = Vars.Menu.Opened;

	ImGui_ImplDX9_NewFrame(); 

	if (Vars.Visuals.SpectatorList)
		E::Visuals->SpecList();

	if (Vars.Visuals.Radar.Enabled && I::Engine->IsConnected())
		DrawRadar();


	Render();
	/*
	if (Vars.Menu.Opened)
	{
		//E::Visuals->PlayerList();

		int pX, pY;
		I::InputSystem->GetCursorPosition(&pX, &pY);
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos.x = (float)(pX);
		io.MousePos.y = (float)(pY);

		static int page = 0;
		ImGui::SetNextWindowSize(ImVec2(960, 645), ImGuiSetCond_FirstUseEver);

		ImVec2 windowSize = ImGui::GetWindowSize();
		int widthz = windowSize.x - 0;


		if (ImGui::Begin("MorphEngine", &Vars.Menu.Opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders))
		{
			const char* tabs[] = {
				"RageBot",
				"LegitBot",
				"Visuals",
				"Misc",
				"Config",
				//"Skins",
				"Colors"
			};
			int tabs_size = sizeof(tabs) / sizeof(tabs[0]);

			for (int i = 0; i < tabs_size; i++)
			{
				ImVec2 windowSize = ImGui::GetWindowSize();
				int width = windowSize.x / tabs_size - 9;

				int distance;
				if (i == page)
					distance = 0;
				else
					distance = i > page ? i - page : page - i;

				if (ImGui::Button(tabs[i], ImVec2(width, 0)))
					page = i;

				if (i < tabs_size - 1)
					ImGui::SameLine();
			}

			ImGui::Separator();

			switch (page)
			{
			case 0:
				RageTab();
				break;
			case 1:
				LegitTab();
				break;
			case 2:
				VisualsTab();
				break;
			case 3:
				MiscTab();
				break;
			case 4:
				ConfigTab();
				break;
			case 5:
				SkinsTab();
				break;
			case 5:
				ColorTab();
				break;
			}

			ImGui::End();
		}
	}
	ImGui::Render();
	*/
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

	color();

	G::d3dinit = true;
}

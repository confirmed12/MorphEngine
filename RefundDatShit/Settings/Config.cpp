#include "../Cheat.h"

inline char* strcats(char* s1, char* s2)
{
	std::string ss1(s1);
	std::string ss2(s2);
	std::string Buff = ss1 + ss2;
	char *Buffer = _strdup(Buff.c_str());
	return Buffer;
}

const char* GetWeaponName(int iWeaponID)
{
	switch (iWeaponID)
	{
	default:
		return "none";
	case WEAPON_DEAGLE:
		return "deagle";
	case WEAPON_ELITE:
		return "duals";
	case WEAPON_FIVESEVEN:
		return "five7";
	case WEAPON_GLOCK:
		return "glock";
	case WEAPON_AK47:
		return "ak47";
	case WEAPON_AUG:
		return "aug";
	case WEAPON_AWP:
		return "awp";
	case WEAPON_FAMAS:
		return "famas";
	case WEAPON_G3SG1:
		return "g3sg1";
	case WEAPON_GALILAR:
		return "galil";
	case WEAPON_M249:
		return "m249";
	case WEAPON_M4A1:
		return "m4a4";
	case WEAPON_MAC10:
		return "mac10";
	case WEAPON_P90:
		return "p90";
	case WEAPON_UMP45:
		return "ump45";
	case WEAPON_XM1014:
		return "xm1014";
	case WEAPON_BIZON:
		return "bizon";
	case WEAPON_MAG7:
		return "mag7";
	case WEAPON_NEGEV:
		return "negev";
	case WEAPON_SAWEDOFF:
		return "sawedoff";
	case WEAPON_TEC9:
		return "tec9";
	case WEAPON_TASER:
		return "taser";
	case WEAPON_USP_SILENCER:
		return "usp-s";
	case WEAPON_MP7:
		return "mp7";
	case WEAPON_MP9:
		return "mp9";
	case WEAPON_NOVA:
		return "nova";
	case WEAPON_P250:
		return "p250";
	case WEAPON_SCAR20:
		return "scar20";
	case WEAPON_SG556:
		return "sg556";
	case WEAPON_SSG08:
		return "ssg08";
	case WEAPON_M4A1_SILENCER:
		return "m4a1-s";
	case WEAPON_HKP2000:
		return "p2000";
	case WEAPON_CZ75A:
		return "cz75";
	case WEAPON_REVOLVER:
		return "revolver";
	case WEAPON_KNIFE_BAYONET:
		return "bayonet";
	case WEAPON_KNIFE_BUTTERFLY:
		return "butterfly";
	case WEAPON_KNIFE_FALCHION:
		return "falshion";
	case WEAPON_KNIFE_FLIP:
		return "flip";
	case WEAPON_KNIFE_GUT:
		return "gut";
	case WEAPON_KNIFE_KARAMBIT:
		return "karambit";
	case WEAPON_KNIFE_M9_BAYONET:
		return "m9";
	case WEAPON_KNIFE_TACTICAL:
		return "huntsman";
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
		return "bowie";
	case WEAPON_KNIFE_PUSH:
		return "daggers";
	}
}

void CConfig::Setup()
{
	SetupValue(Vars.Ragebot.Enabled, false, strenc("Ragebot"), strenc("Enabled"));
	SetupValue(Vars.Ragebot.AutoFire, false, strenc("Ragebot"), strenc("Auto.Fire"));
	SetupValue(Vars.Ragebot.Hold, false, strenc("Ragebot"), strenc("OnKey"));
	SetupValue(Vars.Ragebot.HoldKey, 0, strenc("Ragebot"), strenc("OnKey.Key"));
	SetupValue(Vars.Ragebot.RCS, 0, strenc("Ragebot"), strenc("No.Recoil"));//Vars.Ragebot.AutoPistol
	SetupValue(Vars.Ragebot.AutoPistol, false, strenc("Ragebot"), strenc("Auto.Pistol"));
	SetupValue(Vars.Ragebot.pSilent, 0, strenc("Ragebot"), strenc("pSilent"));
	SetupValue(Vars.Ragebot.RCShow, 0.f, strenc("Ragebot"), strenc("NoRecoil.Force"));
	SetupValue(Vars.Ragebot.FOV, 180.f, strenc("Ragebot"), strenc("FOV"));
	SetupValue(Vars.Ragebot.Silent, true, strenc("Ragebot"), strenc("Silent"));
	SetupValue(Vars.Ragebot.Hitbox, 6, strenc("Ragebot"), strenc("Hitbox"));
	SetupValue(Vars.Ragebot.FriendlyFire, false, strenc("Ragebot"), strenc("FriendlyFire"));
	SetupValue(Vars.Ragebot.AutoStop, false, strenc("Ragebot"), strenc("Auto.Stop"));
	SetupValue(Vars.Ragebot.AutoScope, false, strenc("Ragebot"), strenc("Auto.Scope"));
	//SetupValue(Vars.Ragebot.AutoStopCanShoot, false, strenc("Ragebot"), strenc("AutoStopCanShoot"));
	//SetupValue(Vars.Ragebot.AutoStopFast, false, strenc("Ragebot"), strenc("AutoStopFast"));
	//SetupValue(Vars.Ragebot.ShootWhenStopped, false, strenc("Ragebot"), strenc("ShootWhenStopped"));
	SetupValue(Vars.Ragebot.AutoCrouch, false, strenc("Ragebot"), strenc("Auto.Crouch"));
	SetupValue(Vars.Ragebot.AutoWall, true, strenc("Ragebot"), strenc("AutoWall"));
	SetupValue(Vars.Ragebot.AutoWallDmg, 10.f, strenc("Ragebot"), strenc("AutoWall.Damage"));
	SetupValue(Vars.Ragebot.HitChance, false, strenc("Ragebot"), strenc("HitChance"));
	SetupValue(Vars.Ragebot.HitChanceAmt, 20.f, strenc("Ragebot"), strenc("HitChance.Amount"));
	SetupValue(Vars.Ragebot.Aimstep, false, strenc("Ragebot"), strenc("Aimstep"));
	SetupValue(Vars.Ragebot.AimstepAmount, 39.9f, strenc("Ragebot"), strenc("Aimstep.Amount"));
	SetupValue(Vars.Ragebot.HitScan, 0, strenc("Ragebot"), strenc("HitScan.Type"));
	SetupValue(Vars.Ragebot.MultiPoints, false, strenc("Ragebot"), strenc("MultiPoint"));
	SetupValue(Vars.Ragebot.PointScale, 0.1f, strenc("Ragebot"), strenc("Point.Scale"));//Vars.Legitbot.Aimbot.SmokeCheck
	SetupValue(Vars.Ragebot.Antiaim.Enabled, false, strenc("Ragebot"), strenc("Antiaim.Enabled"));
	//SetupValue(Vars.Ragebot.Antiaim.Zeta, 0, strenc("Ragebot"), strenc("Zeta"));
	SetupValue(Vars.Ragebot.Antiaim.FakeYaw, false, strenc("Ragebot"), strenc("Fake.Yaw"));
	SetupValue(Vars.Ragebot.Antiaim.AtPlayer, false, strenc("Ragebot"), strenc("At.Player"));
	SetupValue(Vars.Ragebot.Antiaim.spinspeed, 12, strenc("Ragebot"), strenc("Spin.Speed"));
	SetupValue(Vars.Ragebot.Antiaim.spinspeedz, 12, strenc("Ragebot"), strenc("Fake.Spin.Speed"));
	SetupValue(Vars.Ragebot.Antiaim.PitchCustom, false, strenc("Ragebot"), strenc("Custom.Pitch.Enabled"));
	SetupValue(Vars.Ragebot.Antiaim.PitchCustom, false, strenc("Ragebot"), strenc("Custom.Yaw.Enabled"));
	SetupValue(Vars.Ragebot.Antiaim.YCReal, 0.f, strenc("Ragebot"), strenc("Custom.Yaw.Real"));
	SetupValue(Vars.Ragebot.Antiaim.YCFake, 0.f, strenc("Ragebot"), strenc("Custom.Yaw.Fake"));
	SetupValue(Vars.Ragebot.Antiaim.PCReal, 0.f, strenc("Ragebot"), strenc("Custom.Pitch.Real"));
	SetupValue(Vars.Ragebot.Antiaim.PCFake, 0.f, strenc("Ragebot"), strenc("Custom.Pitch.Fake"));
	SetupValue(Vars.Ragebot.Antiaim.Resolver, false, strenc("Ragebot"), strenc("Antiaim.Resolver"));
	SetupValue(Vars.Ragebot.Antiaim.Pitch, 0, strenc("Ragebot"), strenc("Antiaim.Pitch"));
	SetupValue(Vars.Ragebot.Antiaim.YawReal, 0, strenc("Ragebot"), strenc("Antiaim.Yaw.Real"));
	SetupValue(Vars.Ragebot.Antiaim.YawFake, 0, strenc("Ragebot"), strenc("Antiaim.Yaw.Fake"));
	SetupValue(Vars.Ragebot.ResolverHelper, 0, strenc("Ragebot"), strenc("Pos.Adjusment"));
	SetupValue(Vars.Ragebot.Antiaim.knife_held, false, strenc("Ragebot"), strenc("Antiaim.KnifeHeld"));
	SetupValue(Vars.Ragebot.Antiaim.no_enemy, false, strenc("Ragebot"), strenc("Antiaim.NoEnemy"));
	//SetupValue(Vars.Ragebot.Antiaim.DetectEdge, false, strenc("Ragebot"), strenc("AntiaimEdge"));
	//SetupValue(Vars.Ragebot.Antiaim.EdgeDistance, 15.f, strenc("Ragebot"), strenc("AntiaimEdgeDistance"));
	SetupValue(Vars.Ragebot.UntrustedCheck, true, strenc("Ragebot"), strenc("Anti.Untrusted"));

		SetupValue(Vars.Legitbot.Aimbot.Enabled, true, strenc("Legitbot"), strenc("Enabled"));
		SetupValue(Vars.Legitbot.Aimbot.Key, 1, strenc("Legitbot"), strenc("Key"));
		SetupValue(Vars.Legitbot.Aimbot.OnKey, true, strenc("Legitbot"), strenc("OnKey"));
		SetupValue(Vars.Legitbot.Aimbot.FriendlyFire, false, strenc("Legitbot"), strenc("FriendlyFire"));
		SetupValue(Vars.Legitbot.Aimbot.AutoPistol, true, strenc("Legitbot"), strenc("Legit.AutoPistol"));
		SetupValue(Vars.Legitbot.Aimbot.SmokeCheck, true, strenc("Legitbot"), strenc("SmokeCheck"));//Vars.Legitbot.Aimbot.SmokeCheck

		for (int id = 0; id < 64; id++) {
			SetupValue(Vars.Legitbot.Weapon[id].PFOV, 1.f, GetWeaponName(id), strenc("pSilent.FOV"));
			SetupValue(Vars.Legitbot.Weapon[id].FOV, 2.f, GetWeaponName(id), strenc("FOV"));
//			SetupValue(Vars.Legitbot.Weapon[id].Silent, false, GetWeaponName(id), strenc("Silent"));
			SetupValue(Vars.Legitbot.Weapon[id].pSilent, false, GetWeaponName(id), strenc("pSilent"));
			SetupValue(Vars.Legitbot.Weapon[id].Hitbox, 9, GetWeaponName(id), strenc("Hitbox"));
			SetupValue(Vars.Legitbot.Weapon[id].Speed, 30.f, GetWeaponName(id), strenc("Smooth"));
			//SetupValue(Vars.Legitbot.Weapon[id].RCS, true, GetWeaponName(id), strenc("RCS"));
			SetupValue(Vars.Legitbot.Weapon[id].RCSAmountX, 0.f, GetWeaponName(id), strenc("RCSAmountX"));
			SetupValue(Vars.Legitbot.Weapon[id].RCSAmountY, 0.f, GetWeaponName(id), strenc("RCSAmountY"));
			//SetupValue(Vars.Skins.Weapons[id].StatTrk, 0, GetWeaponName(id), strenc("StatTrak"));
			SetupValue(Vars.Skins.Weapons[id].PaintKit, 0, GetWeaponName(id), strenc("Skin"));
		}

		/*	WEAPON_KNIFE_BAYONET = 500,
		WEAPON_KNIFE_FLIP = 505,
		WEAPON_KNIFE_GUT = 506,
		WEAPON_KNIFE_KARAMBIT = 507,
		WEAPON_KNIFE_M9_BAYONET = 508,
		WEAPON_KNIFE_TACTICAL = 509,
		WEAPON_KNIFE_FALCHION = 512,
		WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
		WEAPON_KNIFE_BUTTERFLY = 515,
		WEAPON_KNIFE_PUSH = 516	 */

		for (int id = 500; id <= 516; id++) {

			if ((id < 505 && id >= 501) || (id > 512 && id < 514))
				continue;

			//SetupValue(Vars.Skins.Weapons[id].StatTrk, 0, GetWeaponName(id), strenc("StatTrak"));
			SetupValue(Vars.Skins.Weapons[id].PaintKit, 0, GetWeaponName(id), strenc("Skin"));

		}

	SetupValue(Vars.Legitbot.Triggerbot.Enabled, true, strenc("Triggerbot"), strenc("Enabled"));
	SetupValue(Vars.Legitbot.Triggerbot.AutoFire, false, strenc("Triggerbot"), strenc("Ignore.Key"));
	SetupValue(Vars.Legitbot.Triggerbot.AutoWall, false, strenc("Triggerbot"), strenc("Autowall"));
	SetupValue(Vars.Legitbot.Triggerbot.Key, 4, strenc("Triggerbot"), strenc("Key"));
	SetupValue(Vars.Legitbot.Triggerbot.HitChance, false, strenc("Triggerbot"), strenc("HitChance"));
	SetupValue(Vars.Legitbot.Triggerbot.HitChanceAmt, 5.f, strenc("Triggerbot"), strenc("HitChance.Amt"));
	SetupValue(Vars.Legitbot.Triggerbot.Delay, 0, strenc("Triggerbot"), strenc("Delay"));
	SetupValue(Vars.Legitbot.Triggerbot.Filter.Head, true, strenc("Triggerbot"), strenc("Filter.Head"));
	SetupValue(Vars.Legitbot.Triggerbot.Filter.Chest, true, strenc("Triggerbot"), strenc("Filter.Chest"));
	SetupValue(Vars.Legitbot.Triggerbot.Filter.Stomach, true, strenc("Triggerbot"), strenc("Filter.Stomach"));
	SetupValue(Vars.Legitbot.Triggerbot.Filter.Arms, true, strenc("Triggerbot"), strenc("Filter.Arms"));
	SetupValue(Vars.Legitbot.Triggerbot.Filter.Legs, true, strenc("Triggerbot"), strenc("Filter.Legs"));
	SetupValue(Vars.Legitbot.Triggerbot.Filter.Friendly, false, strenc("Triggerbot"), strenc("FriendlyFire"));

	SetupValue(Vars.Visuals.Enabled, true, strenc("Visuals"), strenc("Enabled"));
	SetupValue(Vars.Visuals.Box, true, strenc("Visuals"), strenc("Box.2D"));
	SetupValue(Vars.Visuals.esp3d, false, strenc("Visuals"), strenc("Box.3D"));
	SetupValue(Vars.Visuals.Skeleton, false, strenc("Visuals"), strenc("Skeleton"));
	SetupValue(Vars.Visuals.ViewmodelFov, 10, strenc("Visuals"), strenc("FOV.Viewmodel"));
	SetupValue(Vars.Misc.fov, 0, strenc("Visuals"), strenc("FOV.Override"));
	//Vars.Visuals.ViewmodelFov
	SetupValue(Vars.Visuals.BulletTrace, false, strenc("Visuals"), strenc("Bullet.Trace"));
	SetupValue(Vars.Visuals.Line, 0, strenc("Visuals"), strenc("VectorLine"));
	SetupValue(Vars.Visuals.NoScope, false, strenc("Visuals"), strenc("NoScope"));
	SetupValue(Vars.Visuals.SpectatorList, false, strenc("Visuals"), strenc("SpectatorList"));
	SetupValue(Vars.Visuals.CrosshairOn, true, strenc("Visuals"), strenc("Crosshair.On"));
	SetupValue(Vars.Visuals.CrosshairSpread, true, strenc("Visuals"), strenc("Crosshair.Spread"));
	SetupValue(Vars.Visuals.CrosshairType, 1, strenc("Visuals"), strenc("Crosshair.Type"));
	SetupValue(Vars.Visuals.CrosshairRainbow, true, strenc("Visuals"), strenc("Crosshair.Rainbow"));
	SetupValue(Vars.Visuals.CrosshairStyle, 0, strenc("Visuals"), strenc("Crosshair.Style"));
	SetupValue(Vars.Visuals.InfoName, true, strenc("Visuals"), strenc("Info.Name"));
	SetupValue(Vars.Visuals.InfoHealth, true, strenc("Visuals"), strenc("Info.Health"));
	SetupValue(Vars.Visuals.InfoWeapon, true, strenc("Visuals"), strenc("Info.Weapon"));
	SetupValue(Vars.Visuals.InfoFlashed, true, strenc("Visuals"), strenc("Info.Flashed"));
	SetupValue(Vars.Visuals.Filter.Enemies, true, strenc("Visuals"), strenc("Filter.Enemies"));
	SetupValue(Vars.Visuals.Filter.Friendlies, false, strenc("Visuals"), strenc("Filter.Friendlies"));
	SetupValue(Vars.Visuals.Filter.Weapons, true, strenc("Visuals"), strenc("Filter.Weapons"));
	SetupValue(Vars.Visuals.Filter.Decoy, true, strenc("Visuals"), strenc("Filter.Decoy"));
	SetupValue(Vars.Visuals.Filter.C4, true, strenc("Visuals"), strenc("Filter.C4"));//Vars.Visuals.CrosshairFOV
	SetupValue(Vars.Visuals.CrosshairFOV, true, strenc("Visuals"), strenc("Crosshair.ShowFOV"));
	SetupValue(Vars.Visuals.Chams.Enabled, true, strenc("Visuals"), strenc("Chams.Enabled"));
	SetupValue(Vars.Visuals.Chams.Mode, 1, strenc("Visuals"), strenc("Chams.Mode"));
	SetupValue(Vars.Visuals.Chams.XQZ, true, strenc("Visuals"), strenc("Chams.XQZ"));
	SetupValue(Vars.Visuals.Chams.hands, false, strenc("Visuals"), strenc("Hands.Chams"));
	SetupValue(Vars.Visuals.ChamsWeapon, false, strenc("Visuals"), strenc("Weapon.Chams"));
	SetupValue(Vars.Visuals.Chams.RainbowHands, false, strenc("Visuals"), strenc("Hands.Rainbow.Chams"));
	SetupValue(Vars.Visuals.Chams.RainbowTime, 12, strenc("Visuals"), strenc("Rainbow.Speed"));
	SetupValue(Vars.Visuals.Radar.Enabled, true, strenc("Visuals"), strenc("Radar.Enabled"));
	//SetupValue(Vars.Visuals.Radar.X, 500, strenc("Visuals"), strenc("RadarX")); //spinspeed
//	SetupValue(Vars.Visuals.Radar.Y, 500, strenc("Visuals"), strenc("RadarY"));
	SetupValue(Vars.Visuals.Radar.range, 2, strenc("Visuals"), strenc("Radar.Range"));
	SetupValue(Vars.Visuals.Radar.EnemyOnly, true, strenc("Visuals"), strenc("Radar.EnemyOnly"));
	SetupValue(Vars.Visuals.Radar.Nicks, true, strenc("Visuals"), strenc("Radar.Nicknames"));
	SetupValue(Vars.Visuals.RemovalsHands, false, strenc("Visuals"), strenc("Remove.Hands"));
	SetupValue(Vars.Visuals.thirdperson.enabled, false, strenc("Visuals"), strenc("ThirdPerson"));
	SetupValue(Vars.Visuals.thirdperson.distance, 100, strenc( "Visuals" ), strenc( "ThirdPerson.Dist" ) );
	SetupValue(Vars.Visuals.RemovalsVisualRecoil, false, strenc("Visuals"), strenc("Remove.Recoil"));//Vars.Visuals.Line
	SetupValue(Vars.Visuals.RemovalsFlash, false, strenc("Visuals"), strenc("Remove.Flash"));

	//Vars.Misc.FakeLag
	SetupValue(Vars.Misc.FakeLag, 0, strenc("Misc"), strenc("FakeLag.Type"));
	SetupValue(Vars.Misc.FakeLags, 0, strenc("Misc"), strenc("FakeLag.Amount"));
	SetupValue(Vars.Misc.Bhop, true, strenc("Misc"), strenc("Bhop"));
	SetupValue(Vars.Misc.AutoStrafe, 0, strenc("Misc"), strenc("Auto.Strafe"));
	SetupValue(Vars.Misc.Watermark, true, strenc("Misc"), strenc("Watermark"));
	SetupValue(Vars.Misc.TapSay, false, strenc("Misc"), strenc("Spam.On.HS"));
	SetupValue(Vars.Misc.SoundShit, false, strenc("Misc"), strenc("Spam.On.Kill"));
	SetupValue(Vars.Misc.bombinfo, false, strenc("Misc"), strenc("Spam.Bomb.Info"));

	SetupValue(Vars.Misc.Ranks, true, strenc("Misc"), strenc("Ranks"));
	SetupValue(Vars.Misc.AutoAccept, true, strenc("Misc"), strenc("Auto.Accept"));
	SetupValue(Vars.Misc.AirStuck, false, strenc("Misc"), strenc("AirStuck"));
	SetupValue(Vars.Misc.AirStuckKey, 0, strenc("Misc"), strenc("AirStuck.Key"));
	//SetupValue(Vars.Misc.ClantagChanger.enabled, false, strenc("Misc"), strenc("Clantag.Changer.Enabled"));
	//SetupValue(Vars.Misc.ClantagChanger.animation, false, strenc("Misc"), strenc("Clantag.Changer.Animation"));
	//SetupValue(Vars.Misc.ClantagChanger.animation_speed, 650, strenc("Misc"), strenc("Clantag.Changer.Animation.Speed"));
	//SetupValue(Vars.Misc.ClantagChanger.type, STATIC, strenc("Misc"), strenc("Clantag.Changer.Type"));
	//SetupValue( Vars.Misc.ClantagChanger.value, "", strenc( "Misc" ), strenc( "ClantagChangerValue" ) );
	Vars.Misc.ClantagChanger.value = strdup("MorphEngine.cc");
	Vars.Misc.NameChangerenabled = false;
	//SetupValue(Vars.Misc.NameChangerseparator_animation, false, strenc("Misc"), strenc("NameChanger.Separator.Animation"));

	SetupValue(Vars.g_fMColor[0], 0.21f, strenc("Colors"), strenc("Menu.R"));
	SetupValue(Vars.g_fMColor[1], 0.21f, strenc("Colors"), strenc("Menu.G"));
	SetupValue(Vars.g_fMColor[2], 0.21f, strenc("Colors"), strenc("Menu.B"));
	//BAIM
	SetupValue(Vars.g_fBColor[0], 0.1f, strenc("Colors"), strenc("Body.R"));
	SetupValue(Vars.g_fBColor[1], 0.1f, strenc("Colors"), strenc("Body.G"));
	SetupValue(Vars.g_fBColor[2], 0.1f, strenc("Colors"), strenc("Body.B"));
	//TEXT
	SetupValue(Vars.g_fTColor[0], 1.f, strenc("Colors"), strenc("Text.R"));
	SetupValue(Vars.g_fTColor[1], 1.f, strenc("Colors"), strenc("Text.G"));
	SetupValue(Vars.g_fTColor[2], 1.f, strenc("Colors"), strenc("Text.B"));

	//CHAMS
	SetupValue(Vars.g_iChamsTV[0], 0.968f, strenc("Colors"), strenc("Chams.TV.R"));
	SetupValue(Vars.g_iChamsTV[1], 0.705f, strenc("Colors"), strenc("Chams.TV.G"));
	SetupValue(Vars.g_iChamsTV[2], 0.078f, strenc("Colors"), strenc("Chams.TV.B"));

	SetupValue(Vars.g_iChamsTH[0], 0.941f, strenc("Colors"), strenc("Chams.TH.R"));
	SetupValue(Vars.g_iChamsTH[1], 0.117f, strenc("Colors"), strenc("Chams.TH.G"));
	SetupValue(Vars.g_iChamsTH[2], 0.137f, strenc("Colors"), strenc("Chams.TH.B"));
	//CT
	SetupValue(Vars.g_iChamsCTV[0], 0.125f, strenc("Colors"), strenc("Chams.CTV.R"));
	SetupValue(Vars.g_iChamsCTV[1], 0.705f, strenc("Colors"), strenc("Chams.CTV.G"));
	SetupValue(Vars.g_iChamsCTV[2], 0.223f, strenc("Colors"), strenc("Chams.CTV.B"));

	SetupValue(Vars.g_iChamsCTH[0], 0.247f, strenc("Colors"), strenc("Chams.CTH.R"));
	SetupValue(Vars.g_iChamsCTH[1], 0.282f, strenc("Colors"), strenc("Chams.CTH.G"));
	SetupValue(Vars.g_iChamsCTH[2], 0.803f, strenc("Colors"), strenc("Chams.CTH.B"));

	//bool    bones[6] = { true, true, true, true, true, true };//

	SetupValue(Vars.Ragebot.bones[0], true, strenc("RbotBones"), strenc("Hitscan.Head"));
	SetupValue(Vars.Ragebot.bones[1], true, strenc("RbotBones"), strenc("Hitscan.Neck"));
	SetupValue(Vars.Ragebot.bones[2], true, strenc("RbotBones"), strenc("Hitscan.Pelvis"));
	SetupValue(Vars.Ragebot.bones[3], true, strenc("RbotBones"), strenc("Hitscan.Spine"));
	SetupValue(Vars.Ragebot.bones[4], true, strenc("RbotBones"), strenc("Hitscan.Legs"));
	SetupValue(Vars.Ragebot.bones[5], true, strenc("RbotBones"), strenc("Hitscan.Arms"));

	SetupValue(Vars.Skins.Glove, 0, strenc("Skins"), strenc("GloveModel"));
	SetupValue(Vars.Skins.GloveSkin, 0, strenc("Skins"), strenc("GloveSkin"));
	SetupValue(Vars.Skins.KnifeModel, 0, strenc("Skins"), strenc("KnifeModel"));
}

void CConfig::SetupValue( int &value, int def, std::string category, std::string name )
{
	value = def;
	ints.push_back( new ConfigValue<int>( category, name, &value ) );
}

void CConfig::SetupValue( float &value, float def, std::string category, std::string name )
{
	value = def;
	floats.push_back( new ConfigValue<float>( category, name, &value ) );
}

void CConfig::SetupValue( bool &value, bool def, std::string category, std::string name )
{
	value = def;
	bools.push_back( new ConfigValue<bool>( category, name, &value ) );
}

void CConfig::Save()
{
	static char path[ MAX_PATH ];
	std::string folder, file;

	if( SUCCEEDED( SHGetFolderPathA( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		char szCmd[256];
		sprintf(szCmd, "\\Morph\\%s.ini", Vars.Misc.configname);

		folder = std::string( path ) + strenc( "\\Morph\\" );
		file = std::string(path) + szCmd;
	}

	CreateDirectoryA( folder.c_str(), NULL );

	for ( auto value : ints )
		WritePrivateProfileStringA( value->category.c_str(), value->name.c_str(), std::to_string( *value->value ).c_str(), file.c_str() );

	for ( auto value : floats )
		WritePrivateProfileStringA( value->category.c_str(), value->name.c_str(), std::to_string( *value->value ).c_str(), file.c_str() );

	for ( auto value : bools )
		WritePrivateProfileStringA( value->category.c_str(), value->name.c_str(), *value->value ? "true" : "false", file.c_str() );
}

void CConfig::Load()
{
	static char path[MAX_PATH];
	std::string folder, file;

	if ( SUCCEEDED( SHGetFolderPathA( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		char szCmd[256];
		sprintf(szCmd, "\\Morph\\%s.ini", Vars.Misc.configname);

		folder = std::string(path) + strenc("\\Morph\\");
		file = std::string(path) + szCmd;
	}

	CreateDirectoryA( folder.c_str(), NULL );

	char value_l[32] = { '\0' };

	for ( auto value : ints )
	{
		GetPrivateProfileStringA( value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str() );
		*value->value = atoi( value_l );
	}

	for ( auto value : floats )
	{
		GetPrivateProfileStringA( value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str() );
		*value->value = atof( value_l );
	}

	for ( auto value : bools )
	{
		GetPrivateProfileStringA( value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str() );
		*value->value = !strcmp( value_l, "true" );
	}
}


CConfig* Config = new CConfig();
Variables Vars;
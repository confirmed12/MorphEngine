#pragma once
#include "../ImGUI/imgui.h"

class cVisuals
{
public:
	bool 	Enabled;
	bool 	Box;
	int 	BoxType;
	bool	Skeleton;
	bool	Glow;
	bool	SpectatorList;
	bool	BulletTrace;
	int ViewmodelFov;
	bool esp3d;
	int Line;
	bool CrosshairFOV;
	//float   BulletTraceLength;
	bool	CrosshairOn;
	bool CrosshairSpread;
	bool		CrosshairType;
	int		CrosshairStyle;
	bool CrosshairRainbow;
	float 	TeamColor[3];
	float 	TeamColorWall[3];
	float 	EnemyColor[3];
	float 	EnemyColorWall[3];
	struct
	{
		bool Enemies;
		bool Friendlies;
		bool Weapons;
		bool Decoy;
		bool C4;
	} Filter;

	struct
	{
		bool enabled;
		float distance = 30.f;
	} thirdperson;


	struct
	{
		bool	RainbowHands;
		float 	RainbowTime;
		bool	Enabled;
		int 	Mode;
		bool	Wireframe;
		bool	hands;
		bool	XQZ;
		float 	TeamColor[3];
		float 	TeamColorWall[3];
		float  	EnemyColor[3];
		float 	EnemyColorWall[3];
	} Chams;

	struct
	{
		bool Enabled;
		bool EnemyOnly;
		int range;
		bool Nicks;
	} Radar;

	bool NoScope;
	bool AntiSS;

	bool 	InfoName;
	bool 	InfoHealth;
	bool 	InfoWeapon;
	bool	InfoFlashed;

	bool	RemovalsHands;
	bool	HandChams;
	bool	ChamsWeapon;
	bool	RemovalsVisualRecoil;
	bool	RemovalsFlash;
	bool    RemovalsSmoke;
	bool	RemovalsSky;
};

class cSkins
{
public:
		bool SSEnabled;
		int Glove;
		int GloveSkin;
		int KnifeModel;

		struct wpnz
		{
			int PaintKit;
			int StatTrk;
			float Wear;
		}Weapons[519];
};

class cMenu
{
public:
		bool	Opened = false;
		int 	Key;
		bool	Ragebot = false;
		bool	Legitbot = false;
		bool	Visual = false;
		bool	Misc = false;
};

class cMisc
{
public:
		int FakeLag;
		int		FakeLags;
		int		yawchocked;
		int		fov;
		bool 	Bhop;
		int 	AutoStrafe;
		bool	ChatSpam;
		bool SwastikaSpam;
		bool NameSteal;
		bool NameStealSelf;
		float NameStealDelay = 1.f;
		bool LocSpam;
		int Cname;
		int alpha;
		bool LocSpamEnemies;
		bool LocSpamChat;
		float ChatSpamDelay = 0.5f;
		char ChatSpamMode[128] = "Visit MorphEngine.cc for Premium Cheats!";
		bool TapSay;
		bool SoundShit;
		bool bombinfo;
		char configname[128] = "default";
		bool	Watermark;
		bool	Ranks;
		bool	AirStuck;
		int		AirStuckKey;
		bool	AutoAccept;
		bool Unload;
		struct
		{
			bool enabled;
			char* value;
			bool animation;
			int animation_speed;
			int type;
		} ClantagChanger;
		bool NameChangerenabled;
		int NameChangerseparator_animation;
};

class cRagebot
{
public:
	bool 	Enabled;
	bool 	AutoFire;
	bool 	Hold;
	int 	HoldKey;
	int 	Filter; // will hold flags for team/enemy/etc.
	bool 	Silent;
	int		RCS;
	bool pSilent;
	float	RCShow;
	bool 	VisCheck;
	bool 	AutoWall;
	float	AutoWallDmg;
	int		Hitbox;
	float 	FOV;
	bool 	HitChance;
	float	HitChanceAmt;
	bool MultiPoints;
	float PointScale;
	bool 	AutoStop;
	bool AutoScope;
	bool 	AutoStopFast;
	bool 	AutoStopCanShoot;
	bool	ShootWhenStopped;//
	bool 	AutoCrouch;
	bool 	FriendlyFire;
	bool	Aimstep;
	float	AimstepAmount;
	int	HitScan;
	bool NoSpreadFix;
	//bool HVHhs;
	bool AutoPistol;
	bool	UntrustedCheck = true;
	bool    bones[6] = { true, true, true, true, true, true };//
	int ResolverHelper;

	struct
	{
		bool	Enabled;
		bool FakeYaw;
		int	Resolver;
		bool	knife_held;
		bool	no_enemy;
		int		Pitch;
		int		YawReal;
		int		YawFake;//
		bool	DetectEdge;//
		float	EdgeDistance;//
		bool	Shuffle;
		bool AtPlayer;
		int Zeta;
		int spinspeed;
		int spinspeedz;
		bool YawCustom;
		bool PitchCustom;
		float PCReal;
		float PCFake;
		float YCFake;
		float YCReal;
	} Antiaim;
};

class cLegitBot
{
public:
	struct
	{
		bool 	Enabled;
		int 	Key;
		bool	OnKey;
		int group;
		int 	Filter; // will hold flags for team/enemy/etc.
		bool smart;
		bool 	VisCheck;
		bool 	AlwaysOn;
		bool	FriendlyFire;
		bool	SmokeCheck;
		bool AutoPistol;
	} Aimbot;

	struct
	{
		bool 	Enabled;
		bool 	pSilent;
		float RCSAmountX;
		float RCSAmountY;
		float	Speed;
		int 	Hitbox;
		float	PFOV;
		float 	FOV;
		bool 	AdaptiveSmooth;
	} Weapon[519];

	bool fastzoom;
	bool fastzoomswitch;
	int delay;

	struct
	{
		bool	Enabled;
		bool	AutoFire;
		int		Key;
		bool 	HitChance;
		float	HitChanceAmt;
		bool AutoWall;
		int Delay;


		struct
		{
			bool Head;
			bool Chest;
			bool Stomach;
			bool Arms;
			bool Legs;
			bool Friendly;
			bool smoke;
		} Filter;

	} Triggerbot;

};

class cPlayersList
{
public:
	bool	Playerlist;
	bool	Resolveall;
	struct
	{
		//int YAngle[64];
		int PAngle[64];
		int YAngle[64];
		bool MiscFixes;
	}AAA;

};

class Variables
{
public:
	//CRageBotTab RageBotTab;
	cRagebot Ragebot;
	cPlayersList Players;
	cVisuals Visuals;
	cMisc Misc;
	cLegitBot Legitbot;
	cSkins Skins;
	cMenu Menu;

	int wpn;

	Variables()
	{
		wpn = -1;
	}
	int bomber;
	int Hostage;
	bool font2;
	int defuser;
	bool bomb;
	bool shoot;
	//QAngle lastTickViewAngles = { 0,0,0 };
	bool bIsNoShot;
	int m_iBulletsFire = 0;

	float g_fMColor[4] = { 0.21f, 0.21f, 0.21f, 1.0f }; //RGBA color
	float g_fBColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float g_fTColor[4] = { 1.f, 1.f, 1.f, 1.0f };

	float g_iChamsTV[4] = { 0.968f, 0.705f, 0.078f, 1.0f };
	float g_iChamsCTH[4] = { 0.247f, 0.282f, 0.803f, 1.0f };
	float g_iChamsTH[4] = { 0.941f, 0.117f, 0.137f, 1.0f };
	float g_iChamsCTV[4] = { 0.125f, 0.705f, 0.223f, 1.0f };

	ImFont* font;
	//CColorsTab ColorsTab;
};

	extern Variables Vars;
#pragma once

#include "SDK.h"
#include "Settings\Vars.h"
#include "Settings\Config.h"
#include "SDK\Draw.h"
#include "Hooks.h"
#include "Enhancements.h"
#include "SDK\Input.h"
#include "XorStr.h"
//#include "VMProtectSDK.h"

struct Config
{
	std::string name;
	std::string path;

	Config(const char* name, const char* path);
};

void ApplyAAAHooks();

#define CSGO_VERSION XorStr("1.35.8.7")

#define CSGO_GLOWMANAGER XorStr("0x4FE3A5C")
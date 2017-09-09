#include "../sdk.h"
#include <fstream>
#include <Psapi.h>
#include "../SkinChanger.h"
#include "../Parser.h"
#include "../xor.h"

#include <wininet.h>
#include <cstring>
#include <windows.h>
#include <iostream>
#include <urlmon.h>
#include <ctype.h>

#pragma comment(lib,"wininet.lib")
#pragma comment(lib, "urlmon.lib")


MsgFn U::PrintMessage = (MsgFn)GetProcAddress(GetModuleHandleA(charenc("tier0.dll")), charenc("Msg"));
SendClanTagFn U::SendClanTag;
ServerRankRevealAllFn U::ServerRankRevealAllEx;
InitKeyValuesFn U::InitKeyValuesEx;
LoadFromBufferFn U::LoadFromBufferEx;
IsReadyFn U::IsReady;
CL_FullUpdate_t U::CL_FullUpdate = NULL;
LineGoesThroughSmokeFn U::LineToSmoke = NULL;
TraceToExitFn U::TraceToExit = NULL;

void U::SetupInterfaces()
{
	I::Client = U::CaptureInterface<IBaseClientDll>(strenc("client.dll"), strenc("VClient018"));
	I::ClientMode = **(IClientModeShared***)((*(DWORD**)I::Client)[10] + 5);
	//I::Input			= *reinterpret_cast< CInput** >((*reinterpret_cast<DWORD**>(I::Client))[15] + 0x1);
	auto pClientVFTable = *reinterpret_cast<uint32_t**>(I::Client);
	I::Input = *reinterpret_cast<CInput**>(pClientVFTable[15] + 1);
	//I::Input			= **(CInput***)((*(uint32_t**)I::Client)[15] + 1);
	I::ClientEntList = U::CaptureInterface<IClientEntityList>(strenc("client.dll"), strenc("VClientEntityList003"));
	I::Cvar = U::CaptureInterface<ICVar>(strenc("vstdlib.dll"), strenc("VEngineCvar007"));
	//I::MDLCache			= U::CaptureInterface<IMDLCache>(strenc("datacache.dll"), strenc("MDLCache004"));
	I::Engine = U::CaptureInterface<IEngineClient>(/*engine.dll*/Xor<0xF1, 11, 0x39AB18D2>("\x94\x9C\x94\x9D\x9B\x93\xD9\x9C\x95\x96" + 0x39AB18D2).s, strenc("VEngineClient014"));
	I::EngineTrace = U::CaptureInterface<IEngineTrace>(/*engine.dll*/Xor<0xF1, 11, 0x39AB18D2>("\x94\x9C\x94\x9D\x9B\x93\xD9\x9C\x95\x96" + 0x39AB18D2).s, strenc("EngineTraceClient004"));
	I::InputSystem = U::CaptureInterface<IInputSystem>(strenc("inputsystem.dll"), strenc("InputSystemVersion001"));
	I::Localize = U::CaptureInterface<ILocalize>(strenc("localize.dll"), strenc("Localize_001"));
	I::Globals = **(IGlobalVarsBase***)((*(DWORD**)I::Client)[0] + 0x1B);
	I::Surface = U::CaptureInterface<ISurface>(strenc("vguimatsurface.dll"), strenc("VGUI_Surface031"));
	//I::FileSystem		= U::CaptureInterface<IBaseFileSystem>(strenc("filesystem.dll"), strenc("VBaseFileSystem011"));
	I::GameEvent = U::CaptureInterface<IGameEventManager2>(/*engine.dll*/Xor<0xF1, 11, 0x39AB18D2>("\x94\x9C\x94\x9D\x9B\x93\xD9\x9C\x95\x96" + 0x39AB18D2).s, strenc("GAMEEVENTSMANAGER002"));
	I::VPanel = U::CaptureInterface<IVPanel>(strenc("vgui2.dll"), strenc("VGUI_Panel009"));
	I::RenderView = U::CaptureInterface<IVRenderView>(/*engine.dll*/Xor<0xF1, 11, 0x39AB18D2>("\x94\x9C\x94\x9D\x9B\x93\xD9\x9C\x95\x96" + 0x39AB18D2).s, strenc("VEngineRenderView014"));
	I::ModelRender = U::CaptureInterface<IVModelRender>(/*engine.dll*/Xor<0xF1, 11, 0x39AB18D2>("\x94\x9C\x94\x9D\x9B\x93\xD9\x9C\x95\x96" + 0x39AB18D2).s, strenc("VEngineModel016"));
	I::MaterialSystem = U::CaptureInterface<IMaterialSystem>(strenc("materialsystem.dll"), strenc("VMaterialSystem080"));
	I::ModelInfo = U::CaptureInterface<IVModelInfo>(/*engine.dll*/Xor<0xF1, 11, 0x39AB18D2>("\x94\x9C\x94\x9D\x9B\x93\xD9\x9C\x95\x96" + 0x39AB18D2).s, strenc("VModelInfoClient004"));
	I::GameMovement = U::CaptureInterface<IGameMovement>(strenc("client.dll"), strenc("GameMovement001"));
	I::Prediction = U::CaptureInterface<IPrediction>(strenc("client.dll"), strenc("VClientPrediction001"));
	I::Physprops = U::CaptureInterface<IPhysicsSurfaceProps>(strenc("vphysics.dll"), strenc("VPhysicsSurfaceProps001"));
	I::DebugOverlay = U::CaptureInterface<IVDebugOverlay>(/*engine.dll*/Xor<0xF1, 11, 0x39AB18D2>("\x94\x9C\x94\x9D\x9B\x93\xD9\x9C\x95\x96" + 0x39AB18D2).s, strenc("VDebugOverlay004"));
	I::StudioRender = U::CaptureInterface<IStudioRender>(strenc("studiorender.dll"), strenc("VStudioRender026"));
	I::MoveHelper = **(IMoveHelper***)(U::FindPattern(strenc("client.dll"), strenc("8B 0D ? ? ? ? 8B 46 08 68")) + 0x2);

	GetSkins(k_Skins, k_Gloves, /*csgo*/Xor<0xDE, 5, 0xF859DF4F>("\xBD\xAC\x87\x8E" + 0xF859DF4F).s);
}
/*
inline void**& getvtable(void* inst, size_t offset = 0)
{
return *reinterpret_cast<void***>((size_t)inst + offset);
}

inline uintptr_t GetAbsoluteAddress(uintptr_t instruction_ptr, int offset, int size)
{
return instruction_ptr + *reinterpret_cast<uint32_t*>(instruction_ptr + offset) + size;
};
*/

bool DoesCSGOVerMatch() {
	HINTERNET connect = InternetOpen(/*MyBrowser*/Xor<0x90, 10, 0xF6D9C953>("\xDD\xE8\xD0\xE1\xFB\xE2\xE5\xF2\xEA" + 0xF6D9C953).s, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if (!connect)
		return false;
	//http://morphster.xyz/checker/curcsgover.php
	std::string URL = /*http://morphster.xyz/checker/curcsgover.php*/Xor<0xA4, 44, 0x13ACA975>("\xCC\xD1\xD2\xD7\x92\x86\x85\xC6\xC3\xDF\xDE\xC7\xC3\xC5\xD7\xC1\x9A\xCD\xCF\xCD\x97\xDA\xD2\xDE\xDF\xD6\xDB\xCD\xEF\xA2\xB7\xB1\xA7\xB6\xA1\xA8\xBE\xAC\xB8\xE5\xBC\xA5\xBE" + 0x13ACA975).s;
	LPCSTR Final = URL.c_str();

	HINTERNET OpenAddress = InternetOpenUrl(connect, Final, NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);

	if (!OpenAddress) {
		InternetCloseHandle(connect);
		return false;
	}

	char DataReceived[4096];
	DWORD NumberOfBytesRead = 0;
	while (InternetReadFile(OpenAddress, DataReceived, 4096, &NumberOfBytesRead) && NumberOfBytesRead) {}

	InternetCloseHandle(OpenAddress);
	InternetCloseHandle(connect);

	if (strstr(DataReceived, I::Engine->GetProductVersionString())) {
		return true;
	}

	return false;
}

// learn_more
DWORD U::FindPattern(std::string moduleName, std::string pattern)
{
	const char* pat = pattern.c_str();
	DWORD firstMatch = 0;
	DWORD rangeStart = (DWORD)GetModuleHandleA(moduleName.c_str());
	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
	for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
	{
		if (!*pat)
			return firstMatch;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
		{
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				return firstMatch;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else
				pat += 2;    //one ?
		}
		else
		{
			pat = pattern.c_str();
			firstMatch = 0;
		}
	}
	return NULL;
}

inline bool U::Compare(const uint8_t* data, const uint8_t* pattern, const char* mask) {
	for (; *mask; ++mask, ++data, ++pattern)
		if (*mask == 'x' && *data != *pattern)
			return false;

	return (*mask) == 0;
}

DWORD_PTR U::FindPattern3(std::string strModuleName, PBYTE pbPattern, std::string strMask, DWORD_PTR nCodeBase, DWORD_PTR nSizeOfCode)
{
	BOOL bPatternDidMatch = FALSE;
	HMODULE hModule = GetModuleHandleA(strModuleName.c_str());

	if (!hModule)
		return 0;

	PIMAGE_DOS_HEADER pDsHeader = PIMAGE_DOS_HEADER(hModule);
	PIMAGE_NT_HEADERS pPeHeader = PIMAGE_NT_HEADERS(LONG(hModule) + pDsHeader->e_lfanew);
	PIMAGE_OPTIONAL_HEADER pOptionalHeader = &pPeHeader->OptionalHeader;

	if (!nCodeBase)
		nCodeBase = (ULONG)hModule + pOptionalHeader->BaseOfCode;

	if (!nSizeOfCode)
		nSizeOfCode = pOptionalHeader->SizeOfCode;

	std::size_t nMaskSize = strMask.length();

	if (!nCodeBase || !nSizeOfCode || !nMaskSize)
		return 0;

	for (DWORD_PTR i = nCodeBase; i <= nCodeBase + nSizeOfCode; i++)
	{
		for (size_t t = 0; t < nMaskSize; t++)
		{
			if (*((PBYTE)i + t) == pbPattern[t] || strMask.c_str()[t] == '?')
				bPatternDidMatch = TRUE;

			else
			{
				bPatternDidMatch = FALSE;
				break;
			}
		}

		if (bPatternDidMatch)
			return i;
	}

	return 0;
}

uintptr_t U::FindPattern2(const char* module, const char* pattern_string, const char* mask) {
	MODULEINFO module_info = {};
	GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module), &module_info, sizeof MODULEINFO);

	uintptr_t module_start = uintptr_t(module_info.lpBaseOfDll);

	const uint8_t* pattern = reinterpret_cast<const uint8_t*>(pattern_string);

	for (size_t i = 0; i < module_info.SizeOfImage; i++)
		if (Compare(reinterpret_cast<uint8_t*>(module_start + i), pattern, mask))
			return module_start + i;

	return 0;
}
void U::StdReplaceStr(std::string& replaceIn, const std::string& replace, const std::string& replaceWith)
{
	size_t const span = replace.size();
	size_t const step = replaceWith.size();
	size_t index = 0;

	while (true)
	{
		index = replaceIn.find(replace, index);

		if (index == std::string::npos)
			break;

		replaceIn.replace(index, span, replaceWith);
		index += step;
	}
}

CBaseEntity* U::GetLocalPlayer()
{
	return I::ClientEntList->GetClientEntity(I::Engine->GetLocalPlayer());
}

CNetVarManager* U::NetVars = new CNetVarManager;

wchar_t* U::ConvertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

void U::ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr)
{
	if (!offsets.ClipTraceToPlayersFn)
		return;

	_asm
	{
		MOV		EAX, filter
		LEA		ECX, tr
		PUSH	ECX
		PUSH	EAX
		PUSH	mask
		LEA		EDX, vecAbsEnd
		LEA		ECX, vecAbsStart
		CALL	offsets.ClipTraceToPlayersFn
		ADD		ESP, 0xC
	}
}

void U::TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, CBaseEntity* ignore, trace_t* ptr)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	CTraceFilter filter;
	filter.pSkip = ignore;

	I::EngineTrace->TraceRay(ray, mask, &filter, ptr);
}
bool FileLog = true;
std::ofstream logFile;

std::string GetTimeString()
{
	//Time related variables
	time_t current_time;
	struct tm *time_info;
	static char timeString[10];

	//Get current time
	time(&current_time);
	time_info = localtime(&current_time);

	//Get current time as string
	strftime(timeString, sizeof(timeString), "%I:%M%p", time_info);
	return timeString;
}

void SetConsoleColor(WORD color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

#define FOREGROUND_WHITE		    (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN)
#define FOREGROUND_INTENSE_GREEN	(FOREGROUND_GREEN | FOREGROUND_INTENSITY)

void U::Log(const char *fmt, ...)
{
	if (!fmt) return; //if the passed string is null return
	if (strlen(fmt) < 2) return;

	//Set up va_list and buffer to hold the params 
	va_list va_alist;
	char logBuf[256] = { 0 };

	//Do sprintf with the parameters
	va_start(va_alist, fmt);
	_vsnprintf(logBuf + strlen(logBuf), sizeof(logBuf) - strlen(logBuf), fmt, va_alist);
	va_end(va_alist);

	//Output to console
	if (logBuf[0] != '\0')
	{
		SetConsoleColor(FOREGROUND_INTENSE_GREEN);
		printf("[%s]", GetTimeString().c_str());
		SetConsoleColor(FOREGROUND_WHITE);
		printf(": %s\n", logBuf);
	}

	if (FileLog)
	{
		logFile << logBuf << std::endl;
	}
}

void U::ServerRankRevealAll()
{
	static float fArray[3] = { 0.f, 0.f, 0.f };

	U::ServerRankRevealAllEx = (ServerRankRevealAllFn)(offsets.ServerRankRevealAllEx);
	U::ServerRankRevealAllEx(fArray);
}

void U::InitKeyValues(KeyValues* pKeyValues, const char* name)
{
	U::InitKeyValuesEx = (InitKeyValuesFn)(offsets.InitKeyValuesEx);
	U::InitKeyValuesEx(pKeyValues, name);
}

void U::LoadFromBuffer(KeyValues* pKeyValues, const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc)
{
	U::LoadFromBufferEx = (LoadFromBufferFn)(offsets.LoadFromBufferEx);
	U::LoadFromBufferEx(pKeyValues, resourceName, pBuffer, pFileSystem, pPathID, pfnEvaluateSymbolProc);
}

void U::SendPacket(byte toggle)
{
	*(byte*)(offsets.SendPacket) = toggle;
}

float U::RandomFloat(float min, float max)
{
	assert(max > min);
	float random = ((float)rand()) / (float)RAND_MAX;
	float range = max - min;
	return (random*range) + min;
}

void U::SetupHooks()
{
	H::VPanel = new VTHook((DWORD**)I::VPanel);
	H::ClientMode = new VTHook((DWORD**)I::ClientMode);
	H::Client = new VTHook((DWORD**)I::Client);
	H::ModelRender = new VTHook((DWORD**)I::ModelRender);
	//H::MDLCache = new VTHook((DWORD**)I::MDLCache);
	H::Surface = new VTHook((DWORD**)I::Surface);
	H::GameEvent = new VTHook((DWORD**)I::GameEvent);
	H::D3D9 = new VTHook((DWORD**)offsets.d3d9Device);

	oPaintTraverse = (PaintTraverseFn)H::VPanel->HookFunction((DWORD)Hooks::PaintTraverse, 41);
	oCreateMove = (CreateMoveFn)H::ClientMode->HookFunction((DWORD)Hooks::CreateMove, 24);
	//oFireEventClientSide = (FireEventClientSideFn)H::GameEvent->HookFunction((DWORD)Hooks::hkFireEventClientSide, 9);
	oOverrideView = (OverrideViewFn)H::ClientMode->HookFunction((DWORD)Hooks::OverrideView, 18);
	//oFindMDL = ( FindMdlFn )H::MDLCache->HookFunction( ( DWORD)Hooks::FindMDL, 10);
	oFrameStageNotify = (FrameStageNotifyFn)H::Client->HookFunction((DWORD)Hooks::FrameStageNotify, 36);
	oDrawModelExecute = (DrawModelExecuteFn)H::ModelRender->HookFunction((DWORD)Hooks::DrawModelExecute, 21);
	oPlaySound = (PlaySoundFn)H::Surface->HookFunction((DWORD)Hooks::PlaySound_CSGO, 82);
	oReset = (ResetFn)H::D3D9->HookFunction((DWORD)Hooks::Reset, 16);
	oEndScene = (EndSceneFn)H::D3D9->HookFunction((DWORD)Hooks::EndScene, 42);
	//oGetViewModel = (oGetViewModelFOV)H::ClientMode->HookFunction((DWORD)Hooks::hkGetViewModelFOV, 35);

	U::SendClanTag = (SendClanTagFn)U::FindPattern(/*engine.dll*/Xor<0xF1, 11, 0x39AB18D2>("\x94\x9C\x94\x9D\x9B\x93\xD9\x9C\x95\x96" + 0x39AB18D2).s, /*53 56 57 8B DA 8B F9 FF 15*/Xor<0x81, 27, 0x29598A19>("\xB4\xB1\xA3\xB1\xB3\xA6\xB2\xBF\xA9\xB2\xC9\xAC\xC9\xCF\xAF\xA8\xD3\xB2\xD5\xAD\xB5\xD0\xD1\xB8\xA8\xAF" + 0x29598A19).s);
	U::CL_FullUpdate = (CL_FullUpdate_t)U::FindPattern(/*engine.dll*/Xor<0xF1, 11, 0x39AB18D2>("\x94\x9C\x94\x9D\x9B\x93\xD9\x9C\x95\x96" + 0x39AB18D2).s, /*A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85*/Xor<0x6F, 43, 0xCCC757C2>("\x2E\x41\x51\x4D\x53\x4B\x55\x49\x57\x47\x59\x38\x42\x5C\x42\x5E\x40\xA0\xBE\xA2\xBC\xA4\xB0\xB0\xA7\xCE\xCF\xAA\xBE\xBC\xAD\xBF\xBB\xB0\xA9\xD0\xB3\xA7\xA1\xB6\xAF\xAD" + 0xCCC757C2).s);
	U::LineToSmoke = (LineGoesThroughSmokeFn)U::FindPattern(/*client.dll*/Xor<0xBA, 11, 0xFD2F333C>("\xD9\xD7\xD5\xD8\xD0\xCB\xEE\xA5\xAE\xAF" + 0xFD2F333C).s, /*55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0*/Xor<0x19, 41, 0xCD758030>("\x2C\x2F\x3B\x24\x5F\x3E\x5A\x63\x01\x1A\x10\x04\x60\x65\x07\x18\x11\x0A\x13\x6E\x0D\x1F\x1A\x10\x0E\x12\x0C\x14\x0A\x16\x08\x18\x09\x7C\x1B\x09\x0A\x1E\x7C\x70" + 0xCD758030).s);
	U::TraceToExit = (TraceToExitFn)U::FindPattern(/*client.dll*/Xor<0xBA, 11, 0xFD2F333C>("\xD9\xD7\xD5\xD8\xD0\xCB\xEE\xA5\xAE\xAF" + 0xFD2F333C).s, /*55 8B EC 83 EC 30 F3 0F 10 75*/Xor<0x59, 30, 0xABD5439C>("\x6C\x6F\x7B\x64\x1F\x7E\x1A\x23\x41\x5A\x50\x44\x20\x25\x47\x5B\x59\x4A\x2D\x5F\x4D\x5E\x29\x50\x40\x42\x53\x43\x40" + 0xABD5439C).s);
	//GetSkins(k_Skins, k_Gloves, "csgo");

	//NetVars->DumpNetvars();
}

void U::SetupOffsets()
{
	U::NetVars->Initialize();
	Offsets::GrabOffsets();
}

void U::SetupTextures()
{
	visible_flat = I::MaterialSystem->CreateMaterial(true, false, false);
	visible_tex = I::MaterialSystem->CreateMaterial(false, false, false);
	hidden_flat = I::MaterialSystem->CreateMaterial(true, true, false);
	hidden_tex = I::MaterialSystem->CreateMaterial(false, true, false);
}

void U::Setup()
{
	U::SetupInterfaces();

	U::SetupOffsets();

	if (DoesCSGOVerMatch() == false)
		return;

	D::SetupFonts();

	U::SetupTextures();

	U::SetupHooks();

	//SetupOffsets();

	Config->Setup();


	//START VARS
	Vars.Ragebot.UntrustedCheck = true;
	//Vars.Legitbot. = 8;

	//	U::NetVars->HookProp("DT_BaseViewModel", "m_nModelIndex", SkinChanger::RecvProxy_Viewmodel);

	ApplyAAAHooks();

	if (DoesCSGOVerMatch() == false) {
		E::Misc->Panic();
		ExitProcess(0);
	}
}

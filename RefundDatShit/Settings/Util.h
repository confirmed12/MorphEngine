#pragma once

//#define RandomInt(min, max) (rand() % (max - min + 1) + min)

#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

#define MakePtr (cast, ptr, addValue) ( cast )( (DWORD)( ptr ) + ( DWORD )( addValue ) )
//#define MakePtre (Type, Base, Offset) (*(Type*)((DWORD)Base + Offset))

class KeyValues;

using MsgFn					= void( __cdecl* )( char const *, ... );
using ServerRankRevealAllFn = bool( __cdecl* )( float* );
using SendClanTagFn			= void( __fastcall* )( const char*, const char* );
using InitKeyValuesFn		= void( __thiscall* )( void* thisptr, const char* name );
using LoadFromBufferFn		= void( __thiscall* )( void* thisptr, const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc );
using IsReadyFn				= void( __cdecl* )( );
typedef void(*CL_FullUpdate_t) (void);
typedef bool(__fastcall* TraceToExitFn)(Vector&, trace_t&, float, float, float, float, float, float, trace_t*);
typedef bool(*LineGoesThroughSmokeFn) (Vector, Vector, int16_t);
//typedef void(*CL_ThirdPers) (void);

namespace U
{
	extern MsgFn PrintMessage;
	extern SendClanTagFn SendClanTag;
	extern CL_FullUpdate_t CL_FullUpdate;
	extern LineGoesThroughSmokeFn LineToSmoke;
	extern ServerRankRevealAllFn ServerRankRevealAllEx;
	extern InitKeyValuesFn InitKeyValuesEx;
	extern LoadFromBufferFn LoadFromBufferEx;
	extern TraceToExitFn TraceToExit;
	extern IsReadyFn IsReady;
	//extern CL_ThirdPers CamThinkSvCheatsCheck;
	
	//extern bool DoesCSGOVerMatch();

	extern DWORD FindPattern( std::string moduleName, std::string pattern );
	inline bool Compare(const unsigned char * pData, const unsigned char * bMask, const char * szMask);
	extern DWORD_PTR FindPattern3(std::string strModuleName, PBYTE pbPattern, std::string strMask, DWORD_PTR nCodeBase = NULL, DWORD_PTR nSizeOfCode = NULL);
	//uintptr_t FindPattern(const char * szModule, unsigned char * bMask, const char * szMask);
	//extern uintptr_t FindPattern2(const char * szModule, unsigned char * bMask, const char * szMask);
	extern uintptr_t FindPattern2(const char * module, const char * pattern_string, const char * mask);
	extern void StdReplaceStr(std::string& replaceIn, const std::string& replace, const std::string& replaceWith);
	//extern void FindCamThinkSvCheatsCheck();
	extern CBaseEntity* GetLocalPlayer( );
	extern CNetVarManager* NetVars;
	//extern void SpoofCvar( ConVar* pCvar );
	extern wchar_t* ConvertCharArrayToLPCWSTR( const char* charArray );
	extern void ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr);
	extern void TraceLine( const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, CBaseEntity *ignore, trace_t *ptr );
	//extern unsigned short GetAchievementEventMask();
	extern void Log(const char * fmt, ...);
	extern void ServerRankRevealAll();
	extern void InitKeyValues( KeyValues* pKeyValues, const char* name );
	extern void LoadFromBuffer( KeyValues* pKeyValues, const char* resourceName, const char* pBuffer, void* pFileSystem = nullptr, const char* pPathID = NULL, void* pfnEvaluateSymbolProc = nullptr );
	extern void SendPacket( byte toggle );
	extern void SetupInterfaces();
	extern void SetupHooks();
	extern void SetupOffsets();
	extern void SetupTextures();
	extern void Setup();

	template< class T, class Y >
	T Clamp( T const &val, Y const &minVal, Y const &maxVal )
	{
		if( val < minVal )
			return minVal;
		else if( val > maxVal )
			return maxVal;
		else
			return val;
	}

	template <typename T>
	T GetVFunc( void *vTable, int iIndex ) {
		return ( *( T** )vTable )[ iIndex ];
	}

	template <typename T>
	T* CaptureInterface( std::string strModule, std::string strInterface )
	{
		typedef T* ( *CreateInterfaceFn )( const char* szName, int iReturn );
		CreateInterfaceFn CreateInterface = ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( strModule.c_str() ), charenc( "CreateInterface" ) );

		return CreateInterface( strInterface.c_str(), 0 );
	}

	extern float RandomFloat( float min, float max );
	
}

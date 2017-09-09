#include <cstdint>
#include <Windows.h>
#include "Cheat.h"
#include "rbot.h"
/*
ProtoBufMsg_ReportPlayer::ProtoBufMsg_ReportPlayer(uint64_t* steamid)
{
	static auto *pConstructor = reinterpret_cast<void(__thiscall*)(void*, uint64_t*)>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x0046C640);

	pConstructor(this, steamid);

	this->data->set_to_0xFF = 0xFF;
}

ProtoBufMsg_ReportPlayer::~ProtoBufMsg_ReportPlayer()
{
	static auto *pDeconstructor = reinterpret_cast<void(__thiscall*)(void*)>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x0046C6D0);

	pDeconstructor(this);
}

class ProtoBuf_SendHandler
{
public:
	ProtoBuf_SendHandler();
	void SendProtoBufMessage(ProtoBufMsg_ReportPlayer* message);

	void* vmtptr;
	void* gcclient;
};

ProtoBuf_SendHandler::ProtoBuf_SendHandler()
{
	static void* _vmtptr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x00988E50);
	static void* _gcclient = reinterpret_cast<void*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x02EA907C) + 0x20);

	vmtptr = _vmtptr;
	gcclient = _gcclient;
}

void ProtoBuf_SendHandler::SendProtoBufMessage(ProtoBufMsg_ReportPlayer * message)
{
	static uintptr_t pSendProtoBufMessage = reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + 0x00624E80;

	__asm
	{
		mov ecx, message
		push[ecx + 0x10]
		push[ecx + 0x08]
		mov edx, [ecx + 0x0C]
		mov ecx, this
		call pSendProtoBufMessage
		add esp, 8
	}
}

void RBOT::SubmitReport(const char* SteamId, const char* MatchId)
{
	__declspec(align(8)) ProtoBufMsg_ReportPlayer msg_reportplayer((uint64_t*)SteamId);

	msg_reportplayer.data->account_id = (uint32_t)SteamId;
	msg_reportplayer.data->matchid = (uint64_t)MatchId;

	msg_reportplayer.data->rpt_aimbot = 1;
	msg_reportplayer.data->rpt_wallhack = 1;
	msg_reportplayer.data->rpt_otherhack = 1;
	msg_reportplayer.data->rpt_grief = 1;

	ProtoBuf_SendHandler sendhandler;

	sendhandler.SendProtoBufMessage(&msg_reportplayer);
}*/
void ReportBot()
{
	//testing
}
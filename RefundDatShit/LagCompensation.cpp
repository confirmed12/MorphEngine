/*
void LagCompensation::logEntity(Entity *player)
{
	int idx = player->getIdx();
	LagRecord *m_LagRecords = this->m_LagRecord[idx];

	if (!player || !player->getLife() > 0)
	{
		for (int i = 0; i < 9; i++) // reset sim time on dead records
		{
			m_LagRecords[i].m_fSimulationTime = 0.0f;
		}
	}

	float simTime = player->getSimulTime();

	int highestRecordIdx = -1;
	float highestSimTime = 0.0f;

	for (int i = 0; i < 9; i++) // here we do some checks so we don't have invalid records and optimize things so we don't store an
	{							// entity which already contains the current sim time
		if (m_LagRecords[i].m_fSimulationTime > simTime)
			m_LagRecords[i].m_fSimulationTime = 0.0f;

		if (m_LagRecords[i].m_fSimulationTime == 0.0f)
			continue;

		if (m_LagRecords[i].m_fSimulationTime == simTime)
			return;

		if (m_LagRecords[i].m_fSimulationTime > highestSimTime)
		{
			highestRecordIdx = i;
			highestSimTime = m_LagRecords[i].m_fSimulationTime;
		}
	}

	highestRecordIdx++;
	highestRecordIdx = highestRecordIdx % 9;

	m_LagRecords[highestRecordIdx].m_bIsFixed = false;
	m_LagRecords[highestRecordIdx].m_iTargetID = idx;

	m_LagRecords[highestRecordIdx].m_vAbsOrigin = player->getAbsOrigin();
	m_LagRecords[highestRecordIdx].m_vecVelocity = player->getVel();
	m_LagRecords[highestRecordIdx].m_fSimulationTime = player->getSimulTime();
	m_LagRecords[highestRecordIdx].m_vecAngles = player->getEyeAngles();
	m_LagRecords[highestRecordIdx].m_flCycle = player->getCycle();
	m_LagRecords[highestRecordIdx].m_nSequence = player->getSequence();
	m_LagRecords[highestRecordIdx].flags = player->getFlags();
	m_LagRecords[highestRecordIdx].m_flLowerBodyYawTarget = player->getLowBodYtarg();
	m_LagRecords[highestRecordIdx].m_vecMins = player->getCollideable()->vecMins();
	m_LagRecords[highestRecordIdx].m_vecMax = player->getCollideable()->vecMax();
	m_LagRecords[highestRecordIdx].m_fFallVel = player->getFallVel();

	// commented out since giving weird results, the backtracked entity is crouching, also this is bad anim fix, currently doing without anim fix
	//for (int i = 0; i < 24; i++)
	//m_LagRecords[highestRecordIdx].m_flPoseParameter[i] = player->getPoseParams(i);
}

void LagCompensation::setEntity(Entity *player, LagRecord record)
{
	if (!player || record.m_fSimulationTime == 0.0f)
	{
		record.m_bIsFixed = false;
		return;
	}

	unsigned long g_iModelBoneCounter = **(unsigned long**)(offs.invalidateBoneCache + 10);

	*(int*)((DWORD)player + offs.writeableBones) = 0;
	*(unsigned int*)((DWORD)player + 0x2914) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
	*(unsigned int*)((DWORD)player + 0x2680) = (g_iModelBoneCounter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;

	player->setAbsOrigin(record.m_vAbsOrigin);
	*(Vector*)((DWORD)player + offs.vekVel) = record.m_vecVelocity;
	*(float*)((DWORD)player + offs.simulTime) = record.m_fSimulationTime;
	player->setAbsAngles(record.m_vecAngles);
	*(float*)((DWORD)player + offs.cycle) = record.m_flCycle;
	*(int*)((DWORD)player + offs.sequence) = record.m_nSequence;
	*(int*)((DWORD)player + offs.flags) = record.flags;
	*(float*)((DWORD)player + offs.lowBodYtarg) = record.m_flLowerBodyYawTarget;
	player->getCollideable()->vecMins() = record.m_vecMins;
	player->getCollideable()->vecMax() = record.m_vecMax;
	player->getFallVel() = record.m_fFallVel;

	//for (int i = 0; i < 24; i++)
	//	*(float*)((DWORD)player + offs.poseParams + sizeof(float) * i) = record.m_flPoseParameter[i];

	record.m_bIsFixed = true;

	p_Console->ConsoleColorPrintf(Color::LightBlue(), "backtracked %ls\n", player->getName(player->getIdx()).c_str());
}
int LagCompensation::fakeLagFix(Entity *player, int historyIdx)
{
int idx = player->getIdx();

LagRecord *m_LagRecords = this->m_LagRecord[idx];

LagRecord recentLR = m_LagRecords[historyIdx];
LagRecord prevLR;
if (historyIdx == 0)
prevLR = m_LagRecords[8];
else
prevLR = m_LagRecords[historyIdx - 1];

if (recentLR.m_fSimulationTime == 0.0f)
return -1;

INetChannelInfo *nci = p_Engine->getNetChannelInfo();
int predCmdArrivTick = Global::userCMD->tick_count + 1 + TIME_TO_TICKS(nci->GetAvgLatency(FLOW_INCOMING) + nci->GetAvgLatency(FLOW_OUTGOING)); // (c) n0xius @ uc

int iLerpTicks = TIME_TO_TICKS(lerpTime());
int iTargetTickCount = TIME_TO_TICKS(recentLR.m_fSimulationTime) + iLerpTicks;

float flCorrect = clamp(lerpTime() + nci->GetLatency(FLOW_OUTGOING), 0.f, 1.f) - TICKS_TO_TIME(predCmdArrivTick + TIME_TO_TICKS(lerpTime()) - iTargetTickCount); // (c) n0xius @ uc

if (fabs(flCorrect) > 0.2f) // Too big deltatime, can't lagfix here
{
p_Console->ConsoleColorPrintf(Color::Red(), "StartLagCompensation: delta too big (%.3f)\n", flCorrect);
return -1;
}

if ((recentLR.m_vAbsOrigin - prevLR.m_vAbsOrigin).lengthSqr() > 4096.f)
{
for (auto i = 0; i <= player->getChockedTicks(); i++)
{
rebGameMovement->FullWalkMove(player); // resimulate
}
recentLR.m_bIsBreakingLagComp = true;
p_Console->ConsoleColorPrintf(Color::Green(), "m_bIsBreakingLagComp: true\n");

return -1;
}
else
{
setEntity(player, recentLR);
}

//p_Console->ConsoleColorPrintf(Color::LightBlue(), "tickcount: %d\n", (iTargetTickCount + 1));

return iTargetTickCount + 1;
}

int LagCompensation::fixTickcount(Entity *player)
{
int idx = player->getIdx();

LagRecord *m_LagRecords = this->m_LagRecord[idx];

LagRecord recentLR = m_LagRecords[mVars.historyIdx];

if (recentLR.m_fSimulationTime == 0.0f)
return TIME_TO_TICKS(player->getSimulTime() + lerpTime()) + 1;

int iLerpTicks = TIME_TO_TICKS(lerpTime());
int iTargetTickCount = TIME_TO_TICKS(recentLR.m_fSimulationTime) + iLerpTicks;

p_Console->ConsoleColorPrintf(Color::Blue(), "tickcount: %d\n", (iTargetTickCount + 1));

return iTargetTickCount + 1;
}

*/
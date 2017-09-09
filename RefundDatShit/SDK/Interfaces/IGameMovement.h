#pragma once
#include "../../sdk.h"

class IGameMovement {
public:
	virtual			~IGameMovement(void) {}

	virtual void	ProcessMovement(CBaseEntity *pPlayer, CMoveData *pMove) = 0;
	virtual void	Reset(void) = 0;
	virtual void	StartTrackPredictionErrors(CBaseEntity *pPlayer) = 0;
	virtual void	FinishTrackPredictionErrors(CBaseEntity *pPlayer) = 0;
	virtual void	DiffPrint(char const *fmt, ...) = 0;

	virtual Vector const&	GetPlayerMins(bool ducked) const = 0;
	virtual Vector const&	GetPlayerMaxs(bool ducked) const = 0;
	virtual Vector const&   GetPlayerViewOffset(bool ducked) const = 0;

	virtual bool			IsMovingPlayerStuck(void) const = 0;
	virtual CBaseEntity*	GetMovingPlayer(void) const = 0;
	virtual void			UnblockPusher(CBaseEntity* pPlayer, CBaseEntity *pPusher) = 0;

	virtual void SetupMovementBounds(CMoveData *pMove) = 0;
};
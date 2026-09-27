#ifndef PLAYER_H
#define PLAYER_H

#include "GameCommon.h"

/* ---------- Player Variables ---------- */
extern bool   facingRight;
extern int    charFrame;
extern int    targetStartX;
extern int    charX;
extern double charY;
extern bool   isEntering;
extern bool   levelDone;
extern bool   isExiting;
extern bool   showWinCard;
extern int    playerHealth;

/* Timers & Jump */
extern int    idleTimer;
extern int    fightTimer;
extern int    runCounter;
extern bool   isJumping;
extern double jumpVelocity;
extern int    jumpFrame;
extern int    jumpTimer;

/* Player Image Handles */
extern int imgRunRight[9];
extern int imgRunLeft[9];
extern int imgFightRight[5];
extern int imgFightLeft[5];
extern int imgJump[8];
extern int imgIdle;
extern int imgIdleLeft;

/* Special Powers (N - Army Summon, M - Rectangular Manipulation) */
extern double nCooldown;
extern double mCooldown;
extern int    manipulationEffectTimer;
extern double manipulationEffectX;
extern bool   manipulationFacingRight;
extern int    heroSoldierKillCount;

/* ---------- Player Actions ---------- */
void castNPower();
void castMPower();

void jump()
{
	if (levelDone || isEntering || isExiting || playerHealth <= 0 || isPaused) return;
	if (currentState == FIGHT_RIGHT || currentState == FIGHT_LEFT) return;
	idleTimer = 0;
	if (!isJumping)
	{
		isJumping = true;
		jumpVelocity = JUMP_POWER;
		jumpFrame = 0;
		jumpTimer = 0;
		if (currentState == SIT) currentState = IDLE;
	}
}

#endif
#ifndef GAME_COMMON_H
#define GAME_COMMON_H

#include <cstdio>
#include <windows.h>
#include <math.h>
#include "iGraphics.h"

/* ---------- Screen & World Constants ---------- */
#define SCREEN_W     1280
#define SCREEN_H     720
#define TILE_W       1280
#define TILE_H       720
#define MOVE_SPEED   16   /* Charecter running speed */   
#define NUM_TILES    8
#define GROUND_Y     75       /* character ground level */
#define JUMP_POWER   16.5     /* initial jump velocity (tuned for lower jump height) */
#define GRAVITY      0.65     /* gravity */

/* ---------- Enums ---------- */
enum GameScreen {
	SCREEN_SPLASH,
	SCREEN_MENU,
	SCREEN_GAME,
	SCREEN_OPTIONS,
	SCREEN_CREDITS,
	SCREEN_STORY,
	SCREEN_END_CARD
};

enum CharState {
	IDLE,
	RUN_RIGHT,
	RUN_LEFT,
	FIGHT_RIGHT,
	FIGHT_LEFT,
	SIT
};

/* ---------- Global State Variables (Extern) ---------- */
extern GameScreen currentScreen;
extern CharState  currentState;
extern bool       isPaused;
extern int        currentLevel;
extern int        storyIndex;

/* ---------- Background & Image Handles ---------- */
extern int bg[5];
extern int tileSeq[NUM_TILES];
extern double bgOffset;
extern double maxOffset;
extern int imgYouWin[3];
extern int youWinFrame;
extern int imgStory[4];
extern int imgBL1;
extern int imgBL2;
extern int imgBL3;
extern int lvl3Stage;

/* ---------- Health Card & Progress UI Handles ---------- */
extern int imgHealthTim;
extern int imgHealthMesh;
extern int imgHealthSaint;
extern int imgHealthArcher;
extern int imgHealthSoldier;
extern int imgProgressBar;

/* ---------- Function Declarations ---------- */
void startLevel(int level);
void restartGame();

#endif
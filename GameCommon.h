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

/* ---------- Text Measurement & UI Alignment Helpers ---------- */
inline int iGetTextWidth(const char* str, void* font = GLUT_BITMAP_8_BY_13) {
	if (!str) return 0;
	int len = 0;
	for (int i = 0; str[i] != '\0'; i++) {
		len += glutBitmapWidth(font, (unsigned char)str[i]);
	}
	return len;
}

inline int iGetFontHeight(void* font) {
	if (font == GLUT_BITMAP_TIMES_ROMAN_24) return 24;
	if (font == GLUT_BITMAP_HELVETICA_18) return 18;
	if (font == GLUT_BITMAP_HELVETICA_12) return 12;
	if (font == GLUT_BITMAP_HELVETICA_10) return 10;
	if (font == GLUT_BITMAP_9_BY_15) return 15;
	if (font == GLUT_BITMAP_8_BY_13) return 13;
	if (font == GLUT_BITMAP_TIMES_ROMAN_10) return 10;
	return 13;
}

inline void iDrawCenteredText(double centerX, double baselineY, const char* str, void* font = GLUT_BITMAP_8_BY_13) {
	if (!str || str[0] == '\0') return;
	int w = iGetTextWidth(str, font);
	iText(centerX - (w / 2.0), baselineY, (char*)str, font);
}

inline void iDrawCenteredTextInRect(double x1, double y1, double x2, double y2, const char* str, void* font = GLUT_BITMAP_8_BY_13, double yOffset = 0.0) {
	if (!str || str[0] == '\0') return;
	double cx = (x1 + x2) / 2.0;
	double cy = (y1 + y2) / 2.0;
	int fh = iGetFontHeight(font);
	double baselineY = cy - (fh * 0.35) + yOffset;
	int w = iGetTextWidth(str, font);
	iText(cx - (w / 2.0), baselineY, (char*)str, font);
}

inline void iDrawCenteredTextInBox(double boxX, double boxY, double boxW, double boxH, const char* str, void* font = GLUT_BITMAP_8_BY_13, double yOffset = 0.0) {
	iDrawCenteredTextInRect(boxX, boxY, boxX + boxW, boxY + boxH, str, font, yOffset);
}

inline void iDrawCenteredNumber(double cx, double cy, int number, void* font = GLUT_BITMAP_TIMES_ROMAN_24) {
	char numBuf[32];
	sprintf_s(numBuf, "%d", number);
	int fh = iGetFontHeight(font);
	double baselineY = cy - (fh * 0.35);
	int w = iGetTextWidth(numBuf, font);
	iText(cx - (w / 2.0), baselineY, numBuf, font);
}

/* Dedicated Left-Side Hero/Team Health Card Dynamic Renderer */
inline void drawHeroHealthCard(int imgCard, double cardX, double cardY, int cardW, int cardH, int curHp, int maxHp, void* font = GLUT_BITMAP_TIMES_ROMAN_24) {
	if (imgCard != 0) {
		iShowImage((int)cardX, (int)cardY, cardW, cardH, imgCard);
	}

	char hpText[32];
	if (curHp < 0) curHp = 0;
	sprintf_s(hpText, "%d / %d", curHp, maxHp);

	// Hero Card (Tim): Portrait is on left, Health bar container is on right.
	// Geometry: Center X is 62.0% of card width, Center Y is 50.5% of card height
	double heroHealthBarCenterX = cardX + (cardW * 0.620);
	double heroHealthBarCenterY = cardY + (cardH * 0.505);

	int fh = iGetFontHeight(font);
	double baselineY = heroHealthBarCenterY - (fh * 0.35);
	int textW = iGetTextWidth(hpText, font);

	iSetColor(245, 30, 30);
	iText(heroHealthBarCenterX - (textW / 2.0), baselineY, hpText, font);
}

/* Dedicated Right-Side Villain/Boss Health Card Dynamic Renderer (Saint, Mesh, Villains) */
inline void drawRightSideHealthCard(int imgCard, double cardX, double cardY, int cardW, int cardH, int curHp, int maxHp, void* font = GLUT_BITMAP_TIMES_ROMAN_24) {
	if (imgCard != 0) {
		iShowImage((int)cardX, (int)cardY, cardW, cardH, imgCard);
	}

	char hpText[32];
	if (curHp < 0) curHp = 0;
	sprintf_s(hpText, "%d / %d", curHp, maxHp);

	// Right-Side Villain Cards (Saint, Mesh): Portrait is on right, Health bar container is on left.
	// Actual geometry analysis:
	// Health bar X is [20, 1340] in 1916px card -> Center X = 680 / 1916 = 35.5% of card width
	// Health bar Y is [370, 535] in 821px card -> OpenGL Center Y = (821 - 452.5) / 821 = 44.9% of card height
	double rightHealthBarCenterX = cardX + (cardW * 0.355);
	double rightHealthBarCenterY = cardY + (cardH * 0.449);

	int fh = iGetFontHeight(font);
	double baselineY = rightHealthBarCenterY - (fh * 0.35);
	int textW = iGetTextWidth(hpText, font);

	iSetColor(245, 30, 30);
	iText(rightHealthBarCenterX - (textW / 2.0), baselineY, hpText, font);
}

/* Reusable Health Card Dynamic Centering Helper */
inline void drawCenteredHealthText(double cardX, double cardY, int cardW, int cardH, bool isHero, int curHp, int maxHp, void* font = GLUT_BITMAP_TIMES_ROMAN_24) {
	if (isHero) {
		drawHeroHealthCard(0, cardX, cardY, cardW, cardH, curHp, maxHp, font);
	}
	else {
		drawRightSideHealthCard(0, cardX, cardY, cardW, cardH, curHp, maxHp, font);
	}
}

/* ---------- Function Declarations ---------- */
void startLevel(int level);
void restartGame();

#endif
#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include "GameCommon.h"
#include "AudioManager.h"
#include "SaveSystem.h"

/* Images */
extern int imgSplash;   /* Images/splash.png */
extern int imgMenu;     /* Images/Home.png */
extern int imgGameOver; /* Images/Game over.png */

/* Main Menu Button Coordinates */
#define BTN_START_X1 350
#define BTN_START_X2 730
#define BTN_START_Y1 435
#define BTN_START_Y2 545

#define BTN_OPTIONS_X1 350
#define BTN_OPTIONS_X2 730
#define BTN_OPTIONS_Y1 320
#define BTN_OPTIONS_Y2 425

#define BTN_CREDITS_X1 350
#define BTN_CREDITS_X2 730
#define BTN_CREDITS_Y1 205
#define BTN_CREDITS_Y2 310

#define BTN_EXIT_X1 370
#define BTN_EXIT_X2 750
#define BTN_EXIT_Y1 75
#define BTN_EXIT_Y2 180

#define BTN_BACK_X1 460
#define BTN_BACK_X2 820
#define BTN_BACK_Y1 60
#define BTN_BACK_Y2 150

/* Story Screen Buttons */
#define BTN_STORY_SKIP_X1 50
#define BTN_STORY_SKIP_X2 220
#define BTN_STORY_SKIP_Y1 35
#define BTN_STORY_SKIP_Y2 95

#define BTN_STORY_NEXT_X1 1060
#define BTN_STORY_NEXT_X2 1230
#define BTN_STORY_NEXT_Y1 35
#define BTN_STORY_NEXT_Y2 95

/* End Card Button */
#define BTN_END_HOME_X1 480
#define BTN_END_HOME_X2 800
#define BTN_END_HOME_Y1 120
#define BTN_END_HOME_Y2 190

/* Options Screen Interactive Controls */
#define BTN_OPT_MUSIC_X1 380
#define BTN_OPT_MUSIC_X2 900
#define BTN_OPT_MUSIC_Y1 485
#define BTN_OPT_MUSIC_Y2 545

#define BTN_OPT_SOUND_X1 380
#define BTN_OPT_SOUND_X2 900
#define BTN_OPT_SOUND_Y1 410
#define BTN_OPT_SOUND_Y2 470

#define BTN_OPT_PREV_X1  170
#define BTN_OPT_PREV_X2  250
#define BTN_OPT_PREV_Y1  275
#define BTN_OPT_PREV_Y2  345

#define BTN_OPT_P1_X1    270
#define BTN_OPT_P1_X2    450
#define BTN_OPT_P1_Y1    275
#define BTN_OPT_P1_Y2    345

#define BTN_OPT_P2_X1    470
#define BTN_OPT_P2_X2    650
#define BTN_OPT_P2_Y1    275
#define BTN_OPT_P2_Y2    345

#define BTN_OPT_P3_X1    670
#define BTN_OPT_P3_X2    850
#define BTN_OPT_P3_Y1    275
#define BTN_OPT_P3_Y2    345

#define BTN_OPT_NEXT_X1  870
#define BTN_OPT_NEXT_X2  950
#define BTN_OPT_NEXT_Y1  275
#define BTN_OPT_NEXT_Y2  345

#define BTN_OPT_ADD_X1   970
#define BTN_OPT_ADD_X2   1110
#define BTN_OPT_ADD_Y1   275
#define BTN_OPT_ADD_Y2   345

#define BTN_OPT_RENAME_X1 890
#define BTN_OPT_RENAME_X2 1060
#define BTN_OPT_RENAME_Y1 215
#define BTN_OPT_RENAME_Y2 243

#define BTN_MODAL_CONFIRM_X1 380
#define BTN_MODAL_CONFIRM_X2 610
#define BTN_MODAL_CONFIRM_Y1 220
#define BTN_MODAL_CONFIRM_Y2 275

#define BTN_MODAL_CANCEL_X1  670
#define BTN_MODAL_CANCEL_X2  900
#define BTN_MODAL_CANCEL_Y1  220
#define BTN_MODAL_CANCEL_Y2  275

#define BTN_OPT_BACK_X1  460
#define BTN_OPT_BACK_X2  820
#define BTN_OPT_BACK_Y1  35
#define BTN_OPT_BACK_Y2  95

extern int optionsPlayerPage;
extern bool isNamingPlayer;
extern char nameInputBuffer[32];
extern int  nameInputLen;
extern int  namingTargetSlot;
extern int  nameInputBlinkTimer;

void drawSplash()
{
	iShowImage(0, 0, SCREEN_W, SCREEN_H, imgSplash);
	iSetColor(255, 255, 255);
	iText(SCREEN_W / 2 - 175, 255,
		"Please click ENTER to open the game.",
		GLUT_BITMAP_HELVETICA_18);
}

void drawMenu()
{
	iShowImage(0, 0, SCREEN_W, SCREEN_H, imgMenu);

	/* Interactive Name Input Dialog upon clicking "START GAME" */
	if (isNamingPlayer && currentScreen == SCREEN_MENU) {
		// Dark background overlay
		iSetColor(6, 8, 16);
		iFilledRectangle(300, 140, 680, 420);

		// Glowing Border
		iSetColor(255, 215, 0);
		iRectangle(300, 140, 680, 420);
		iRectangle(303, 143, 674, 414);

		// Header
		iSetColor(255, 215, 0);
		iText(SCREEN_W / 2 - 200, 510, (char*)"ENTER HERO / PLAYER NAME", GLUT_BITMAP_TIMES_ROMAN_24);

		iSetColor(100, 200, 255);
		iText(340, 475, (char*)"Type your player name to begin the journey:", GLUT_BITMAP_HELVETICA_12);

		// Input Box
		iSetColor(20, 26, 45);
		iFilledRectangle(340, 390, 600, 60);
		iSetColor(100, 220, 255);
		iRectangle(340, 390, 600, 60);
		iRectangle(342, 392, 596, 56);

		// Text & Cursor
		nameInputBlinkTimer++;
		bool showCursor = ((nameInputBlinkTimer / 25) % 2 == 0);

		if (nameInputLen == 0) {
			if (showCursor) {
				iSetColor(255, 215, 0);
				iText(360, 412, (char*)"|", GLUT_BITMAP_TIMES_ROMAN_24);
			}
			iSetColor(120, 140, 170);
			iText(375, 412, (char*)"Type your name here...", GLUT_BITMAP_HELVETICA_18);
		}
		else {
			char displayBuf[64];
			if (showCursor) {
				sprintf_s(displayBuf, "%s|", nameInputBuffer);
			}
			else {
				sprintf_s(displayBuf, "%s", nameInputBuffer);
			}
			iSetColor(255, 255, 255);
			iText(360, 412, displayBuf, GLUT_BITMAP_TIMES_ROMAN_24);
		}

		// Length & instructions
		char charInfo[128];
		sprintf_s(charInfo, "Length: %d / 24 chars  |  Supports letters, numbers, spaces & symbols", nameInputLen);
		iSetColor(160, 190, 220);
		iText(340, 360, charInfo, GLUT_BITMAP_HELVETICA_10);

		iSetColor(255, 230, 150);
		iText(340, 330, (char*)"Press ENTER to Start Game  |  Press ESC to Cancel", GLUT_BITMAP_HELVETICA_12);

		// START GAME CONFIRM BUTTON
		iSetColor(20, 75, 40);
		iFilledRectangle(BTN_MODAL_CONFIRM_X1, BTN_MODAL_CONFIRM_Y1,
			BTN_MODAL_CONFIRM_X2 - BTN_MODAL_CONFIRM_X1, BTN_MODAL_CONFIRM_Y2 - BTN_MODAL_CONFIRM_Y1);
		iSetColor(50, 255, 120);
		iRectangle(BTN_MODAL_CONFIRM_X1, BTN_MODAL_CONFIRM_Y1,
			BTN_MODAL_CONFIRM_X2 - BTN_MODAL_CONFIRM_X1, BTN_MODAL_CONFIRM_Y2 - BTN_MODAL_CONFIRM_Y1);
		iSetColor(255, 255, 255);
		iText(BTN_MODAL_CONFIRM_X1 + 30, BTN_MODAL_CONFIRM_Y1 + 20, (char*)"START GAME [ENTER]", GLUT_BITMAP_HELVETICA_12);

		// CANCEL BUTTON
		iSetColor(75, 20, 30);
		iFilledRectangle(BTN_MODAL_CANCEL_X1, BTN_MODAL_CANCEL_Y1,
			BTN_MODAL_CANCEL_X2 - BTN_MODAL_CANCEL_X1, BTN_MODAL_CANCEL_Y2 - BTN_MODAL_CANCEL_Y1);
		iSetColor(255, 70, 70);
		iRectangle(BTN_MODAL_CANCEL_X1, BTN_MODAL_CANCEL_Y1,
			BTN_MODAL_CANCEL_X2 - BTN_MODAL_CANCEL_X1, BTN_MODAL_CANCEL_Y2 - BTN_MODAL_CANCEL_Y1);
		iSetColor(255, 255, 255);
		iText(BTN_MODAL_CANCEL_X1 + 45, BTN_MODAL_CANCEL_Y1 + 20, (char*)"CANCEL [ESC]", GLUT_BITMAP_HELVETICA_12);
	}
}

void drawOptions()
{
	/* Background - Dark premium sci-fi/fantasy backdrop */
	iSetColor(12, 14, 26);
	iFilledRectangle(0, 0, SCREEN_W, SCREEN_H);

	/* Decorative Outer & Inner Golden Borders */
	iSetColor(255, 215, 0);
	iRectangle(35, 25, SCREEN_W - 70, SCREEN_H - 50);
	iRectangle(39, 29, SCREEN_W - 78, SCREEN_H - 58);

	/* Title */
	iSetColor(255, 215, 0);
	iText(SCREEN_W / 2 - 215, 650,
		(char*)"GAME OPTIONS & PROFILE SETTINGS",
		GLUT_BITMAP_TIMES_ROMAN_24);

	/* Subtitle */
	iSetColor(100, 200, 255);
	iText(SCREEN_W / 2 - 175, 620,
		(char*)"DYNAMIC PLAYER PROFILES & AUDIO CONTROLS",
		GLUT_BITMAP_HELVETICA_18);

	/* Accent Line Under Title */
	iSetColor(255, 215, 0);
	iLine(SCREEN_W / 2 - 250, 605, SCREEN_W / 2 + 250, 605);

	/* ================= 1. MUSIC TOGGLE BUTTON ================= */
	bool musicOn = gAudio.isMusicEnabled();
	iSetColor(22, 26, 45);
	iFilledRectangle(BTN_OPT_MUSIC_X1, BTN_OPT_MUSIC_Y1,
		BTN_OPT_MUSIC_X2 - BTN_OPT_MUSIC_X1, BTN_OPT_MUSIC_Y2 - BTN_OPT_MUSIC_Y1);
	iSetColor(musicOn ? 50 : 255, musicOn ? 255 : 60, musicOn ? 100 : 60);
	iRectangle(BTN_OPT_MUSIC_X1, BTN_OPT_MUSIC_Y1,
		BTN_OPT_MUSIC_X2 - BTN_OPT_MUSIC_X1, BTN_OPT_MUSIC_Y2 - BTN_OPT_MUSIC_Y1);
	iRectangle(BTN_OPT_MUSIC_X1 + 2, BTN_OPT_MUSIC_Y1 + 2,
		BTN_OPT_MUSIC_X2 - BTN_OPT_MUSIC_X1 - 4, BTN_OPT_MUSIC_Y2 - BTN_OPT_MUSIC_Y1 - 4);

	iSetColor(255, 255, 255);
	iText(BTN_OPT_MUSIC_X1 + 25, BTN_OPT_MUSIC_Y1 + 34, (char*)"BACKGROUND MUSIC [M] :", GLUT_BITMAP_HELVETICA_18);

	if (musicOn) {
		iSetColor(50, 255, 120);
		iText(BTN_OPT_MUSIC_X1 + 340, BTN_OPT_MUSIC_Y1 + 34, (char*)"[ ON ]", GLUT_BITMAP_TIMES_ROMAN_24);
	}
	else {
		iSetColor(255, 70, 70);
		iText(BTN_OPT_MUSIC_X1 + 340, BTN_OPT_MUSIC_Y1 + 34, (char*)"[ OFF ]", GLUT_BITMAP_TIMES_ROMAN_24);
	}
	iSetColor(170, 185, 210);
	iText(BTN_OPT_MUSIC_X1 + 25, BTN_OPT_MUSIC_Y1 + 12, (char*)"Controls Game BG & Gameplay Background Music", GLUT_BITMAP_HELVETICA_12);

	/* ================= 2. SOUND SFX TOGGLE BUTTON ================= */
	bool soundOn = gAudio.isSoundEnabled();
	iSetColor(22, 26, 45);
	iFilledRectangle(BTN_OPT_SOUND_X1, BTN_OPT_SOUND_Y1,
		BTN_OPT_SOUND_X2 - BTN_OPT_SOUND_X1, BTN_OPT_SOUND_Y2 - BTN_OPT_SOUND_Y1);
	iSetColor(soundOn ? 50 : 255, soundOn ? 255 : 60, soundOn ? 100 : 60);
	iRectangle(BTN_OPT_SOUND_X1, BTN_OPT_SOUND_Y1,
		BTN_OPT_SOUND_X2 - BTN_OPT_SOUND_X1, BTN_OPT_SOUND_Y2 - BTN_OPT_SOUND_Y1);
	iRectangle(BTN_OPT_SOUND_X1 + 2, BTN_OPT_SOUND_Y1 + 2,
		BTN_OPT_SOUND_X2 - BTN_OPT_SOUND_X1 - 4, BTN_OPT_SOUND_Y2 - BTN_OPT_SOUND_Y1 - 4);

	iSetColor(255, 255, 255);
	iText(BTN_OPT_SOUND_X1 + 25, BTN_OPT_SOUND_Y1 + 34, (char*)"SOUND EFFECTS (SFX) [S] :", GLUT_BITMAP_HELVETICA_18);

	if (soundOn) {
		iSetColor(50, 255, 120);
		iText(BTN_OPT_SOUND_X1 + 340, BTN_OPT_SOUND_Y1 + 34, (char*)"[ ON ]", GLUT_BITMAP_TIMES_ROMAN_24);
	}
	else {
		iSetColor(255, 70, 70);
		iText(BTN_OPT_SOUND_X1 + 340, BTN_OPT_SOUND_Y1 + 34, (char*)"[ OFF ]", GLUT_BITMAP_TIMES_ROMAN_24);
	}
	iSetColor(170, 185, 210);
	iText(BTN_OPT_SOUND_X1 + 25, BTN_OPT_SOUND_Y1 + 12, (char*)"Controls Hero Heart, Sword Clashes & Enemy Action SFX", GLUT_BITMAP_HELVETICA_12);

	/* ================= 3. DYNAMIC PLAYER PROFILE SELECTION ================= */
	int activeIdx = gSaveSystem.getActivePlayerIndex();
	int totalPlayers = gSaveSystem.getNumPlayers();
	if (optionsPlayerPage < 0) optionsPlayerPage = 0;
	if (optionsPlayerPage * 3 >= totalPlayers) {
		optionsPlayerPage = (totalPlayers > 0) ? (totalPlayers - 1) / 3 : 0;
	}
	int startIdx = optionsPlayerPage * 3;

	iSetColor(255, 215, 0);
	char profileHeader[256];
	sprintf_s(profileHeader, "SELECT DYNAMIC PLAYER PROFILE (Page %d/%d | Total: %d Players)",
		optionsPlayerPage + 1, (totalPlayers + 2) / 3, totalPlayers);
	iText(SCREEN_W / 2 - 200, 362, profileHeader, GLUT_BITMAP_HELVETICA_12);

	// < PREV BUTTON
	iSetColor(22, 26, 45);
	iFilledRectangle(BTN_OPT_PREV_X1, BTN_OPT_PREV_Y1, BTN_OPT_PREV_X2 - BTN_OPT_PREV_X1, BTN_OPT_PREV_Y2 - BTN_OPT_PREV_Y1);
	iSetColor(optionsPlayerPage > 0 ? 255 : 80, optionsPlayerPage > 0 ? 215 : 80, optionsPlayerPage > 0 ? 0 : 80);
	iRectangle(BTN_OPT_PREV_X1, BTN_OPT_PREV_Y1, BTN_OPT_PREV_X2 - BTN_OPT_PREV_X1, BTN_OPT_PREV_Y2 - BTN_OPT_PREV_Y1);
	iSetColor(optionsPlayerPage > 0 ? 255 : 100, optionsPlayerPage > 0 ? 255 : 100, optionsPlayerPage > 0 ? 255 : 100);
	iText(BTN_OPT_PREV_X1 + 15, BTN_OPT_PREV_Y1 + 28, (char*)"< PREV", GLUT_BITMAP_HELVETICA_12);

	// Slot 1
	int slot1Idx = startIdx;
	if (slot1Idx < totalPlayers) {
		bool isActive = (activeIdx == slot1Idx);
		iSetColor(22, 26, 45);
		iFilledRectangle(BTN_OPT_P1_X1, BTN_OPT_P1_Y1, BTN_OPT_P1_X2 - BTN_OPT_P1_X1, BTN_OPT_P1_Y2 - BTN_OPT_P1_Y1);
		iSetColor(isActive ? 255 : 120, isActive ? 215 : 130, isActive ? 0 : 150);
		iRectangle(BTN_OPT_P1_X1, BTN_OPT_P1_Y1, BTN_OPT_P1_X2 - BTN_OPT_P1_X1, BTN_OPT_P1_Y2 - BTN_OPT_P1_Y1);
		if (isActive) {
			iRectangle(BTN_OPT_P1_X1 + 2, BTN_OPT_P1_Y1 + 2, BTN_OPT_P1_X2 - BTN_OPT_P1_X1 - 4, BTN_OPT_P1_Y2 - BTN_OPT_P1_Y1 - 4);
			iSetColor(255, 215, 0);
		}
		else {
			iSetColor(200, 200, 220);
		}
		iText(BTN_OPT_P1_X1 + 20, BTN_OPT_P1_Y1 + 44, (char*)gSaveSystem.getPlayer(slot1Idx).name, GLUT_BITMAP_HELVETICA_18);
		char sText1[64];
		sprintf_s(sText1, "Score: %d %s", gSaveSystem.getPlayer(slot1Idx).score, isActive ? "[ACTIVE]" : "");
		iSetColor(isActive ? 50 : 120, isActive ? 255 : 220, isActive ? 120 : 255);
		iText(BTN_OPT_P1_X1 + 20, BTN_OPT_P1_Y1 + 18, sText1, GLUT_BITMAP_HELVETICA_12);
	}
	else if (slot1Idx == totalPlayers && totalPlayers < MAX_SAVED_PLAYERS) {
		// Empty slot: Add prompt
		iSetColor(18, 22, 38);
		iFilledRectangle(BTN_OPT_P1_X1, BTN_OPT_P1_Y1, BTN_OPT_P1_X2 - BTN_OPT_P1_X1, BTN_OPT_P1_Y2 - BTN_OPT_P1_Y1);
		iSetColor(60, 160, 100);
		iRectangle(BTN_OPT_P1_X1, BTN_OPT_P1_Y1, BTN_OPT_P1_X2 - BTN_OPT_P1_X1, BTN_OPT_P1_Y2 - BTN_OPT_P1_Y1);
		iSetColor(80, 230, 130);
		iText(BTN_OPT_P1_X1 + 35, BTN_OPT_P1_Y1 + 28, (char*)"+ NEW PROFILE", GLUT_BITMAP_HELVETICA_12);
	}

	// Slot 2
	int slot2Idx = startIdx + 1;
	if (slot2Idx < totalPlayers) {
		bool isActive = (activeIdx == slot2Idx);
		iSetColor(22, 26, 45);
		iFilledRectangle(BTN_OPT_P2_X1, BTN_OPT_P2_Y1, BTN_OPT_P2_X2 - BTN_OPT_P2_X1, BTN_OPT_P2_Y2 - BTN_OPT_P2_Y1);
		iSetColor(isActive ? 255 : 120, isActive ? 215 : 130, isActive ? 0 : 150);
		iRectangle(BTN_OPT_P2_X1, BTN_OPT_P2_Y1, BTN_OPT_P2_X2 - BTN_OPT_P2_X1, BTN_OPT_P2_Y2 - BTN_OPT_P2_Y1);
		if (isActive) {
			iRectangle(BTN_OPT_P2_X1 + 2, BTN_OPT_P2_Y1 + 2, BTN_OPT_P2_X2 - BTN_OPT_P2_X1 - 4, BTN_OPT_P2_Y2 - BTN_OPT_P2_Y1 - 4);
			iSetColor(255, 215, 0);
		}
		else {
			iSetColor(200, 200, 220);
		}
		iText(BTN_OPT_P2_X1 + 20, BTN_OPT_P2_Y1 + 44, (char*)gSaveSystem.getPlayer(slot2Idx).name, GLUT_BITMAP_HELVETICA_18);
		char sText2[64];
		sprintf_s(sText2, "Score: %d %s", gSaveSystem.getPlayer(slot2Idx).score, isActive ? "[ACTIVE]" : "");
		iSetColor(isActive ? 50 : 120, isActive ? 255 : 220, isActive ? 120 : 255);
		iText(BTN_OPT_P2_X1 + 20, BTN_OPT_P2_Y1 + 18, sText2, GLUT_BITMAP_HELVETICA_12);
	}
	else if (slot2Idx == totalPlayers && totalPlayers < MAX_SAVED_PLAYERS) {
		// Empty slot: Add prompt
		iSetColor(18, 22, 38);
		iFilledRectangle(BTN_OPT_P2_X1, BTN_OPT_P2_Y1, BTN_OPT_P2_X2 - BTN_OPT_P2_X1, BTN_OPT_P2_Y2 - BTN_OPT_P2_Y1);
		iSetColor(60, 160, 100);
		iRectangle(BTN_OPT_P2_X1, BTN_OPT_P2_Y1, BTN_OPT_P2_X2 - BTN_OPT_P2_X1, BTN_OPT_P2_Y2 - BTN_OPT_P2_Y1);
		iSetColor(80, 230, 130);
		iText(BTN_OPT_P2_X1 + 35, BTN_OPT_P2_Y1 + 28, (char*)"+ NEW PROFILE", GLUT_BITMAP_HELVETICA_12);
	}

	// Slot 3
	int slot3Idx = startIdx + 2;
	if (slot3Idx < totalPlayers) {
		bool isActive = (activeIdx == slot3Idx);
		iSetColor(22, 26, 45);
		iFilledRectangle(BTN_OPT_P3_X1, BTN_OPT_P3_Y1, BTN_OPT_P3_X2 - BTN_OPT_P3_X1, BTN_OPT_P3_Y2 - BTN_OPT_P3_Y1);
		iSetColor(isActive ? 255 : 120, isActive ? 215 : 130, isActive ? 0 : 150);
		iRectangle(BTN_OPT_P3_X1, BTN_OPT_P3_Y1, BTN_OPT_P3_X2 - BTN_OPT_P3_X1, BTN_OPT_P3_Y2 - BTN_OPT_P3_Y1);
		if (isActive) {
			iRectangle(BTN_OPT_P3_X1 + 2, BTN_OPT_P3_Y1 + 2, BTN_OPT_P3_X2 - BTN_OPT_P3_X1 - 4, BTN_OPT_P3_Y2 - BTN_OPT_P3_Y1 - 4);
			iSetColor(255, 215, 0);
		}
		else {
			iSetColor(200, 200, 220);
		}
		iText(BTN_OPT_P3_X1 + 20, BTN_OPT_P3_Y1 + 44, (char*)gSaveSystem.getPlayer(slot3Idx).name, GLUT_BITMAP_HELVETICA_18);
		char sText3[64];
		sprintf_s(sText3, "Score: %d %s", gSaveSystem.getPlayer(slot3Idx).score, isActive ? "[ACTIVE]" : "");
		iSetColor(isActive ? 50 : 120, isActive ? 255 : 220, isActive ? 120 : 255);
		iText(BTN_OPT_P3_X1 + 20, BTN_OPT_P3_Y1 + 18, sText3, GLUT_BITMAP_HELVETICA_12);
	}
	else if (slot3Idx == totalPlayers && totalPlayers < MAX_SAVED_PLAYERS) {
		// Empty slot: Add prompt
		iSetColor(18, 22, 38);
		iFilledRectangle(BTN_OPT_P3_X1, BTN_OPT_P3_Y1, BTN_OPT_P3_X2 - BTN_OPT_P3_X1, BTN_OPT_P3_Y2 - BTN_OPT_P3_Y1);
		iSetColor(60, 160, 100);
		iRectangle(BTN_OPT_P3_X1, BTN_OPT_P3_Y1, BTN_OPT_P3_X2 - BTN_OPT_P3_X1, BTN_OPT_P3_Y2 - BTN_OPT_P3_Y1);
		iSetColor(80, 230, 130);
		iText(BTN_OPT_P3_X1 + 35, BTN_OPT_P3_Y1 + 28, (char*)"+ NEW PROFILE", GLUT_BITMAP_HELVETICA_12);
	}

	// NEXT > BUTTON
	bool hasNext = ((optionsPlayerPage + 1) * 3 < totalPlayers);
	iSetColor(22, 26, 45);
	iFilledRectangle(BTN_OPT_NEXT_X1, BTN_OPT_NEXT_Y1, BTN_OPT_NEXT_X2 - BTN_OPT_NEXT_X1, BTN_OPT_NEXT_Y2 - BTN_OPT_NEXT_Y1);
	iSetColor(hasNext ? 255 : 80, hasNext ? 215 : 80, hasNext ? 0 : 80);
	iRectangle(BTN_OPT_NEXT_X1, BTN_OPT_NEXT_Y1, BTN_OPT_NEXT_X2 - BTN_OPT_NEXT_X1, BTN_OPT_NEXT_Y2 - BTN_OPT_NEXT_Y1);
	iSetColor(hasNext ? 255 : 100, hasNext ? 255 : 100, hasNext ? 255 : 100);
	iText(BTN_OPT_NEXT_X1 + 15, BTN_OPT_NEXT_Y1 + 28, (char*)"NEXT >", GLUT_BITMAP_HELVETICA_12);

	// [+ ADD PLAYER] BUTTON
	bool canAdd = (totalPlayers < MAX_SAVED_PLAYERS);
	iSetColor(25, 45, 35);
	iFilledRectangle(BTN_OPT_ADD_X1, BTN_OPT_ADD_Y1, BTN_OPT_ADD_X2 - BTN_OPT_ADD_X1, BTN_OPT_ADD_Y2 - BTN_OPT_ADD_Y1);
	iSetColor(canAdd ? 50 : 80, canAdd ? 255 : 80, canAdd ? 120 : 80);
	iRectangle(BTN_OPT_ADD_X1, BTN_OPT_ADD_Y1, BTN_OPT_ADD_X2 - BTN_OPT_ADD_X1, BTN_OPT_ADD_Y2 - BTN_OPT_ADD_Y1);
	iSetColor(canAdd ? 255 : 120, canAdd ? 255 : 120, canAdd ? 255 : 120);
	iText(BTN_OPT_ADD_X1 + 14, BTN_OPT_ADD_Y1 + 38, (char*)"+ ADD PLAYER", GLUT_BITMAP_HELVETICA_12);
	iSetColor(120, 220, 160);
	iText(BTN_OPT_ADD_X1 + 28, BTN_OPT_ADD_Y1 + 16, (char*)"[Press +]", GLUT_BITMAP_HELVETICA_10);

	/* Active Player Statistics Card */
	iSetColor(16, 20, 36);
	iFilledRectangle(200, 120, 880, 130);
	iSetColor(70, 90, 130);
	iRectangle(200, 120, 880, 130);

	if (totalPlayers > 0) {
		const PlayerRecord& curP = gSaveSystem.getActivePlayer();
		iSetColor(255, 215, 0);
		char statTitle[256];
		sprintf_s(statTitle, "%s Active Statistics & Progress (Binary Save: data/savegame.bin)", curP.name);
		iText(220, 224, statTitle, GLUT_BITMAP_HELVETICA_12);

		// [RENAME PLAYER] Button on Stats Card
		iSetColor(35, 55, 90);
		iFilledRectangle(BTN_OPT_RENAME_X1, BTN_OPT_RENAME_Y1,
			BTN_OPT_RENAME_X2 - BTN_OPT_RENAME_X1, BTN_OPT_RENAME_Y2 - BTN_OPT_RENAME_Y1);
		iSetColor(100, 200, 255);
		iRectangle(BTN_OPT_RENAME_X1, BTN_OPT_RENAME_Y1,
			BTN_OPT_RENAME_X2 - BTN_OPT_RENAME_X1, BTN_OPT_RENAME_Y2 - BTN_OPT_RENAME_Y1);
		iSetColor(255, 255, 255);
		iText(BTN_OPT_RENAME_X1 + 15, BTN_OPT_RENAME_Y1 + 8, (char*)"RENAME [R]", GLUT_BITMAP_HELVETICA_12);

		iSetColor(240, 245, 255);
		char statLine1[256];
		sprintf_s(statLine1, "Total Score (Valid Kills): %d   |   Highest Level Reached: Level %d   |   Total Defeated: %d",
			curP.score, curP.highestLevel, curP.totalKills);
		iText(220, 198, statLine1, GLUT_BITMAP_HELVETICA_12);

		char statLine2[256];
		sprintf_s(statLine2, "Progress -> Level 1: %d%% %s | Level 2: %d%% %s | Level 3: %d%% %s | Level 4: %d%% %s",
			curP.levelProgressPct[0], curP.levelCompleted[0] ? "[DONE]" : "",
			curP.levelProgressPct[1], curP.levelCompleted[1] ? "[DONE]" : "",
			curP.levelProgressPct[2], curP.levelCompleted[2] ? "[DONE]" : "",
			curP.levelProgressPct[3], curP.levelCompleted[3] ? "[DONE]" : "");
		iSetColor(120, 220, 255);
		iText(220, 172, statLine2, GLUT_BITMAP_HELVETICA_12);

		char statLine3[256];
		sprintf_s(statLine3, "Kills -> Ghost: %d | Skeleton: %d | Soldier: %d | Archer: %d | Saint: %d | Mesh: %d",
			curP.ghostKills, curP.skeletonKills, curP.soldierKills, curP.archerKills, curP.saintKills, curP.meshKills);
		iSetColor(180, 200, 220);
		iText(220, 146, statLine3, GLUT_BITMAP_HELVETICA_12);
	}
	else {
		iSetColor(255, 215, 0);
		iText(240, 200, (char*)"NO SAVED PLAYER PROFILES YET", GLUT_BITMAP_HELVETICA_18);
		iSetColor(180, 210, 240);
		iText(240, 160, (char*)"Click 'START GAME' or '+ ADD PLAYER' to enter your Hero Name and create your dynamic profile!", GLUT_BITMAP_HELVETICA_12);
	}

	/* Bottom Instruction / Shortcuts Tip */
	iSetColor(160, 185, 220);
	iText(200, 102, (char*)"Tips: Click '+ ADD PLAYER' to enter custom name | Press 'R' to Rename active player | Press '1','2','3' to select slot", GLUT_BITMAP_HELVETICA_10);

	/* ================= 4. INTERACTIVE BACK BUTTON ================= */
	iSetColor(25, 30, 55);
	iFilledRectangle(BTN_OPT_BACK_X1, BTN_OPT_BACK_Y1,
		BTN_OPT_BACK_X2 - BTN_OPT_BACK_X1, BTN_OPT_BACK_Y2 - BTN_OPT_BACK_Y1);
	iSetColor(255, 215, 0);
	iRectangle(BTN_OPT_BACK_X1, BTN_OPT_BACK_Y1,
		BTN_OPT_BACK_X2 - BTN_OPT_BACK_X1, BTN_OPT_BACK_Y2 - BTN_OPT_BACK_Y1);
	iSetColor(255, 255, 255);
	iText(BTN_OPT_BACK_X1 + 55, BTN_OPT_BACK_Y1 + 22, (char*)"BACK TO MENU [B]", GLUT_BITMAP_TIMES_ROMAN_24);

	/* ================= 5. INTERACTIVE NAME INPUT MODAL ================= */
	if (isNamingPlayer && currentScreen == SCREEN_OPTIONS) {
		// Dark background overlay
		iSetColor(6, 8, 16);
		iFilledRectangle(300, 140, 680, 420);

		// Glowing Border
		iSetColor(255, 215, 0);
		iRectangle(300, 140, 680, 420);
		iRectangle(303, 143, 674, 414);

		// Header
		iSetColor(255, 215, 0);
		if (namingTargetSlot == -1) {
			iText(SCREEN_W / 2 - 200, 510, (char*)"CREATE NEW PLAYER PROFILE", GLUT_BITMAP_TIMES_ROMAN_24);
		}
		else {
			iText(SCREEN_W / 2 - 200, 510, (char*)"RENAME ACTIVE PLAYER PROFILE", GLUT_BITMAP_TIMES_ROMAN_24);
		}

		iSetColor(100, 200, 255);
		iText(340, 475, (char*)"Type any desired player name, then press ENTER:", GLUT_BITMAP_HELVETICA_12);

		// Input Box
		iSetColor(20, 26, 45);
		iFilledRectangle(340, 390, 600, 60);
		iSetColor(100, 220, 255);
		iRectangle(340, 390, 600, 60);
		iRectangle(342, 392, 596, 56);

		// Text & Cursor
		nameInputBlinkTimer++;
		bool showCursor = ((nameInputBlinkTimer / 25) % 2 == 0);

		if (nameInputLen == 0) {
			if (showCursor) {
				iSetColor(255, 215, 0);
				iText(360, 412, (char*)"|", GLUT_BITMAP_TIMES_ROMAN_24);
			}
			iSetColor(120, 140, 170);
			iText(375, 412, (char*)"Enter player name here...", GLUT_BITMAP_HELVETICA_18);
		}
		else {
			char displayBuf[64];
			if (showCursor) {
				sprintf_s(displayBuf, "%s|", nameInputBuffer);
			}
			else {
				sprintf_s(displayBuf, "%s", nameInputBuffer);
			}
			iSetColor(255, 255, 255);
			iText(360, 412, displayBuf, GLUT_BITMAP_TIMES_ROMAN_24);
		}

		// Length & instructions
		char charInfo[128];
		sprintf_s(charInfo, "Length: %d / 24 chars  |  Supports all letters, numbers, spaces & symbols", nameInputLen);
		iSetColor(160, 190, 220);
		iText(340, 360, charInfo, GLUT_BITMAP_HELVETICA_10);

		iSetColor(255, 230, 150);
		iText(340, 330, (char*)"Press ENTER to Save  |  Press ESC to Cancel  |  Backspace to Delete", GLUT_BITMAP_HELVETICA_12);

		// CONFIRM BUTTON
		iSetColor(20, 75, 40);
		iFilledRectangle(BTN_MODAL_CONFIRM_X1, BTN_MODAL_CONFIRM_Y1,
			BTN_MODAL_CONFIRM_X2 - BTN_MODAL_CONFIRM_X1, BTN_MODAL_CONFIRM_Y2 - BTN_MODAL_CONFIRM_Y1);
		iSetColor(50, 255, 120);
		iRectangle(BTN_MODAL_CONFIRM_X1, BTN_MODAL_CONFIRM_Y1,
			BTN_MODAL_CONFIRM_X2 - BTN_MODAL_CONFIRM_X1, BTN_MODAL_CONFIRM_Y2 - BTN_MODAL_CONFIRM_Y1);
		iSetColor(255, 255, 255);
		iText(BTN_MODAL_CONFIRM_X1 + 35, BTN_MODAL_CONFIRM_Y1 + 20, (char*)"SAVE NAME [ENTER]", GLUT_BITMAP_HELVETICA_12);

		// CANCEL BUTTON
		iSetColor(75, 20, 30);
		iFilledRectangle(BTN_MODAL_CANCEL_X1, BTN_MODAL_CANCEL_Y1,
			BTN_MODAL_CANCEL_X2 - BTN_MODAL_CANCEL_X1, BTN_MODAL_CANCEL_Y2 - BTN_MODAL_CANCEL_Y1);
		iSetColor(255, 70, 70);
		iRectangle(BTN_MODAL_CANCEL_X1, BTN_MODAL_CANCEL_Y1,
			BTN_MODAL_CANCEL_X2 - BTN_MODAL_CANCEL_X1, BTN_MODAL_CANCEL_Y2 - BTN_MODAL_CANCEL_Y1);
		iSetColor(255, 255, 255);
		iText(BTN_MODAL_CANCEL_X1 + 45, BTN_MODAL_CANCEL_Y1 + 20, (char*)"CANCEL [ESC]", GLUT_BITMAP_HELVETICA_12);
	}
}

void drawStory()
{
	int idx = storyIndex - 1;
	if (idx < 0) idx = 0;
	if (idx > 3) idx = 3;

	// Render the Story image
	iShowImage(0, 0, SCREEN_W, SCREEN_H, imgStory[idx]);

	// In Story Parts 1-3: Show both SKIP (Left) and NEXT (Right)
	if (storyIndex >= 1 && storyIndex <= 3)
	{
		// SKIP BUTTON (LEFT)
		iSetColor(20, 20, 30);
		iFilledRectangle(BTN_STORY_SKIP_X1, BTN_STORY_SKIP_Y1,
			BTN_STORY_SKIP_X2 - BTN_STORY_SKIP_X1, BTN_STORY_SKIP_Y2 - BTN_STORY_SKIP_Y1);
		iSetColor(255, 70, 70);
		iRectangle(BTN_STORY_SKIP_X1, BTN_STORY_SKIP_Y1,
			BTN_STORY_SKIP_X2 - BTN_STORY_SKIP_X1, BTN_STORY_SKIP_Y2 - BTN_STORY_SKIP_Y1);
		iSetColor(255, 255, 255);
		iText(BTN_STORY_SKIP_X1 + 35, BTN_STORY_SKIP_Y1 + 22, "SKIP [S]", GLUT_BITMAP_HELVETICA_18);

		// NEXT BUTTON (RIGHT)
		iSetColor(20, 20, 30);
		iFilledRectangle(BTN_STORY_NEXT_X1, BTN_STORY_NEXT_Y1,
			BTN_STORY_NEXT_X2 - BTN_STORY_NEXT_X1, BTN_STORY_NEXT_Y2 - BTN_STORY_NEXT_Y1);
		iSetColor(255, 215, 0);
		iRectangle(BTN_STORY_NEXT_X1, BTN_STORY_NEXT_Y1,
			BTN_STORY_NEXT_X2 - BTN_STORY_NEXT_X1, BTN_STORY_NEXT_Y2 - BTN_STORY_NEXT_Y1);
		iSetColor(255, 255, 255);
		iText(BTN_STORY_NEXT_X1 + 32, BTN_STORY_NEXT_Y1 + 22, "NEXT [N]", GLUT_BITMAP_HELVETICA_18);
	}
	else if (storyIndex == 4)
	{
		// Story Part 4: NEXT / CONTINUE BUTTON (RIGHT)
		iSetColor(20, 20, 30);
		iFilledRectangle(BTN_STORY_NEXT_X1, BTN_STORY_NEXT_Y1,
			BTN_STORY_NEXT_X2 - BTN_STORY_NEXT_X1, BTN_STORY_NEXT_Y2 - BTN_STORY_NEXT_Y1);
		iSetColor(255, 215, 0);
		iRectangle(BTN_STORY_NEXT_X1, BTN_STORY_NEXT_Y1,
			BTN_STORY_NEXT_X2 - BTN_STORY_NEXT_X1, BTN_STORY_NEXT_Y2 - BTN_STORY_NEXT_Y1);
		iSetColor(255, 255, 255);
		iText(BTN_STORY_NEXT_X1 + 32, BTN_STORY_NEXT_Y1 + 22, "NEXT [N]", GLUT_BITMAP_HELVETICA_18);
	}
}

void drawEndCard()
{
	// End Card stylish dark backdrop
	iSetColor(10, 10, 20);
	iFilledRectangle(0, 0, SCREEN_W, SCREEN_H);

	// Decorative Borders
	iSetColor(255, 215, 0);
	iRectangle(30, 30, SCREEN_W - 60, SCREEN_H - 60);
	iRectangle(34, 34, SCREEN_W - 68, SCREEN_H - 68);

	// Animated You Win Graphic
	int winW = 540;
	int winH = 340;
	int winX = (SCREEN_W - winW) / 2;
	int winY = SCREEN_H - winH - 60;
	iShowImage(winX, winY, winW, winH, imgYouWin[youWinFrame % 3]);

	// Victory Title & Subtitles
	iSetColor(255, 215, 0);
	iText(SCREEN_W / 2 - 190, 270, "VICTORY - ALL LEVELS COMPLETED!", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(240, 240, 255);
	iText(SCREEN_W / 2 - 325, 225, "Congratulations! You defeated the Soldier Army, Archer Horde & King Mesh!", GLUT_BITMAP_HELVETICA_18);
	iSetColor(120, 220, 255);
	iText(SCREEN_W / 2 - 150, 190, "Peace has been restored to the Kingdom!", GLUT_BITMAP_HELVETICA_18);

	// "GO TO HOME" Interactive Button
	iSetColor(22, 26, 45);
	iFilledRectangle(BTN_END_HOME_X1, BTN_END_HOME_Y1,
		BTN_END_HOME_X2 - BTN_END_HOME_X1, BTN_END_HOME_Y2 - BTN_END_HOME_Y1);

	iSetColor(255, 215, 0);
	iRectangle(BTN_END_HOME_X1, BTN_END_HOME_Y1,
		BTN_END_HOME_X2 - BTN_END_HOME_X1, BTN_END_HOME_Y2 - BTN_END_HOME_Y1);

	iSetColor(255, 255, 255);
	iText(BTN_END_HOME_X1 + 65, BTN_END_HOME_Y1 + 25, "GO TO HOME [H]", GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawCredits()
{
	/* Background - Dark premium sci-fi/fantasy backdrop */
	iSetColor(12, 14, 26);
	iFilledRectangle(0, 0, SCREEN_W, SCREEN_H);

	/* Decorative Outer & Inner Golden Borders */
	iSetColor(255, 215, 0);
	iRectangle(35, 35, SCREEN_W - 70, SCREEN_H - 70);
	iRectangle(39, 39, SCREEN_W - 78, SCREEN_H - 78);

	/* Title */
	iSetColor(255, 215, 0);
	iText(SCREEN_W / 2 - 140, 635,
		(char*)"PROJECT CREDITS",
		GLUT_BITMAP_TIMES_ROMAN_24);

	/* Subtitle */
	iSetColor(100, 200, 255);
	iText(SCREEN_W / 2 - 110, 595,
		(char*)"DEVELOPMENT TEAM",
		GLUT_BITMAP_HELVETICA_18);

	/* Accent Line Under Title */
	iSetColor(255, 215, 0);
	iLine(SCREEN_W / 2 - 200, 580, SCREEN_W / 2 + 200, 580);

	/* --- Developer Cards (2x2 Grid) --- */
	int cardW = 500;
	int cardH = 110;
	int leftColX = 110;
	int rightColX = 670;
	int topRowY = 430;
	int bottomRowY = 280;

	// Card 1: MD RABIUL ISLAM BHUIYAN SEYAM
	iSetColor(22, 26, 45);
	iFilledRectangle(leftColX, topRowY, cardW, cardH);
	iSetColor(255, 215, 0);
	iRectangle(leftColX, topRowY, cardW, cardH);
	iSetColor(255, 215, 0);
	iText(leftColX + 25, topRowY + 70, (char*)"MD RABIUL ISLAM BHUIYAN SEYAM", GLUT_BITMAP_HELVETICA_18);
	iSetColor(200, 220, 255);
	iText(leftColX + 25, topRowY + 35, (char*)"Student ID: 00725105101088", GLUT_BITMAP_HELVETICA_18);

	// Card 2: AFIA FARZANA
	iSetColor(22, 26, 45);
	iFilledRectangle(rightColX, topRowY, cardW, cardH);
	iSetColor(255, 215, 0);
	iRectangle(rightColX, topRowY, cardW, cardH);
	iSetColor(255, 215, 0);
	iText(rightColX + 25, topRowY + 70, (char*)"AFIA FARZANA", GLUT_BITMAP_HELVETICA_18);
	iSetColor(200, 220, 255);
	iText(rightColX + 25, topRowY + 35, (char*)"Student ID: 00725105101105", GLUT_BITMAP_HELVETICA_18);

	// Card 3: INJAMAMUL HAQUE PIASH
	iSetColor(22, 26, 45);
	iFilledRectangle(leftColX, bottomRowY, cardW, cardH);
	iSetColor(255, 215, 0);
	iRectangle(leftColX, bottomRowY, cardW, cardH);
	iSetColor(255, 215, 0);
	iText(leftColX + 25, bottomRowY + 70, (char*)"INJAMAMUL HAQUE PIASH", GLUT_BITMAP_HELVETICA_18);
	iSetColor(200, 220, 255);
	iText(leftColX + 25, bottomRowY + 35, (char*)"Student ID: 0072510510106", GLUT_BITMAP_HELVETICA_18);

	// Card 4: MD AJMAIN ABIR
	iSetColor(22, 26, 45);
	iFilledRectangle(rightColX, bottomRowY, cardW, cardH);
	iSetColor(255, 215, 0);
	iRectangle(rightColX, bottomRowY, cardW, cardH);
	iSetColor(255, 215, 0);
	iText(rightColX + 25, bottomRowY + 70, (char*)"MD AJMAIN ABIR", GLUT_BITMAP_HELVETICA_18);
	iSetColor(200, 220, 255);
	iText(rightColX + 25, bottomRowY + 35, (char*)"Student ID: 00725105101116", GLUT_BITMAP_HELVETICA_18);

	/* Department / Course Footer Note */
	iSetColor(180, 190, 210);
	iText(SCREEN_W / 2 - 200, 185, (char*)"Department of Computer Science & Engineering - AUST", GLUT_BITMAP_HELVETICA_12);

	/* Interactive BACK Button */
	iSetColor(25, 30, 55);
	iFilledRectangle(BTN_BACK_X1, BTN_BACK_Y1, BTN_BACK_X2 - BTN_BACK_X1, BTN_BACK_Y2 - BTN_BACK_Y1);
	iSetColor(255, 215, 0);
	iRectangle(BTN_BACK_X1, BTN_BACK_Y1, BTN_BACK_X2 - BTN_BACK_X1, BTN_BACK_Y2 - BTN_BACK_Y1);
	iSetColor(255, 255, 255);
	iText(BTN_BACK_X1 + 55, BTN_BACK_Y1 + 27, (char*)"BACK TO MENU [B]", GLUT_BITMAP_TIMES_ROMAN_24);
}

#endif
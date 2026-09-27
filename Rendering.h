#ifndef RENDERING_H
#define RENDERING_H

#include "GameCommon.h"
#include "Enemy.h"
#include "Player.h"
#include "MenuSystem.h"
#include "AudioManager.h"
#include "SaveSystem.h"

/* ================================================================
CIRCULAR COOLDOWN DIAL HUD RENDERER (Enlarged for Level 3)
================================================================ */
void drawCooldownDial(double cx, double cy, double radius, double progress, const char* keyLetter, const char* label, bool ready)
{
	// 1. Dark outer container
	iSetColor(15, 18, 30);
	iFilledCircle(cx, cy, radius + 5);

	// 2. White / Light Background base
	iSetColor(240, 240, 248);
	iFilledCircle(cx, cy, radius);

	// 3. Blue Circular Filling Arc (0.0 to 1.0)
	if (progress > 0.0) {
		if (ready) iSetColor(0, 215, 255); // Radiant Cyan
		else iSetColor(30, 144, 255);      // Vivid Blue

		int numSegments = 60;
		int activeSegments = (int)(numSegments * progress);
		if (activeSegments > numSegments) activeSegments = numSegments;

		glBegin(GL_TRIANGLE_FAN);
		glVertex2f((GLfloat)cx, (GLfloat)cy);
		for (int i = 0; i <= activeSegments; i++) {
			double angleDeg = 90.0 - (360.0 * ((double)i / numSegments));
			double angleRad = angleDeg * 3.1415926535 / 180.0;
			glVertex2f((GLfloat)(cx + radius * cos(angleRad)), (GLfloat)(cy + radius * sin(angleRad)));
		}
		glEnd();
	}

	// 4. Inner core center circle
	iSetColor(20, 24, 40);
	iFilledCircle(cx, cy, radius - 8);

	// 5. Border Ring
	if (ready) {
		iSetColor(255, 215, 0); // Gold border when ready!
	}
	else {
		iSetColor(130, 150, 190);
	}
	iCircle(cx, cy, radius);
	iCircle(cx, cy, radius + 1);
	iCircle(cx, cy, radius + 2);

	// 6. Center Hotkey Letter (Centered perfectly inside dial)
	if (ready) iSetColor(255, 255, 255);
	else iSetColor(170, 180, 200);
	iDrawCenteredText(cx, cy - 8, (char*)keyLetter, GLUT_BITMAP_TIMES_ROMAN_24);

	// 7. Label and Status underneath (Centered perfectly)
	iSetColor(255, 255, 255);
	iDrawCenteredText(cx, cy - radius - 16, (char*)label, GLUT_BITMAP_HELVETICA_12);
	if (ready) {
		iSetColor(50, 255, 120);
		iDrawCenteredText(cx, cy - radius - 30, "READY", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		iSetColor(180, 210, 240);
		char pctStr[16];
		sprintf_s(pctStr, "%d%%", (int)(progress * 100.0));
		iDrawCenteredText(cx, cy - radius - 30, pctStr, GLUT_BITMAP_HELVETICA_12);
	}
}

/* ================================================================
DRAWING & INTERFACE
================================================================ */
void drawGame()
{
	if (playerHealth <= 0) {
		iShowImage(0, 0, SCREEN_W, SCREEN_H, imgGameOver);
		iSetColor(255, 255, 255);
		iDrawCenteredText(SCREEN_W / 2, 50, "Press 'R' to Restart | 'H' for Home | ESC to Exit", GLUT_BITMAP_HELVETICA_18);
		return;
	}

	if (showWinCard) {
		/* Show appropriate background for victory card */
		if (currentLevel == 1) {
			iShowImage(0, 0, SCREEN_W, SCREEN_H, bg[3]); // B4
		}
		else if (currentLevel == 2) {
			iShowImage(0, 0, SCREEN_W, SCREEN_H, bg[4]); // B5
		}
		else if (currentLevel == 3) {
			iShowImage(0, 0, SCREEN_W, SCREEN_H, imgBL2); // BL2
		}
		else {
			iShowImage(0, 0, SCREEN_W, SCREEN_H, imgBL3); // BL3
		}

		/* Center Animated You Win card (Frames 1-3) */
		int winW = 600;
		int winH = 400;
		int winX = (SCREEN_W - winW) / 2;
		int winY = (SCREEN_H - winH) / 2 + 50;
		iShowImage(winX, winY, winW, winH, imgYouWin[youWinFrame % 3]);

		/* Victory Banner / Controls at bottom */
		iSetColor(15, 15, 25);
		iFilledRectangle(SCREEN_W / 2 - 400, 45, 800, 65);
		iSetColor(255, 215, 0);
		iRectangle(SCREEN_W / 2 - 400, 45, 800, 65);

		iSetColor(255, 255, 255);
		char winTitle[64];
		if (currentLevel == 1) {
			sprintf_s(winTitle, "LEVEL 1 COMPLETED!");
		}
		else if (currentLevel == 2) {
			sprintf_s(winTitle, "LEVEL 2 COMPLETED - SAINT DEFEATED!");
		}
		else if (currentLevel == 3) {
			sprintf_s(winTitle, "LEVEL 3 COMPLETED - ARCHER HORDE DEFEATED!");
		}
		else {
			sprintf_s(winTitle, "VICTORY - KING MESH DEFEATED!");
		}
		iDrawCenteredTextInRect(SCREEN_W / 2 - 400, 78, SCREEN_W / 2 + 400, 110, winTitle, GLUT_BITMAP_TIMES_ROMAN_24);

		iSetColor(255, 215, 0);
		if (currentLevel == 1) {
			iDrawCenteredTextInRect(SCREEN_W / 2 - 400, 48, SCREEN_W / 2 + 400, 78, "Press 'N' / [Right Arrow] for Level 2 | 'R' to Restart | 'H' for Home | ESC to Exit", GLUT_BITMAP_HELVETICA_18);
		}
		else if (currentLevel == 2) {
			iDrawCenteredTextInRect(SCREEN_W / 2 - 400, 48, SCREEN_W / 2 + 400, 78, "Press 'N' / [Right Arrow] for Level 3 | 'R' to Restart | 'H' for Home | ESC to Exit", GLUT_BITMAP_HELVETICA_18);
		}
		else if (currentLevel == 3) {
			iDrawCenteredTextInRect(SCREEN_W / 2 - 400, 48, SCREEN_W / 2 + 400, 78, "Press 'N' / [Right Arrow] for Final Boss (Level 4) | 'R' to Restart | 'H' for Home | ESC to Exit", GLUT_BITMAP_HELVETICA_18);
		}
		else {
			iDrawCenteredTextInRect(SCREEN_W / 2 - 400, 48, SCREEN_W / 2 + 400, 78, "Press 'N' / [Right Arrow] / [Enter] for Victory End Card | 'R' to Restart | 'H' for Home", GLUT_BITMAP_HELVETICA_18);
		}
		return;
	}

	if (currentLevel == 4) {
		/* Level 4 Background: BL3 */
		iShowImage(0, 0, SCREEN_W, SCREEN_H, imgBL3);
	}
	else if (currentLevel == 3) {
		/* Level 3 Backgrounds: BL1 or BL2 */
		if (lvl3Stage == 1) {
			iShowImage(0, 0, SCREEN_W, SCREEN_H, imgBL1);
		}
		else {
			iShowImage(0, 0, SCREEN_W, SCREEN_H, imgBL2);
		}
	}
	else if (isBossArena) {
		/* Render B5 Arena Background */
		iShowImage(0, 0, SCREEN_W, SCREEN_H, bg[4]);
	}
	else {
		/* Render Scrolling Background Sequence (B1, B2, B3, B2, B3, B2, B3, B4) */
		for (int i = 0; i < NUM_TILES; i++)
		{
			double tileLeft = (double)(i * TILE_W) - bgOffset;
			if (tileLeft >= SCREEN_W)     break;
			if (tileLeft + TILE_W <= 0)   continue;
			int bgIdx = tileSeq[i];
			iShowImage((int)tileLeft, 0, TILE_W, TILE_H, bg[bgIdx]);
		}
	}

	/* 1. Render Obstacles (Balls & Bats - Level 2) */
	if (currentLevel == 2 && !isBossArena) {
		for (int i = 0; i < MAX_OBSTACLES; i++) {
			if (obstacles[i].active) {
				if (obstacles[i].type == OBSTACLE_BALL) {
					iShowImage((int)obstacles[i].x, (int)obstacles[i].y, obstacles[i].width, obstacles[i].height, imgBall[obstacles[i].frame % 5]);
				}
				else if (obstacles[i].type == OBSTACLE_BAT) {
					if (!obstacles[i].fromRight) {
						iShowImageFlipped((int)obstacles[i].x, (int)obstacles[i].y, obstacles[i].width, obstacles[i].height, imgBat[obstacles[i].frame % 3], true);
					}
					else {
						iShowImage((int)obstacles[i].x, (int)obstacles[i].y, obstacles[i].width, obstacles[i].height, imgBat[obstacles[i].frame % 3]);
					}
				}
			}
		}
	}

	/* 2. Render Projectiles (Arrows & Boss Flash) */
	if (currentLevel == 3 || currentLevel == 4) {
		for (int i = 0; i < MAX_ARROWS; i++) {
			if (arrows[i].active) {
				if (arrows[i].fromRight) {
					// Flipped horizontally so arrowhead points LEFT towards player!
					iShowImageFlipped((int)arrows[i].x, (int)arrows[i].y, 90, 35, imgArrow, true);
				}
				else {
					// Unflipped so arrowhead points RIGHT towards player!
					iShowImage((int)arrows[i].x, (int)arrows[i].y, 90, 35, imgArrow);
				}
			}
		}
	}
	if (bossFlash.active) {
		iShowImage((int)bossFlash.x, (int)bossFlash.y, 110, 110, imgFlash);
	}

	/* 3. Render Friendly Summoned Allies / Bots (Level 3 N Power) */
	if (currentLevel == 3) {
		for (int i = 0; i < MAX_ALLIES; i++) {
			if (allies[i].active && allies[i].alive) {
				if (allies[i].type == ALLY_GHOST) {
					iShowImage((int)allies[i].x, (int)allies[i].y, 180, 180, imgGhostRight[allies[i].frame % 2]);

					// Cyan Ally Health Bar
					iSetColor(0, 200, 255);
					iFilledRectangle((int)allies[i].x + 65, (int)allies[i].y + 190, (allies[i].health / 50.0) * 50.0, 5);
					iSetColor(255, 255, 255);
					iRectangle((int)allies[i].x + 65, (int)allies[i].y + 190, 50, 5);
				}
				else if (allies[i].type == ALLY_SKELETON) {
					int sImg = 0;
					if (allies[i].isFighting) {
						sImg = imgSkeletonFightRight[allies[i].fightFrame % 5];
					}
					else {
						sImg = imgSkeletonRunRight[allies[i].frame % 5];
					}
					iShowImage((int)allies[i].x, (int)allies[i].y, 180, 250, sImg);

					// Green Ally Health Bar
					iSetColor(50, 255, 100);
					iFilledRectangle((int)allies[i].x + 65, (int)allies[i].y + 255, (allies[i].health / 70.0) * 50.0, 5);
					iSetColor(255, 255, 255);
					iRectangle((int)allies[i].x + 65, (int)allies[i].y + 255, 50, 5);
				}
			}
		}
	}

	/* 4. Render Enemies & Bosses (Behind Hero Layer) */
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active && enemies[i].alive) {
			if (enemies[i].type == ENEMY_GHOST) {
				int eImg = enemies[i].fromRight ? imgGhostLeft[enemies[i].frame % 2] : imgGhostRight[enemies[i].frame % 2];
				iShowImage((int)enemies[i].x, (int)enemies[i].y, 200, 200, eImg);

				iSetColor(255, 0, 0);
				iFilledRectangle((int)enemies[i].x + 75, (int)enemies[i].y + 210, enemies[i].health / 2.0, 6);
				iSetColor(255, 255, 255);
				iRectangle((int)enemies[i].x + 75, (int)enemies[i].y + 210, 50, 6);
			}
			else if (enemies[i].type == ENEMY_SKELETON) {
				int sImg = 0;
				if (enemies[i].isFighting) {
					sImg = enemies[i].fromRight ?
						imgSkeletonFightLeft[enemies[i].fightFrame % 5] :
						imgSkeletonFightRight[enemies[i].fightFrame % 5];
				}
				else {
					sImg = enemies[i].fromRight ?
						imgSkeletonRunLeft[enemies[i].frame % 5] :
						imgSkeletonRunRight[enemies[i].frame % 5];
				}

				iShowImage((int)enemies[i].x, (int)enemies[i].y, 180, 250, sImg);

				iSetColor(255, 0, 0);
				iFilledRectangle((int)enemies[i].x + 65, (int)enemies[i].y + 255, (enemies[i].health / 150.0) * 50.0, 6);
				iSetColor(255, 255, 255);
				iRectangle((int)enemies[i].x + 65, (int)enemies[i].y + 255, 50, 6);
			}
			else if (enemies[i].type == ENEMY_SOLDIER) {
				// Soldier rendering (Walk vs Fight) - Increased height to 255px
				int soldierImg = 0;
				if (enemies[i].isFighting) {
					soldierImg = imgSoldierFight[enemies[i].fightFrame % 6];
				}
				else {
					soldierImg = imgSoldierWalk[enemies[i].frame % 5];
				}

				// If spawned from left, flip horizontally so soldier faces right!
				if (!enemies[i].fromRight) {
					iShowImageFlipped((int)enemies[i].x, (int)enemies[i].y, 195, 255, soldierImg, true);
				}
				else {
					iShowImage((int)enemies[i].x, (int)enemies[i].y, 195, 255, soldierImg);
				}

				// Soldier Health Bar
				iSetColor(220, 20, 40);
				iFilledRectangle((int)enemies[i].x + 70, (int)enemies[i].y + 260, (enemies[i].health / 100.0) * 50.0, 6);
				iSetColor(255, 255, 255);
				iRectangle((int)enemies[i].x + 70, (int)enemies[i].y + 260, 50, 6);
			}
			else if (enemies[i].type == ENEMY_ARCHER) {
				// Archer rendering (Walk vs Shoot) - 8 Walk frames & 5 Fight frames
				int archerImg = 0;
				if (enemies[i].isFighting) {
					archerImg = imgArcherFight[enemies[i].fightFrame % 5];
				}
				else {
					archerImg = imgArcherWalk[enemies[i].frame % 8];
				}

				// If spawned from Right, flip horizontally so archer faces left towards player!
				// If spawned from Left, keep original unflipped so archer faces right!
				if (enemies[i].fromRight) {
					iShowImageFlipped((int)enemies[i].x, (int)enemies[i].y, 200, 255, archerImg, true);
				}
				else {
					iShowImage((int)enemies[i].x, (int)enemies[i].y, 200, 255, archerImg);
				}

				// Archer Health Bar
				iSetColor(220, 20, 40);
				iFilledRectangle((int)enemies[i].x + 70, (int)enemies[i].y + 260, (enemies[i].health / 120.0) * 50.0, 6);
				iSetColor(255, 255, 255);
				iRectangle((int)enemies[i].x + 70, (int)enemies[i].y + 260, 50, 6);
			}
		}
	}

	/* Render Boss Saint (Level 2) */
	if (bossSaint.active && bossSaint.alive) {
		int saintImg = 0;
		if (bossSaint.isFighting) {
			saintImg = imgSaintFight[bossSaint.fightFrame % 4];
		}
		else {
			saintImg = imgSaintWalk[bossSaint.walkFrame % 4];
		}

		iShowImage((int)bossSaint.x, (int)bossSaint.y, 220, 270, saintImg);

		iSetColor(220, 20, 60);
		iFilledRectangle((int)bossSaint.x + 60, (int)bossSaint.y + 275, (bossSaint.health / 300.0) * 100.0, 8);
		iSetColor(255, 255, 255);
		iRectangle((int)bossSaint.x + 60, (int)bossSaint.y + 275, 100, 8);
		iSetColor(255, 215, 0);
		iDrawCenteredText((int)bossSaint.x + 110, (int)bossSaint.y + 288, "SAINT", GLUT_BITMAP_HELVETICA_12);
	}

	/* Render Boss Mesh (Level 4 - King / Main Villain - Scaled slightly taller than Hero: 280px) */
	if (bossMesh.active && bossMesh.alive) {
		int meshImg = 0;
		int meshW = 170; // Proportional scaling: 230 * (280 / 380) = 169.47 -> 170
		int meshH = 280; // Visible height slightly taller than Hero (250px)
		if (bossMesh.isFighting) {
			meshImg = imgMeshFight[bossMesh.fightFrame % 5];
			meshW = 194; // Proportional scaling: 250 * (280 / 360) = 194.44 -> 194
			meshH = 280;
		}
		else {
			meshImg = imgMeshWalk[bossMesh.walkFrame % 5];
			meshW = 170;
			meshH = 280;
		}

		if (bossMesh.fromRight) {
			// On right side of Hero -> Unflipped, faces Left
			iShowImage((int)bossMesh.x, (int)bossMesh.y, meshW, meshH, meshImg);
		}
		else {
			// On left side of Hero -> Flipped horizontally, faces Right
			iShowImageFlipped((int)bossMesh.x, (int)bossMesh.y, meshW, meshH, meshImg, true);
		}

		// Teleportation Warp Visual Aura
		if (bossMesh.vanishEffectTimer > 0) {
			iSetColor(180, 0, 255);
			iCircle((int)bossMesh.x + meshW / 2, (int)bossMesh.y + meshH / 2, 60 + (18 - bossMesh.vanishEffectTimer) * 3);
			iSetColor(0, 220, 255);
			iCircle((int)bossMesh.x + meshW / 2, (int)bossMesh.y + meshH / 2, 50 + (18 - bossMesh.vanishEffectTimer) * 2);
		}
	}

	/* 5. Render Hero Character (FOREGROUND / FRONT LAYER - ON TOP OF ALL ENEMIES & OBSTACLES) */
	if (isJumping) {
		iShowImage(charX, (int)charY, 180, 250, imgJump[jumpFrame % 8]);
	}
	else if (currentState == SIT) {
		iShowImage(charX + 15, (int)charY, 150, 185, imgSit[charFrame % 3]);
	}
	else if (currentState == FIGHT_RIGHT) {
		// Fiery sword fight animation (height 270px) - 3 images
		iShowImage(charX, (int)charY, 300, 270, imgFightRight[charFrame % 3]);
	}
	else if (currentState == FIGHT_LEFT) {
		// Fiery sword fight animation (height 270px) - 3 images
		iShowImage(charX - 120, (int)charY, 300, 270, imgFightLeft[charFrame % 3]);
	}
	else {
		int currentImg = imgIdle;
		if (currentState == IDLE) {
			currentImg = facingRight ? imgIdle : imgIdleLeft;
		}
		else if (currentState == RUN_RIGHT) currentImg = imgRunRight[charFrame % 9];
		else if (currentState == RUN_LEFT) currentImg = imgRunLeft[charFrame % 9];

		iShowImage(charX, (int)charY, 180, 250, currentImg);
	}

	/* 6. Render Manipulation Directional Shockwave Effect (M Power - 500px Directional Area) */
	if (manipulationEffectTimer > 0) {
		double rectX = 0;
		if (manipulationFacingRight) {
			rectX = manipulationEffectX + 80.0;
		}
		else {
			rectX = manipulationEffectX + 100.0 - 500.0;
		}
		double rectY = GROUND_Y;
		double rectW = 500.0;
		double rectH = 260.0;

		// Glowing translucent cyan rectangular energy fill
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.0f, 0.85f, 1.0f, 0.24f * (manipulationEffectTimer / 25.0f));
		glRectf((GLfloat)rectX, (GLfloat)rectY, (GLfloat)(rectX + rectW), (GLfloat)(rectY + rectH));
		glDisable(GL_BLEND);

		// Radiant cyan / white electric rectangular borders
		iSetColor(0, 220, 255);
		iRectangle((int)rectX, (int)rectY, (int)rectW, (int)rectH);
		iSetColor(140, 240, 255);
		iRectangle((int)rectX + 2, (int)rectY + 2, (int)rectW - 4, (int)rectH - 4);
		iSetColor(255, 255, 255);
		iRectangle((int)rectX + 4, (int)rectY + 4, (int)rectW - 8, (int)rectH - 8);

		// Dynamic horizontal energy scanlines
		for (int line = 0; line < 6; line++) {
			double lineY = rectY + 20 + line * 40;
			iSetColor(200, 245, 255);
			iLine((int)rectX + 6, (int)lineY, (int)(rectX + rectW - 6), (int)lineY);
		}
	}

	/* ================================================================
	7. HEALTH CARDS & HUD (Hero -> TOP-LEFT, Enemy/Boss/Progress -> TOP-RIGHT)
	================================================================ */

	// HERO / TIM HEALTH CARD (ALWAYS ON TOP-LEFT) - Native Aspect Ratio (1916:821 = 2.3337), Large & Crisp
	int cardW = 380;
	int cardH = 163;
	int timCardX = 20;
	int timCardY = SCREEN_H - cardH - 15;
	drawHeroHealthCard(imgHealthTim, timCardX, timCardY, cardW, cardH, playerHealth, 200, GLUT_BITMAP_TIMES_ROMAN_24);

	/* Top-Center Special Abilities HUD (N & M Cooldown Dials for Level 3 - Greatly Enlarged & Balanced) */
	if (currentLevel == 3) {
		double dialRadius = 40.0;
		drawCooldownDial(SCREEN_W / 2 - 75, SCREEN_H - 65, dialRadius, nCooldown, "N", "SUMMON", (nCooldown >= 1.0));
		drawCooldownDial(SCREEN_W / 2 + 75, SCREEN_H - 65, dialRadius, mCooldown, "M", "MANIPULATE", (mCooldown >= 1.0));
	}

	// ENEMY / BOSS HEALTH CARDS OR PROGRESS BAR (ALWAYS ON TOP-RIGHT)
	int enemyCardW = 380;
	int enemyCardH = 163;
	int enemyCardX = SCREEN_W - enemyCardW - 20;
	int enemyCardY = SCREEN_H - enemyCardH - 15;

	if (currentLevel == 4) {
		// King Mesh Health Card (Level 4 - 400 HP) - Native Aspect Ratio (1915:821 = 2.3325)
		drawRightSideHealthCard(imgHealthMesh, enemyCardX, enemyCardY, enemyCardW, enemyCardH, bossMesh.health, 400, GLUT_BITMAP_TIMES_ROMAN_24);
	}
	else if (currentLevel == 3) {
		// LEVEL 3 HUD OVERHAUL: Soldier & Archer Enemy Cards REMOVED!
		// Replaced exclusively with the Progress Bar Card displaying tracked numerical progress.
		int pbW = 360;
		int pbH = 120; // Native Aspect Ratio 2172:724 = 3.0
		int pbX = SCREEN_W - pbW - 20;
		int pbY = SCREEN_H - pbH - 25;
		iShowImage(pbX, pbY, pbW, pbH, imgProgressBar);

		double pbSlotCenterX = pbX + (pbW * 0.500);
		double pbSlotCenterY = pbY + (pbH * 0.505);

		iSetColor(255, 215, 0);
		char progStr[32];
		if (lvl3Stage == 1) {
			int bl1Remaining = 50 - bl1KillCount;
			if (bl1Remaining < 0) bl1Remaining = 0;
			sprintf_s(progStr, "%d / 50", bl1Remaining);
		}
		else {
			int bl2Remaining = 50 - bl2KillCount;
			if (bl2Remaining < 0) bl2Remaining = 0;
			sprintf_s(progStr, "%d / 50", bl2Remaining);
		}
		iDrawCenteredText(pbSlotCenterX, pbSlotCenterY - iGetFontHeight(GLUT_BITMAP_TIMES_ROMAN_24) * 0.35, progStr, GLUT_BITMAP_TIMES_ROMAN_24);

		// Synchronize progress to binary save
		gSaveSystem.updateLevelProgress(currentLevel, (lvl3Stage == 1) ? (bl1KillCount * 100 / 100) : (50 + bl2KillCount * 50 / 50));
	}
	else if (isBossArena && bossSaint.active && bossSaint.alive) {
		// Saint Health Card (Level 2 Boss Arena - 300 HP) - Native Aspect Ratio (1916:821 = 2.3337)
		drawRightSideHealthCard(imgHealthSaint, enemyCardX, enemyCardY, enemyCardW, enemyCardH, bossSaint.health, 300, GLUT_BITMAP_TIMES_ROMAN_24);
	}
	else {
		// Level 1 & Level 2 Journey: Progress Bar Card (Numerical percentage centered - Native Aspect Ratio 3:1)
		int pbW = 360;
		int pbH = 120;
		int pbX = SCREEN_W - pbW - 20;
		int pbY = SCREEN_H - pbH - 25;
		iShowImage(pbX, pbY, pbW, pbH, imgProgressBar);

		double progress = bgOffset / (maxOffset > 0 ? maxOffset : 1.0);
		if (progress > 1.0) progress = 1.0;

		double pbSlotCenterX = pbX + (pbW * 0.500);
		double pbSlotCenterY = pbY + (pbH * 0.505);

		// Clean numerical percentage horizontally & vertically center-aligned
		iSetColor(255, 215, 0);
		char progStr[32];
		sprintf_s(progStr, "%d%%", (int)(progress * 100.0));
		iDrawCenteredText(pbSlotCenterX, pbSlotCenterY - iGetFontHeight(GLUT_BITMAP_TIMES_ROMAN_24) * 0.35, progStr, GLUT_BITMAP_TIMES_ROMAN_24);

		// Synchronize progress to binary save
		gSaveSystem.updateLevelProgress(currentLevel, (int)(progress * 100.0));
	}

	if (isPaused) {
		iSetColor(255, 255, 255);
		iDrawCenteredText(SCREEN_W / 2, SCREEN_H / 2, "GAME PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
		iDrawCenteredText(SCREEN_W / 2, SCREEN_H / 2 - 30, "Press SPACE to Resume", GLUT_BITMAP_HELVETICA_18);
	}

	iSetColor(230, 230, 230);
	if (currentLevel == 4) {
		iDrawCenteredText(SCREEN_W / 2, 18, "[Arrows/WASD] Move | [Click] Sword Attack | Defeat King Mesh!", GLUT_BITMAP_HELVETICA_12);
	}
	else if (currentLevel == 3) {
		iDrawCenteredText(SCREEN_W / 2, 18, "[N] Summon Army | [M] Manipulate Wave | [Arrows/WASD] Move | [Click] Sword Attack", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		iDrawCenteredText(SCREEN_W / 2, 18, "[D/Right] Move | [W/Up] Jump | [S/Down] Crouch | [Space] Pause | [R] Restart | [Click] Attack", GLUT_BITMAP_HELVETICA_12);
	}
}

#endif

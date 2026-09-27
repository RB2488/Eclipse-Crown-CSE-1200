#include "GameCommon.h"
#include "Player.h"
#include "Enemy.h"
#include "MenuSystem.h"
#include "AudioManager.h"
#include "SaveSystem.h"

/* ================================================================
   GLOBAL VARIABLE DEFINITIONS
   ================================================================ */
GameScreen currentScreen = SCREEN_SPLASH;
CharState  currentState = IDLE;
bool       isPaused = false;
int        currentLevel = 1;
int        storyIndex = 1;
int        optionsPlayerPage = 0;
bool       isNamingPlayer = false;
char       nameInputBuffer[32] = "";
int        nameInputLen = 0;
int        namingTargetSlot = -1;
int        nameInputBlinkTimer = 0;

AudioManager gAudio;
SaveSystem   gSaveSystem;

int imgSplash;
int imgMenu;
int imgGameOver;
int imgYouWin[3];
int imgStory[4];
int youWinFrame = 0;
int youWinTimer = 0;

// Health Cards & Progress Bar Handles
int imgHealthTim;
int imgHealthMesh;
int imgHealthSaint;
int imgHealthArcher;
int imgHealthSoldier;
int imgProgressBar;

// Tracking Variables
int lastDamageSoundHealth = 200;
int lastMeshTeleportThreshold = 400;
int attackQueued = 0;

// Background sequence: B1, then (B2, B3) x3, then B4 (Level 1 and Level 2 Journey)
int tileSeq[NUM_TILES] = { 0, 1, 2, 1, 2, 1, 2, 3 };
int bg[5];
int imgRunRight[9];   // 9-frame run-right animation (Run1.png to Run9.png)
int imgRunLeft[9];    // 9-frame run-left animation (Runl1.png to Runl9.png)
int imgFightRight[5]; // 5-frame fight-right animation (fr1.png to fr5.png)
int imgFightLeft[5];  // 5-frame fight-left animation (fl1.png to fl5.png)
int imgJump[8];       // 8-frame jump animation (j1.png to j8.png)
int imgIdle;          // 1 idle image (idle.png)
int imgIdleLeft;      // 1 idle-left image (idlel.png)
int imgGhostLeft[2];  // g1.png, g2.png (facing left)
int imgGhostRight[2]; // gl1.png, gl2.png (facing right)
int imgSkeletonRunRight[5];   // sRun1.png to sRun5.png
int imgSkeletonRunLeft[5];    // sRunl1.png to sRunl5.png
int imgSkeletonFightRight[5]; // sFight1.png to sFight5.png
int imgSkeletonFightLeft[5];  // sFightl1.png to sFightl5.png

// Level 3 Backgrounds & Handles
int imgBL1;
int imgBL2;
int imgBL3;
int lvl3Stage = 1; // 1 = BL1 (Soldiers), 2 = BL2 (Archers)

// Level 3 Enemy & Weapon Image Handles
int imgSoldierWalk[5];  // SoilderW1.png to SoilderW5.png
int imgSoldierFight[6]; // f1.png to f6.png
int imgArcherWalk[8];   // walk frame 01..8 without background.png
int imgArcherFight[5];  // fight frame 1..5 without background.png
int imgArrow;           // Arrow.png

// Obstacles & Crouch Assets
Obstacle obstacles[MAX_OBSTACLES];
int ballSpawnCount = 0;
int batSpawnCount = 0;
int lastObstacleSpawnTimer = 0;
int imgBall[5];
int imgBat[3];
int imgSit[3];
int sitTimer = 0;

// Level 1 Stage 2 (B2, B3, B4) Enemy Counters
int lvl1LastStageGhostCount = 0;
int lvl1LastStageSkeletonCount = 0;
int lastLvl1Stage2SpawnTimer = 0;

// Level 2 Boss Fight Minion Counters
int bossGhostSpawnCount = 0;
int bossSkeletonSpawnCount = 0;
int lastBossMinionSpawnTimer = 0;

// Boss Saint Assets & Variables
BossSaint bossSaint;
FlashProjectile bossFlash;
bool isBossArena = false;
bool bossTriggered = false;
int imgSaintWalk[4];
int imgSaintFight[4];
int imgFlash;

// Level 4 Boss Mesh Assets & Variables
BossMesh bossMesh;
int lvl4SoldierSpawnCount = 0;
int lvl4ArcherSpawnCount = 0;
int lastLvl4MinionSpawnTimer = 0;
int imgMeshWalk[5];
int imgMeshFight[5];

// Level 3 Counters & Variables
int soldierSpawnCount = 0;
int soldierKillCount = 0;
int archerSpawnCount = 0;
int archerKillCount = 0;
int lastSoldierSpawnTimer = 0;
int lastArcherSpawnTimer = 0;
bool nSummonedOnce = false;

// Special Powers (N - Army Summon, M - Manipulation)
double nCooldown = 1.0; // 0.0 to 1.0 (starts 100% full)
double mCooldown = 1.0; // 0.0 to 1.0 (starts 100% full)
int manipulationEffectTimer = 0;
double manipulationEffectX = 0.0;
bool manipulationFacingRight = true;
int heroSoldierKillCount = 0;
int lvl3GhostSpawnCount = 0;
int lvl3SkeletonSpawnCount = 0;
int lastLvl3GhostSpawnTimer = 0;
int lastLvl3SkeletonSpawnTimer = 0;

Enemy enemies[MAX_ENEMIES];
Ally  allies[MAX_ALLIES];
ArrowProjectile arrows[MAX_ARROWS];

int globalGameTimer = 0;
int lastSpawnTimer = 0;
int spawnCount = 0;
int killCount = 0;
int ghostKillCount = 0;
int skeletonSpawnCount = 0;
int skeletonRightSpawnCount = 0;
int skeletonLeftSpawnCount = 0;
int skeletonKillCount = 0;
int lastSkeletonSpawnTimer = 0;

bool   facingRight = true;
double bgOffset = 0.0;
double maxOffset = (double)((NUM_TILES - 1) * TILE_W);
int    charFrame = 0;
int    targetStartX = (SCREEN_W - 180) / 2;
int    charX = -200;
double charY = GROUND_Y;
bool   isEntering = false;
bool   levelDone = false;
bool   isExiting = false;
bool   showWinCard = false;
int    playerHealth = 200;

int    idleTimer = 0;
int    fightTimer = 0;
int    runCounter = 0;
bool   isJumping = false;
double jumpVelocity = 0.0;
int    jumpFrame = 0;
int    jumpTimer = 0;

/* ================================================================
   SPECIAL POWERS IMPLEMENTATION (N - SUMMON / DOUBLE, M - MANIPULATE)
   ================================================================ */
void castNPower()
{
	if (currentLevel != 3 || levelDone || isEntering || isExiting || playerHealth <= 0 || isPaused) return;
	if (nCooldown < 1.0) return;

	// Each N Button activation spawns 2 friendly Ghosts and 1 friendly Skeleton from Left side!
	int ghostsSpawned = 0;
	int skeletonsSpawned = 0;
	for (int i = 0; i < MAX_ALLIES; i++) {
		if (!allies[i].active) {
			if (ghostsSpawned < 2) {
				allies[i].type = ALLY_GHOST;
				allies[i].active = true;
				allies[i].alive = true;
				allies[i].health = 50; // Balanced Bot health
				allies[i].maxHealth = 50;
				allies[i].baseY = 260.0 + (rand() % 30) - 15;
				allies[i].y = allies[i].baseY;
				allies[i].x = -60 - (ghostsSpawned * 70);
				allies[i].frame = 0;
				allies[i].timer = 0;
				allies[i].floatTimer = ghostsSpawned * 15;
				allies[i].isFighting = false;
				ghostsSpawned++;
			}
			else if (skeletonsSpawned < 1) {
				allies[i].type = ALLY_SKELETON;
				allies[i].active = true;
				allies[i].alive = true;
				allies[i].health = 70; // Balanced Bot health
				allies[i].maxHealth = 70;
				allies[i].baseY = GROUND_Y;
				allies[i].y = GROUND_Y;
				allies[i].x = -180;
				allies[i].frame = 0;
				allies[i].timer = 0;
				allies[i].isFighting = false;
				allies[i].fightFrame = 0;
				allies[i].fightTimer = 0;
				allies[i].attackTimer = 0;
				skeletonsSpawned++;
				break;
			}
		}
	}

	nCooldown = 0.0; // Reset cooldown
}

void castMPower()
{
	if (currentLevel != 3 || levelDone || isEntering || isExiting || playerHealth <= 0 || isPaused) return;
	if (mCooldown < 1.0) return;

	// Trigger Manipulation Power
	mCooldown = 0.0;
	manipulationEffectTimer = 25; // 25 frames expanding rectangular energy animation
	manipulationFacingRight = facingRight;
	manipulationEffectX = charX;

	// Directional Attack Area: 500 Pixels strictly in facing direction (0 to 500px from hero)
	double rectMinX, rectMaxX;
	if (manipulationFacingRight) {
		rectMinX = charX + 80.0;
		rectMaxX = charX + 580.0;
	}
	else {
		rectMinX = charX + 100.0 - 500.0;
		rectMaxX = charX + 100.0;
	}

	// Defeat all applicable enemies within the 500px directional square/rectangle!
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active && enemies[i].alive) {
			if (enemies[i].x >= rectMinX - 40.0 && enemies[i].x <= rectMaxX + 40.0) {
				enemies[i].health = 0;
				enemies[i].alive = false;
				enemies[i].active = false;
				if (enemies[i].type == ENEMY_SOLDIER) {
					soldierKillCount++;
					heroSoldierKillCount++;
					gSaveSystem.recordValidKill(2);
					// Health recovery: +20 HP per 3 soldier kills by Hero!
					if (heroSoldierKillCount % 3 == 0) {
						playerHealth += 20;
						if (playerHealth > 200) playerHealth = 200;
					}
				}
				else if (enemies[i].type == ENEMY_ARCHER) {
					archerKillCount++;
					gSaveSystem.recordValidKill(3);
				}
				else if (enemies[i].type == ENEMY_GHOST) {
					ghostKillCount++;
					gSaveSystem.recordValidKill(0);
				}
				else if (enemies[i].type == ENEMY_SKELETON) {
					skeletonKillCount++;
					gSaveSystem.recordValidKill(1);
				}
				killCount++;
			}
		}
	}

	// Destroy all incoming arrows in range
	for (int i = 0; i < MAX_ARROWS; i++) {
		if (arrows[i].active) {
			if (arrows[i].x >= rectMinX - 30.0 && arrows[i].x <= rectMaxX + 30.0) {
				arrows[i].active = false;
			}
		}
	}
}

/* ================================================================
   GAME LOGIC & MOVEMENT
   ================================================================ */
void startLevel(int level)
{
	currentLevel = level;
	playerHealth = 200;
	bgOffset = 0.0;
	charX = -200;
	charY = GROUND_Y;
	isEntering = true;
	isExiting = false;
	showWinCard = false;
	isJumping = false;
	jumpVelocity = 0.0;
	jumpFrame = 0;
	jumpTimer = 0;
	levelDone = false;
	isBossArena = false;
	bossTriggered = false;
	currentState = RUN_RIGHT;
	facingRight = true;
	charFrame = 0;
	idleTimer = 0;
	fightTimer = 0;
	attackQueued = 0;
	sitTimer = 0;
	runCounter = 0;
	globalGameTimer = 0;
	lastSpawnTimer = 0;
	spawnCount = 0;
	killCount = 0;
	ghostKillCount = 0;
	skeletonSpawnCount = 0;
	skeletonRightSpawnCount = 0;
	skeletonLeftSpawnCount = 0;
	skeletonKillCount = 0;
	lastSkeletonSpawnTimer = 0;
	lvl1LastStageGhostCount = 0;
	lvl1LastStageSkeletonCount = 0;
	lastLvl1Stage2SpawnTimer = 0;
	bossGhostSpawnCount = 0;
	bossSkeletonSpawnCount = 0;
	lastBossMinionSpawnTimer = 0;
	youWinFrame = 0;
	youWinTimer = 0;
	isPaused = false;

	// Level 3 resets
	soldierSpawnCount = 0;
	soldierKillCount = 0;
	archerSpawnCount = 0;
	archerKillCount = 0;
	lastSoldierSpawnTimer = 0;
	lastArcherSpawnTimer = 0;
	lvl3Stage = 1;
	nCooldown = 1.0;
	mCooldown = 1.0;
	nSummonedOnce = false;
	manipulationEffectTimer = 0;
	heroSoldierKillCount = 0;
	lvl3GhostSpawnCount = 0;
	lvl3SkeletonSpawnCount = 0;
	lastLvl3GhostSpawnTimer = 0;
	lastLvl3SkeletonSpawnTimer = 0;

	initEnemies();
	initObstacles();
	initBoss();
	initAllies();
	initArrows();
	initMesh();

	gAudio.resetDebounceTimers();
	lastDamageSoundHealth = 200;
	lastMeshTeleportThreshold = 400;
	bossMesh.health = 400;
	bossMesh.maxHealth = 400;
	bossSaint.health = 300;
	bossSaint.maxHealth = 300;

	if (level == 3) {
		targetStartX = 180;
		nSummonedOnce = false;
	}
	else if (level == 4) {
		targetStartX = (int)(SCREEN_W * 0.25); // 25% screen width = 320px
		nSummonedOnce = false;
	}
	else {
		targetStartX = (SCREEN_W - 180) / 2;
	}

	// Set tile sequence: B1 -> (B2, B3) x 3 -> B4 (Both Level 1 and Level 2 Journey)
	tileSeq[0] = 0; // B1
	tileSeq[1] = 1; // B2
	tileSeq[2] = 2; // B3
	tileSeq[3] = 1; // B2
	tileSeq[4] = 2; // B3
	tileSeq[5] = 1; // B2
	tileSeq[6] = 2; // B3
	tileSeq[7] = 3; // B4

	currentScreen = SCREEN_GAME;
	gAudio.playGameplayBGM();
	gAudio.playGhostComingSound(0);
}

void restartGame()
{
	startLevel(currentLevel);
}

void advanceStory()
{
	if (storyIndex == 1) {
		storyIndex = 2;
	}
	else if (storyIndex == 2) {
		storyIndex = 3;
	}
	else if (storyIndex == 3) {
		startLevel(1);
	}
	else if (storyIndex == 4) {
		startLevel(3); // Level 2 story epilogue transitions to Level 3!
	}
}

void skipStory()
{
	if (storyIndex <= 3) {
		startLevel(1);
	}
	else {
		startLevel(3);
	}
}

void sit()
{
	if (levelDone || isEntering || isExiting || playerHealth <= 0 || isPaused) return;
	if (isJumping) return;
	if (currentState == FIGHT_RIGHT || currentState == FIGHT_LEFT) return;
	currentState = SIT;
	charFrame = 0;
	sitTimer = 0;
	idleTimer = 0;
}

void moveRight()
{
	if (levelDone || isEntering || isExiting || playerHealth <= 0 || isPaused) return;
	if (currentState == FIGHT_RIGHT || currentState == FIGHT_LEFT) return;

	bool canMoveRight = true;
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active && enemies[i].alive) {
			double dist = enemies[i].x - charX;
			/* Blocking right side */
			if (enemies[i].fromRight && dist > 0 && dist < 140.0) {
				canMoveRight = false;
			}
		}
	}

	/* Boss Saint blocking */
	if (bossSaint.active && bossSaint.alive) {
		double dist = bossSaint.x - charX;
		if (dist > 0 && dist < 150.0) {
			canMoveRight = false;
		}
	}

	/* Boss Mesh blocking (Level 4) */
	if (bossMesh.active && bossMesh.alive) {
		double dist = bossMesh.x - charX;
		if (dist > 0 && dist < 185.0) {
			canMoveRight = false;
		}
	}

	if (currentState == SIT) {
		currentState = IDLE;
	}

	facingRight = true;
	if (!isJumping && currentState != RUN_RIGHT) {
		currentState = RUN_RIGHT;
		charFrame = 0;
	}
	idleTimer = 0;

	// Smooth 9-frame cycle
	if (currentState == RUN_RIGHT) {
		runCounter++;
		if (runCounter > 3) {
			charFrame = (charFrame + 1) % 9;
			runCounter = 0;
		}
	}

	if (currentLevel == 3 || currentLevel == 4 || isBossArena) {
		// Free arena positioning - Faster movement speed in Level 3 & Level 4!
		int speed = (currentLevel == 3 || currentLevel == 4) ? 22 : 8;
		if (canMoveRight && charX < SCREEN_W - 200) {
			charX += speed;
		}
	}
	else {
		// Scrolling journey towards B4
		if (bgOffset < maxOffset && canMoveRight)
		{
			bgOffset += MOVE_SPEED;
			for (int i = 0; i < MAX_ENEMIES; i++) {
				if (enemies[i].active) enemies[i].x -= MOVE_SPEED;
			}
			for (int i = 0; i < MAX_OBSTACLES; i++) {
				if (obstacles[i].active) obstacles[i].x -= MOVE_SPEED;
			}

			if (bgOffset >= maxOffset) {
				bgOffset = maxOffset;

				if (currentLevel == 1) {
					levelDone = true;
					isExiting = true;

					for (int i = 0; i < MAX_ENEMIES; i++) {
						enemies[i].active = false;
						enemies[i].alive = false;
					}
					for (int i = 0; i < MAX_OBSTACLES; i++) {
						obstacles[i].active = false;
					}
				}
				else if (currentLevel == 2) {
					// Level 2 B4 reached: Start exit run from B4 to transition to B5 arena!
					isExiting = true;
					for (int i = 0; i < MAX_ENEMIES; i++) {
						enemies[i].active = false;
						enemies[i].alive = false;
					}
					for (int i = 0; i < MAX_OBSTACLES; i++) {
						obstacles[i].active = false;
					}
				}
			}
		}
	}
}

void moveLeft()
{
	if (levelDone || isEntering || isExiting || playerHealth <= 0 || isPaused) return;
	if (currentState == FIGHT_RIGHT || currentState == FIGHT_LEFT) return;

	bool canMoveLeft = true;
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active && enemies[i].alive) {
			double dist = enemies[i].x - charX;
			/* Blocking left side */
			if (!enemies[i].fromRight && dist < 0 && dist > -140.0) {
				canMoveLeft = false;
			}
		}
	}

	/* Boss Saint blocking */
	if (bossSaint.active && bossSaint.alive) {
		double dist = bossSaint.x - charX;
		if (dist < 0 && dist > -150.0) {
			canMoveLeft = false;
		}
	}

	/* Boss Mesh blocking (Level 4) */
	if (bossMesh.active && bossMesh.alive) {
		double dist = bossMesh.x - charX;
		if (dist < 0 && dist > -185.0) {
			canMoveLeft = false;
		}
	}

	if (currentState == SIT) {
		currentState = IDLE;
	}

	facingRight = false;
	if (!isJumping && currentState != RUN_LEFT) {
		currentState = RUN_LEFT;
		charFrame = 0;
	}
	idleTimer = 0;

	// Smooth 9-frame cycle
	if (currentState == RUN_LEFT) {
		runCounter++;
		if (runCounter > 3) {
			charFrame = (charFrame + 1) % 9;
			runCounter = 0;
		}
	}

	if (currentLevel == 3 || currentLevel == 4 || isBossArena) {
		// Free arena movement left - Faster movement speed in Level 3 & Level 4!
		int speed = (currentLevel == 3 || currentLevel == 4) ? 22 : 8;
		if (canMoveLeft && charX > 40) {
			charX -= speed;
		}
	}
	else if (bgOffset > 0 && canMoveLeft)
	{
		bgOffset -= MOVE_SPEED;
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].active) enemies[i].x += MOVE_SPEED;
		}
		for (int i = 0; i < MAX_OBSTACLES; i++) {
			if (obstacles[i].active) obstacles[i].x += MOVE_SPEED;
		}
		if (bgOffset < 0) bgOffset = 0;
	}
}

/* ================================================================
   HERO ATTACK & COMBAT LOGIC
   ================================================================ */
void applyHeroAttackDamage()
{
	bool ghostFightHit = false;
	bool otherEnemyHit = false;

	/* Check flash projectile deflection */
	if (bossFlash.active) {
		double distFlash = bossFlash.x - charX;
		if (distFlash > 0 && distFlash < 240.0) {
			bossFlash.active = false;
		}
	}

	/* Check arrow deflection */
	for (int a = 0; a < MAX_ARROWS; a++) {
		if (arrows[a].active) {
			double distArrow = arrows[a].x - charX;
			if (distArrow > -20.0 && distArrow < 240.0) {
				arrows[a].active = false;
			}
		}
	}

	/* Check Boss Saint hit (Level 2 - Damage: 50 HP per strike) */
	if (bossSaint.active && bossSaint.alive) {
		double distSaint = bossSaint.x - charX;
		bool hitSaint = false;
		if (facingRight && distSaint > 0 && distSaint < 280.0) {
			hitSaint = true;
		}
		else if (!facingRight && distSaint < 0 && distSaint > -280.0) {
			hitSaint = true;
		}

		if (hitSaint) {
			otherEnemyHit = true;
			bossSaint.health -= 50; // 50 HP damage (2x) per strike on Saint
			if (bossSaint.health <= 0) {
				bossSaint.health = 0;
				bossSaint.alive = false;
				bossSaint.active = false;
				levelDone = true;
				isExiting = true;
				gSaveSystem.recordValidKill(4); // Boss Saint kill (+1 Score)
				gSaveSystem.completeLevel(2);

				// Instantly banish all minions (ghosts & skeletons) upon Saint death
				for (int m = 0; m < MAX_ENEMIES; m++) {
					enemies[m].active = false;
					enemies[m].alive = false;
				}
			}
		}
	}

	/* Check Boss Mesh hit (Level 4 - King / Main Villain - Damage: 60 HP per strike) */
	if (bossMesh.active && bossMesh.alive) {
		double distMesh = bossMesh.x - charX;
		bool hitMesh = false;
		if (facingRight && distMesh > 0 && distMesh < 300.0) {
			hitMesh = true;
		}
		else if (!facingRight && distMesh < 0 && distMesh > -300.0) {
			hitMesh = true;
		}

		if (hitMesh) {
			otherEnemyHit = true;
			bossMesh.health -= 60; // 60 HP damage (2x) per strike on King Mesh (Level 4)
			if (bossMesh.health < 0) bossMesh.health = 0;

			// When 50 HP threshold is crossed, teleport!
			int currentThreshold = (bossMesh.health / 50) * 50;
			if (currentThreshold < lastMeshTeleportThreshold && bossMesh.health > 0) {
				lastMeshTeleportThreshold = currentThreshold;
				bossMesh.vanishEffectTimer = 18;

				if (bossMesh.fromRight) {
					// Teleport behind hero (left side) with proper fighting gap
					bossMesh.x = charX - 180.0;
					if (bossMesh.x < 50.0) bossMesh.x = 50.0;
					bossMesh.fromRight = false;
				}
				else {
					// Teleport in front of hero (right side) with proper fighting gap
					bossMesh.x = charX + 180.0;
					if (bossMesh.x > SCREEN_W - 220.0) bossMesh.x = SCREEN_W - 220.0;
					bossMesh.fromRight = true;
				}
				bossMesh.isFighting = false;
				bossMesh.attackCooldown = 0; // Resume fighting immediately from new location!
			}

			if (bossMesh.health <= 0) {
				bossMesh.health = 0;
				bossMesh.alive = false;
				bossMesh.active = false;
				levelDone = true;
				isExiting = true;
				gSaveSystem.recordValidKill(5); // Boss Mesh kill (+1 Score)
				gSaveSystem.completeLevel(4);

				// Instantly banish all minions upon King Mesh death
				for (int m = 0; m < MAX_ENEMIES; m++) {
					enemies[m].active = false;
					enemies[m].alive = false;
				}
			}
		}
	}

	/* Check standard and Level 3/4 enemies hit (Front & Back) */
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active && enemies[i].alive) {
			double dist = enemies[i].x - charX;
			bool hit = false;
			if (facingRight && dist > 0 && dist < 260.0) {
				hit = true;
			}
			else if (!facingRight && dist < 0 && dist > -260.0) {
				hit = true;
			}

			if (hit) {
				if (enemies[i].type == ENEMY_GHOST) {
					ghostFightHit = true;
				}
				else {
					otherEnemyHit = true;
				}

				// Damage power (2x): 40 vs Ghost & Skeleton (Level 1 & 2), 30 vs Soldier & Archer
				int dmg = 30;
				if (enemies[i].type == ENEMY_GHOST || enemies[i].type == ENEMY_SKELETON) {
					dmg = 40;
				}
				else if (enemies[i].type == ENEMY_SOLDIER || enemies[i].type == ENEMY_ARCHER) {
					dmg = 30;
				}

				enemies[i].health -= dmg;
				if (enemies[i].health <= 0) {
					enemies[i].alive = false;
					enemies[i].active = false;
					killCount++;
					gSaveSystem.recordValidKill(enemies[i].type); // +1 Score per valid enemy kill!

					if (currentLevel == 4) {
						// In Level 4 arena, each minion kill grants +25 HP sustain!
						playerHealth += 25;
						if (playerHealth > 200) playerHealth = 200;
					}
					else if (enemies[i].type == ENEMY_GHOST) {
						ghostKillCount++;
						if (currentLevel != 3 && ghostKillCount % 3 == 0) {
							playerHealth += 20;
							if (playerHealth > 200) playerHealth = 200;
						}
					}
					else if (enemies[i].type == ENEMY_SKELETON) {
						skeletonKillCount++;
						if (currentLevel != 3 && skeletonKillCount % 2 == 0) {
							playerHealth += 20;
							if (playerHealth > 200) playerHealth = 200;
						}
					}
					else if (enemies[i].type == ENEMY_SOLDIER) {
						soldierKillCount++;
						heroSoldierKillCount++;
						// Health recovery: +20 HP per 3 soldier kills by Hero!
						if (heroSoldierKillCount % 3 == 0) {
							playerHealth += 20;
							if (playerHealth > 200) playerHealth = 200;
						}
					}
					else if (enemies[i].type == ENEMY_ARCHER) {
						archerKillCount++;
						// Health recovery: +20 HP per 3 archer kills by Hero!
						if (archerKillCount % 3 == 0) {
							playerHealth += 20;
							if (playerHealth > 200) playerHealth = 200;
						}
					}
				}
			}
		}
	}

	// Play combat audio: Ghost vs Other Enemies
	if (ghostFightHit) {
		gAudio.playGhostSwordFightSound(globalGameTimer);
	}
	else {
		gAudio.playHeroSwordFightSound(globalGameTimer);
	}
}

void performHeroAttack()
{
	if (currentScreen != SCREEN_GAME || isPaused || isEntering || isExiting || levelDone || playerHealth <= 0) return;

	if (currentState == FIGHT_RIGHT || currentState == FIGHT_LEFT) {
		// Hero is currently executing an attack (3 images playing).
		// Queue the next attack so it executes ONLY AFTER the current 3 images complete!
		attackQueued = 1;
		return;
	}

	if (facingRight) currentState = FIGHT_RIGHT;
	else currentState = FIGHT_LEFT;
	charFrame = 0;
	fightTimer = 0;
	attackQueued = 0;
	idleTimer = 0;

	applyHeroAttackDamage();
}

/* ================================================================
   PHYSICS & TICK UPDATE
   ================================================================ */
void updatePhysics()
{
	globalGameTimer++;

	bool isGameplayActive = (currentScreen == SCREEN_GAME && !isPaused && !showWinCard && playerHealth > 0);
	gAudio.updateBGM((int)currentScreen, isGameplayActive, globalGameTimer);

	if (isPaused) return;

	if (playerHealth < lastDamageSoundHealth) {
		gAudio.playHeroHurtSound(globalGameTimer);
		lastDamageSoundHealth = playerHealth;
	}
	else if (playerHealth > lastDamageSoundHealth) {
		lastDamageSoundHealth = playerHealth;
	}

	if (showWinCard || currentScreen == SCREEN_END_CARD) {
		youWinTimer++;
		if (youWinTimer > 6) {
			youWinFrame = (youWinFrame + 1) % 3;
			youWinTimer = 0;
		}
		return; // Freeze all game physics, enemy updates, and damage during victory win screen!
	}

	if (currentScreen != SCREEN_GAME) return;

	if (isEntering) {
		facingRight = true;
		currentState = RUN_RIGHT;
		charX += 10;

		runCounter++;
		if (runCounter > 3) {
			charFrame = (charFrame + 1) % 9;
			runCounter = 0;
		}

		if (charX >= targetStartX) {
			charX = targetStartX;
			isEntering = false;
			currentState = IDLE;

			// Trigger Boss Saint rapid entrance when hero enters B5 arena to 25% point
			if (isBossArena && !bossTriggered) {
				bossTriggered = true;
				bossSaint.active = true;
				bossSaint.alive = true;
				bossSaint.health = 300;
				bossSaint.maxHealth = 300;
				bossSaint.x = SCREEN_W + 50;
				bossSaint.y = GROUND_Y;
				bossSaint.isEntering = true;
				bossSaint.isFighting = false;
				bossSaint.walkFrame = 0;
				bossSaint.walkTimer = 0;
				bossSaint.fightFrame = 0;
				bossSaint.fightTimer = 0;
				bossSaint.attackCooldown = 0;
				bossSaint.flashCooldown = 30;
				gAudio.playGhostComingSound(globalGameTimer);
			}

			// Trigger Boss Mesh entrance when hero enters Level 4 BL3 arena to 25% point
			if (currentLevel == 4 && !bossMesh.active && bossMesh.alive) {
				bossMesh.active = true;
				bossMesh.isEntering = true;
				bossMesh.x = SCREEN_W + 60;
				bossMesh.y = GROUND_Y;
				bossMesh.health = 400;
				bossMesh.maxHealth = 400;
				bossMesh.walkFrame = 0;
				bossMesh.walkTimer = 0;
				bossMesh.fightFrame = 0;
				bossMesh.fightTimer = 0;
				bossMesh.attackCooldown = 0;
				bossMesh.teleportTimer = 0;
				bossMesh.damageSinceTeleport = 0;
				bossMesh.fromRight = true;
				gAudio.playGhostComingSound(globalGameTimer);
			}
		}
		return;
	}

	if (isExiting) {
		facingRight = true;
		currentState = RUN_RIGHT;
		charX += 16;

		runCounter++;
		if (runCounter > 3) {
			charFrame = (charFrame + 1) % 9;
			runCounter = 0;
		}

		if (charX > SCREEN_W + 150) {
			if (currentLevel == 1) {
				isExiting = false;
				showWinCard = true;
				gSaveSystem.completeLevel(1);
			}
			else if (currentLevel == 2) {
				if (!isBossArena) {
					// Exited B4 -> Enter B5 Arena at 25% screen width!
					isBossArena = true;
					isExiting = false;
					isEntering = true;
					charX = -120;
					targetStartX = (int)(SCREEN_W * 0.25); // 25% point = 320px
					bossTriggered = false;
					gSaveSystem.updateLevelProgress(2, 50);
				}
				else {
					// Defeated Boss Saint -> Show Victory Win Card!
					isExiting = false;
					showWinCard = true;
					gSaveSystem.completeLevel(2);
				}
			}
			else if (currentLevel == 3) {
				if (lvl3Stage == 1) {
					// Defeated 100 soldiers in BL1 -> Transition to BL2!
					lvl3Stage = 2;
					isExiting = false;
					isEntering = true;
					charX = -120;
					targetStartX = (int)(SCREEN_W * 0.25); // 25% screen width = 320px
					currentState = RUN_RIGHT;
					facingRight = true;
					gSaveSystem.updateLevelProgress(3, 50);

					// Clear old soldiers & projectiles
					for (int i = 0; i < MAX_ENEMIES; i++) {
						enemies[i].active = false;
						enemies[i].alive = false;
					}
					for (int i = 0; i < MAX_ALLIES; i++) {
						allies[i].active = false;
						allies[i].alive = false;
					}
					for (int i = 0; i < MAX_ARROWS; i++) {
						arrows[i].active = false;
					}

					archerSpawnCount = 0;
					archerKillCount = 0;
					lastArcherSpawnTimer = globalGameTimer;
					nCooldown = 1.0;
					mCooldown = 1.0;
				}
				else if (lvl3Stage == 2) {
					// Defeated 100 archers in BL2 -> Show Level 3 Victory Win Card!
					isExiting = false;
					levelDone = true;
					showWinCard = true;
					gSaveSystem.completeLevel(3);

					for (int i = 0; i < MAX_ENEMIES; i++) {
						enemies[i].active = false;
						enemies[i].alive = false;
					}
					for (int a = 0; a < MAX_ALLIES; a++) {
						allies[a].active = false;
						allies[a].alive = false;
					}
					for (int a = 0; a < MAX_ARROWS; a++) {
						arrows[a].active = false;
					}
				}
			}
			else if (currentLevel == 4) {
				// Defeated Boss Mesh -> Level 4 Victory Win Card!
				isExiting = false;
				levelDone = true;
				showWinCard = true;
				gSaveSystem.completeLevel(4);

				for (int i = 0; i < MAX_ENEMIES; i++) {
					enemies[i].active = false;
					enemies[i].alive = false;
				}
				for (int a = 0; a < MAX_ARROWS; a++) {
					arrows[a].active = false;
				}
			}
		}
		return;
	}

	// 3-Frame Hero Fight Animation progression with action locking & attack chaining
	if (currentState == FIGHT_RIGHT || currentState == FIGHT_LEFT) {
		fightTimer++;
		if (fightTimer >= 4) {
			charFrame++;
			fightTimer = 0;
			if (charFrame >= 3) {
				if (attackQueued > 0) {
					// Chain queued attack immediately (plays next 3 images)
					attackQueued = 0;
					charFrame = 0;
					fightTimer = 0;
					if (facingRight) currentState = FIGHT_RIGHT;
					else currentState = FIGHT_LEFT;
					applyHeroAttackDamage();
				}
				else {
					currentState = IDLE;
					charFrame = 0;
				}
			}
		}
	}
	else if (currentState == SIT) {
		sitTimer++;
		if (sitTimer > 4 && charFrame < 2) {
			charFrame++;
		}
		if (sitTimer > 26) { // Crouch duration
			currentState = IDLE;
			charFrame = 0;
			sitTimer = 0;
		}
	}
	else if (!isJumping) {
		idleTimer++;
		if (idleTimer > 8) {
			currentState = IDLE;
			charFrame = 0;
		}
	}

	/* --- Jump Physics & Animation --- */
	if (isJumping) {
		charY += jumpVelocity;
		jumpVelocity -= GRAVITY;

		jumpTimer++;
		if (jumpTimer > 3) {
			if (jumpFrame < 7) jumpFrame++;
			jumpTimer = 0;
		}

		if (currentLevel != 3 && currentLevel != 4 && !isBossArena) {
			if (facingRight) {
				if (bgOffset < maxOffset) {
					bgOffset += 3.5;
					for (int i = 0; i < MAX_ENEMIES; i++) {
						if (enemies[i].active) enemies[i].x -= 3.5;
					}
					for (int i = 0; i < MAX_OBSTACLES; i++) {
						if (obstacles[i].active) obstacles[i].x -= 3.5;
					}
				}
			}
			else {
				if (bgOffset > 0) {
					bgOffset -= 3.5;
					for (int i = 0; i < MAX_ENEMIES; i++) {
						if (enemies[i].active) enemies[i].x += 3.5;
					}
					for (int i = 0; i < MAX_OBSTACLES; i++) {
						if (obstacles[i].active) obstacles[i].x += 3.5;
					}
				}
			}
		}
		else {
			int jumpMoveSpeed = (currentLevel == 3 || currentLevel == 4) ? 12 : 3;
			if (facingRight && charX < SCREEN_W - 200) {
				charX += jumpMoveSpeed;
			}
			else if (!facingRight && charX > 40) {
				charX -= jumpMoveSpeed;
			}
		}

		if (charY <= (double)GROUND_Y)
		{
			charY = GROUND_Y;
			isJumping = false;
			jumpVelocity = 0.0;
			jumpFrame = 0;
			jumpTimer = 0;
		}
	}

	/* Cooldown Recharge for Level 3 Powers */
	if (currentLevel == 3 && !levelDone && !isPaused) {
		if (nCooldown < 1.0) {
			nCooldown += 0.0035; // ~5-6 seconds full recharge
			if (nCooldown > 1.0) nCooldown = 1.0;
		}
		if (mCooldown < 1.0) {
			mCooldown += 0.0028; // ~7-8 seconds full recharge
			if (mCooldown > 1.0) mCooldown = 1.0;
		}
		if (manipulationEffectTimer > 0) {
			manipulationEffectTimer--;
		}
	}

	if (bossMesh.vanishEffectTimer > 0) {
		bossMesh.vanishEffectTimer--;
	}

	/* ================================================================
	   SPAWNING & LEVEL STAGE MECHANICS
	   ================================================================ */
	if (!levelDone && !isEntering && !isBossArena) {
		if (currentLevel == 1) {
			/* --- LEVEL 1 STAGE 1: Tiles 0 to 4 (12 Ghosts + 11 Skeletons = 23 with extra in B2/B3) --- */
			if (bgOffset < 5.0 * TILE_W) {
				if (spawnCount < 12) {
					if (globalGameTimer > 15 && (globalGameTimer - lastSpawnTimer > 40)) {
						for (int i = 0; i < MAX_ENEMIES; i++) {
							if (!enemies[i].active) {
								enemies[i].type = ENEMY_GHOST;
								enemies[i].active = true;
								enemies[i].alive = true;
								enemies[i].health = 100;
								enemies[i].maxHealth = 100;
								enemies[i].baseY = 260.0;
								enemies[i].y = enemies[i].baseY;
								enemies[i].fromRight = (spawnCount % 5 != 0);
								spawnCount++;
								if (enemies[i].fromRight) enemies[i].x = SCREEN_W + 30;
								else enemies[i].x = -80;
								lastSpawnTimer = globalGameTimer;
								gAudio.playGhostComingSound(globalGameTimer);
								break;
							}
						}
					}
				}

				if (skeletonSpawnCount < 11) {
					if (globalGameTimer > 35 && (globalGameTimer - lastSkeletonSpawnTimer > 60)) {
						for (int i = 0; i < MAX_ENEMIES; i++) {
							if (!enemies[i].active) {
								enemies[i].type = ENEMY_SKELETON;
								enemies[i].active = true;
								enemies[i].alive = true;
								enemies[i].health = 150;
								enemies[i].maxHealth = 150;
								enemies[i].baseY = GROUND_Y;
								enemies[i].y = GROUND_Y;
								bool spawnFromRight = (skeletonSpawnCount % 4 != 0);
								enemies[i].fromRight = spawnFromRight;
								if (spawnFromRight) {
									skeletonRightSpawnCount++;
									enemies[i].x = SCREEN_W + 50;
								}
								else {
									skeletonLeftSpawnCount++;
									enemies[i].x = -80;
								}
								enemies[i].frame = 0;
								enemies[i].timer = 0;
								enemies[i].isFighting = false;
								enemies[i].fightFrame = 0;
								enemies[i].fightTimer = 0;
								enemies[i].attackTimer = 0;
								skeletonSpawnCount++;
								lastSkeletonSpawnTimer = globalGameTimer;
								gAudio.playGhostComingSound(globalGameTimer);
								break;
							}
						}
					}
				}
			}
			/* --- LEVEL 1 STAGE 2: Tiles 5 to 7 (7 Ghosts + 9 Skeletons = 16) --- */
			else if (bgOffset >= 5.0 * TILE_W && bgOffset < maxOffset) {
				if (lvl1LastStageGhostCount < 7) {
					if (globalGameTimer - lastSpawnTimer > 42) {
						for (int i = 0; i < MAX_ENEMIES; i++) {
							if (!enemies[i].active) {
								enemies[i].type = ENEMY_GHOST;
								enemies[i].active = true;
								enemies[i].alive = true;
								enemies[i].health = 100;
								enemies[i].maxHealth = 100;
								enemies[i].baseY = 260.0;
								enemies[i].y = enemies[i].baseY;
								enemies[i].fromRight = (lvl1LastStageGhostCount % 4 != 0);
								lvl1LastStageGhostCount++;
								if (enemies[i].fromRight) enemies[i].x = SCREEN_W + 30;
								else enemies[i].x = -80;
								lastSpawnTimer = globalGameTimer;
								gAudio.playGhostComingSound(globalGameTimer);
								break;
							}
						}
					}
				}

				if (lvl1LastStageSkeletonCount < 9) {
					if (globalGameTimer - lastSkeletonSpawnTimer > 60) {
						for (int i = 0; i < MAX_ENEMIES; i++) {
							if (!enemies[i].active) {
								enemies[i].type = ENEMY_SKELETON;
								enemies[i].active = true;
								enemies[i].alive = true;
								enemies[i].health = 150;
								enemies[i].maxHealth = 150;
								enemies[i].baseY = GROUND_Y;
								enemies[i].y = GROUND_Y;
								bool spawnFromRight = (lvl1LastStageSkeletonCount % 3 != 0);
								enemies[i].fromRight = spawnFromRight;
								if (spawnFromRight) enemies[i].x = SCREEN_W + 50;
								else enemies[i].x = -80;
								enemies[i].frame = 0;
								enemies[i].timer = 0;
								enemies[i].isFighting = false;
								enemies[i].fightFrame = 0;
								enemies[i].fightTimer = 0;
								enemies[i].attackTimer = 0;
								lvl1LastStageSkeletonCount++;
								lastSkeletonSpawnTimer = globalGameTimer;
								gAudio.playGhostComingSound(globalGameTimer);
								break;
							}
						}
					}
				}
			}
		}
		else if (currentLevel == 2) {
			/* --- LEVEL 2 STAGE 1: Giant 3X Fireballs & 20-Bat Swarms (3 Waves with Direction Sequence) --- */
			if (bgOffset < 5.0 * TILE_W) {
				int totalWaves = ballSpawnCount + batSpawnCount;
				if (totalWaves < 6) {
					if (globalGameTimer > 25 && (globalGameTimer - lastObstacleSpawnTimer > 110)) {
						bool isBall = (totalWaves % 2 == 0);
						if (ballSpawnCount >= 3) isBall = false;
						if (batSpawnCount >= 3) isBall = true;

						if (isBall) {
							// All 3 Ball Waves: 2x Size Fireball (180x180) strictly from Right to Left!
							for (int i = 0; i < MAX_OBSTACLES; i++) {
								if (!obstacles[i].active) {
									obstacles[i].active = true;
									obstacles[i].hitPlayer = false;
									obstacles[i].type = OBSTACLE_BALL;
									obstacles[i].fromRight = true;
									obstacles[i].x = SCREEN_W + 50;
									obstacles[i].y = GROUND_Y - 5;
									obstacles[i].width = 180; // 2x size
									obstacles[i].height = 180;
									obstacles[i].speed = 9.0;
									obstacles[i].frame = 0;
									obstacles[i].frameTimer = 0;
									ballSpawnCount++;
									lastObstacleSpawnTimer = globalGameTimer;
									break;
								}
							}
						}
						else {
							// All 3 Bat Waves: 20 Bats strictly from Right to Left, flight height tuned and speed boosted!
							int spawnedInFlock = 0;
							for (int i = 0; i < MAX_OBSTACLES && spawnedInFlock < 20; i++) {
								if (!obstacles[i].active) {
									obstacles[i].active = true;
									obstacles[i].hitPlayer = false;
									obstacles[i].type = OBSTACLE_BAT;
									obstacles[i].fromRight = true;
									obstacles[i].x = SCREEN_W + 40 + (spawnedInFlock * 35) + (rand() % 25);
									obstacles[i].y = (GROUND_Y + 180) + ((spawnedInFlock % 4) * 16) - 15 + (rand() % 25);
									double scale = 0.70 + (rand() % 65) * 0.01;
									obstacles[i].width = (int)(100.0 * scale);
									obstacles[i].height = (int)(75.0 * scale);
									obstacles[i].speed = 13.5 + (rand() % 15) * 0.1; // Slightly faster horizontal speed
									obstacles[i].frame = spawnedInFlock % 3;
									obstacles[i].frameTimer = 0;
									spawnedInFlock++;
								}
							}
							batSpawnCount++;
							lastObstacleSpawnTimer = globalGameTimer;
						}
					}
				}
			}
			/* --- LEVEL 2 STAGE 2: 8 Ghosts + 5 Skeletons --- */
			else if (bgOffset >= 5.0 * TILE_W && bgOffset < maxOffset) {
				if (spawnCount < 8) {
					if (globalGameTimer > 20 && (globalGameTimer - lastSpawnTimer > 70)) {
						for (int i = 0; i < MAX_ENEMIES; i++) {
							if (!enemies[i].active) {
								enemies[i].type = ENEMY_GHOST;
								enemies[i].active = true;
								enemies[i].alive = true;
								enemies[i].health = 100;
								enemies[i].maxHealth = 100;
								enemies[i].baseY = 260.0;
								enemies[i].y = enemies[i].baseY;
								enemies[i].fromRight = (spawnCount % 5 != 0);
								spawnCount++;
								if (enemies[i].fromRight) enemies[i].x = SCREEN_W + 30;
								else enemies[i].x = -80;
								lastSpawnTimer = globalGameTimer;
								gAudio.playGhostComingSound(globalGameTimer);
								break;
							}
						}
					}
				}

				if (skeletonSpawnCount < 5) {
					if (globalGameTimer > 40 && (globalGameTimer - lastSkeletonSpawnTimer > 95)) {
						for (int i = 0; i < MAX_ENEMIES; i++) {
							if (!enemies[i].active) {
								enemies[i].type = ENEMY_SKELETON;
								enemies[i].active = true;
								enemies[i].alive = true;
								enemies[i].health = 150;
								enemies[i].maxHealth = 150;
								enemies[i].baseY = GROUND_Y;
								enemies[i].y = GROUND_Y;
								bool spawnFromRight = (skeletonSpawnCount % 4 != 0);
								enemies[i].fromRight = spawnFromRight;
								if (spawnFromRight) enemies[i].x = SCREEN_W + 50;
								else enemies[i].x = -80;
								enemies[i].frame = 0;
								enemies[i].timer = 0;
								enemies[i].isFighting = false;
								enemies[i].fightFrame = 0;
								enemies[i].fightTimer = 0;
								enemies[i].attackTimer = 0;
								skeletonSpawnCount++;
								lastSkeletonSpawnTimer = globalGameTimer;
								gAudio.playGhostComingSound(globalGameTimer);
								break;
							}
						}
					}
				}
			}
		}
		else if (currentLevel == 3) {
			/* --- LEVEL 3 STAGE 1: BL1 (100 Soldiers in Squads of 4: 1/3 Left, 2/3 Right) --- */
			if (lvl3Stage == 1) {
				int activeSoldiers = 0;
				for (int i = 0; i < MAX_ENEMIES; i++) {
					if (enemies[i].active && enemies[i].alive && enemies[i].type == ENEMY_SOLDIER) {
						activeSoldiers++;
					}
				}

				// Soldier Spawning: Squads of 4 from Left (1/3) & Right (2/3) up to 100 soldiers
				if (soldierSpawnCount < 100 && activeSoldiers < 10) {
					if (globalGameTimer - lastSoldierSpawnTimer > 70) {
						int squadIdx = soldierSpawnCount / 4;
						bool spawnFromRightSide = (squadIdx % 3 != 2); // 2 squads Right, 1 squad Left (1/3 Left, 2/3 Right)
						int spawnedInGroup = 0;
						int toSpawnThisGroup = 4;
						if (soldierSpawnCount + toSpawnThisGroup > 100) {
							toSpawnThisGroup = 100 - soldierSpawnCount;
						}

						for (int i = 0; i < MAX_ENEMIES && spawnedInGroup < toSpawnThisGroup; i++) {
							if (!enemies[i].active) {
								enemies[i].type = ENEMY_SOLDIER;
								enemies[i].active = true;
								enemies[i].alive = true;
								enemies[i].health = 100;
								enemies[i].maxHealth = 100;
								enemies[i].baseY = GROUND_Y;
								enemies[i].y = GROUND_Y;
								enemies[i].fromRight = spawnFromRightSide;

								if (spawnFromRightSide) {
									enemies[i].x = SCREEN_W + 40 + (spawnedInGroup * 60);
								}
								else {
									enemies[i].x = -80 - (spawnedInGroup * 60);
								}

								enemies[i].frame = spawnedInGroup % 5;
								enemies[i].timer = 0;
								enemies[i].isFighting = false;
								enemies[i].fightFrame = 0;
								enemies[i].fightTimer = 0;
								enemies[i].attackTimer = 0;

								spawnedInGroup++;
								soldierSpawnCount++;
							}
						}
						lastSoldierSpawnTimer = globalGameTimer;
						gAudio.playGhostComingSound(globalGameTimer);
					}
				}

				// Check BL1 Stage Completion: 100 Soldiers Defeated!
				if (soldierKillCount >= 100 && !isExiting) {
					isExiting = true;
					// All friendly bots vanish immediately upon completing BL1
					for (int a = 0; a < MAX_ALLIES; a++) {
						allies[a].active = false;
						allies[a].alive = false;
					}
				}
			}
			/* --- LEVEL 3 STAGE 2: BL2 (100 Archers in Squads of 4: 1/3 Left, 2/3 Right) --- */
			else if (lvl3Stage == 2) {
				int activeArchers = 0;
				for (int i = 0; i < MAX_ENEMIES; i++) {
					if (enemies[i].active && enemies[i].alive && enemies[i].type == ENEMY_ARCHER) {
						activeArchers++;
					}
				}

				// Archer Spawning: Squads of 4 from Left (1/3) & Right (2/3) up to 100 archers
				if (archerSpawnCount < 100 && activeArchers < 8) {
					if (globalGameTimer - lastArcherSpawnTimer > 70) {
						int squadIdx = archerSpawnCount / 4;
						bool spawnFromRightSide = (squadIdx % 3 != 2); // 2 squads Right, 1 squad Left (1/3 Left, 2/3 Right)
						int spawnedInGroup = 0;
						int toSpawnThisGroup = 4;
						if (archerSpawnCount + toSpawnThisGroup > 100) {
							toSpawnThisGroup = 100 - archerSpawnCount;
						}

						for (int i = 0; i < MAX_ENEMIES && spawnedInGroup < toSpawnThisGroup; i++) {
							if (!enemies[i].active) {
								enemies[i].type = ENEMY_ARCHER;
								enemies[i].active = true;
								enemies[i].alive = true;
								enemies[i].health = 120;
								enemies[i].maxHealth = 120;
								enemies[i].baseY = GROUND_Y;
								enemies[i].y = GROUND_Y;
								enemies[i].fromRight = spawnFromRightSide;

								if (spawnFromRightSide) {
									enemies[i].x = SCREEN_W + 40 + (spawnedInGroup * 60);
								}
								else {
									enemies[i].x = -80 - (spawnedInGroup * 60);
								}

								enemies[i].frame = spawnedInGroup % 8;
								enemies[i].timer = 0;
								enemies[i].isFighting = false;
								enemies[i].fightFrame = 0;
								enemies[i].fightTimer = 0;
								enemies[i].attackTimer = 0;
								enemies[i].shootCooldown = 20 + (spawnedInGroup * 15) + (rand() % 20);

								spawnedInGroup++;
								archerSpawnCount++;
							}
						}
						lastArcherSpawnTimer = globalGameTimer;
						gAudio.playGhostComingSound(globalGameTimer);
					}
				}

				// Check BL2 Level 3 Completion: 100 Archers Defeated!
				if (archerKillCount >= 100 && !isExiting && !levelDone) {
					isExiting = true;
					// All friendly bots and enemy arrows vanish immediately upon completing BL2
					for (int a = 0; a < MAX_ALLIES; a++) {
						allies[a].active = false;
						allies[a].alive = false;
					}
					for (int a = 0; a < MAX_ARROWS; a++) {
						arrows[a].active = false;
					}
				}
			}
		}
		else if (currentLevel == 4) {
			/* --- LEVEL 4: BL3 ARENA (Boss Mesh + 5 Soldier & 5 Archer Minions) --- */
			if (bossMesh.alive && !levelDone) {
				lastLvl4MinionSpawnTimer++;

				int activeMinions = 0;
				for (int i = 0; i < MAX_ENEMIES; i++) {
					if (enemies[i].active && enemies[i].alive) activeMinions++;
				}

				// Spawn 5 Soldiers and 5 Archers randomly from both Left and Right
				if (activeMinions < 4 && lastLvl4MinionSpawnTimer > 75) {
					bool canSpawnSoldier = (lvl4SoldierSpawnCount < 5);
					bool canSpawnArcher = (lvl4ArcherSpawnCount < 5);

					if (canSpawnSoldier || canSpawnArcher) {
						bool spawnSoldier = false;
						if (canSpawnSoldier && canSpawnArcher) {
							spawnSoldier = (rand() % 2 == 0);
						}
						else if (canSpawnSoldier) {
							spawnSoldier = true;
						}
						else {
							spawnSoldier = false;
						}

						bool spawnFromRight = (rand() % 2 == 0);

						for (int i = 0; i < MAX_ENEMIES; i++) {
							if (!enemies[i].active) {
								if (spawnSoldier) {
									enemies[i].type = ENEMY_SOLDIER;
									enemies[i].active = true;
									enemies[i].alive = true;
									enemies[i].health = 100;
									enemies[i].maxHealth = 100;
									enemies[i].baseY = GROUND_Y;
									enemies[i].y = GROUND_Y;
									enemies[i].fromRight = spawnFromRight;
									enemies[i].x = spawnFromRight ? (SCREEN_W + 40) : -80;
									enemies[i].frame = 0;
									enemies[i].timer = 0;
									enemies[i].isFighting = false;
									enemies[i].fightFrame = 0;
									enemies[i].fightTimer = 0;
									enemies[i].attackTimer = 0;
									lvl4SoldierSpawnCount++;
								}
								else {
									enemies[i].type = ENEMY_ARCHER;
									enemies[i].active = true;
									enemies[i].alive = true;
									enemies[i].health = 120;
									enemies[i].maxHealth = 120;
									enemies[i].baseY = GROUND_Y;
									enemies[i].y = GROUND_Y;
									enemies[i].fromRight = spawnFromRight;
									enemies[i].x = spawnFromRight ? (SCREEN_W + 40) : -80;
									enemies[i].frame = 0;
									enemies[i].timer = 0;
									enemies[i].isFighting = false;
									enemies[i].fightFrame = 0;
									enemies[i].fightTimer = 0;
									enemies[i].attackTimer = 0;
									enemies[i].shootCooldown = 20 + (rand() % 30);
									lvl4ArcherSpawnCount++;
								}
								lastLvl4MinionSpawnTimer = 0;
								gAudio.playGhostComingSound(globalGameTimer);
								break;
							}
						}
					}
				}
			}
		}
	}

	/* --- LEVEL 2 BOSS ARENA: Minions (3 Ghosts & 2 Skeletons enter from LEFT side) --- */
	if (isBossArena && bossTriggered && bossSaint.alive && !levelDone) {
		lastBossMinionSpawnTimer++;

		if (bossGhostSpawnCount < 3 && lastBossMinionSpawnTimer > 65) {
			for (int i = 0; i < MAX_ENEMIES; i++) {
				if (!enemies[i].active) {
					enemies[i].type = ENEMY_GHOST;
					enemies[i].active = true;
					enemies[i].alive = true;
					enemies[i].health = 100;
					enemies[i].maxHealth = 100;
					enemies[i].baseY = 260.0;
					enemies[i].y = enemies[i].baseY;
					enemies[i].fromRight = false; // Enter from LEFT side!
					enemies[i].x = -80;
					enemies[i].frame = 0;
					enemies[i].timer = 0;
					enemies[i].floatTimer = bossGhostSpawnCount * 20;

					bossGhostSpawnCount++;
					lastBossMinionSpawnTimer = 0;
					gAudio.playGhostComingSound(globalGameTimer);
					break;
				}
			}
		}

		if (bossSkeletonSpawnCount < 2 && lastBossMinionSpawnTimer > 85) {
			for (int i = 0; i < MAX_ENEMIES; i++) {
				if (!enemies[i].active) {
					enemies[i].type = ENEMY_SKELETON;
					enemies[i].active = true;
					enemies[i].alive = true;
					enemies[i].health = 150;
					enemies[i].maxHealth = 150;
					enemies[i].baseY = GROUND_Y;
					enemies[i].y = GROUND_Y;
					enemies[i].fromRight = false; // Enter from LEFT side!
					enemies[i].x = -80;
					enemies[i].frame = 0;
					enemies[i].timer = 0;
					enemies[i].isFighting = false;
					enemies[i].fightFrame = 0;
					enemies[i].fightTimer = 0;
					enemies[i].attackTimer = 0;

					bossSkeletonSpawnCount++;
					lastBossMinionSpawnTimer = 0;
					gAudio.playGhostComingSound(globalGameTimer);
					break;
				}
			}
		}
	}

	/* --- Update Obstacles (Balls & Bats with Left/Right Directions) --- */
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		if (obstacles[i].active) {
			if (obstacles[i].fromRight) {
				obstacles[i].x -= obstacles[i].speed;
			}
			else {
				obstacles[i].x += obstacles[i].speed;
			}

			obstacles[i].frameTimer++;
			if (obstacles[i].frameTimer > 4) {
				if (obstacles[i].type == OBSTACLE_BALL) {
					obstacles[i].frame = (obstacles[i].frame + 1) % 5;
				}
				else {
					obstacles[i].frame = (obstacles[i].frame + 1) % 3;
				}
				obstacles[i].frameTimer = 0;
			}

			if (!obstacles[i].hitPlayer) {
				if (obstacles[i].type == OBSTACLE_BALL) {
					if (obstacles[i].x + obstacles[i].width - 35 >= charX + 20 && obstacles[i].x + 35 <= charX + 140) {
						if (!isJumping || charY <= GROUND_Y + 90) {
							playerHealth -= 25;
							if (playerHealth < 0) playerHealth = 0;
							obstacles[i].hitPlayer = true;
						}
					}
				}
				else if (obstacles[i].type == OBSTACLE_BAT) {
					if (obstacles[i].x + obstacles[i].width - 15 >= charX + 20 && obstacles[i].x + 15 <= charX + 140) {
						if (currentState != SIT) {
							playerHealth -= 12;
							if (playerHealth < 0) playerHealth = 0;
							obstacles[i].hitPlayer = true;
						}
					}
				}
			}

			if (obstacles[i].fromRight && obstacles[i].x < -400) {
				obstacles[i].active = false;
			}
			else if (!obstacles[i].fromRight && obstacles[i].x > SCREEN_W + 400) {
				obstacles[i].active = false;
			}
		}
	}

	/* --- Update Arrow Projectiles (Level 3 Archers) --- */
	for (int i = 0; i < MAX_ARROWS; i++) {
		if (arrows[i].active) {
			if (arrows[i].fromRight) {
				arrows[i].x -= arrows[i].speed;
			}
			else {
				arrows[i].x += arrows[i].speed;
			}

			// Sword Deflection
			if ((currentState == FIGHT_RIGHT || currentState == FIGHT_LEFT)) {
				double distArrow = arrows[i].x - charX;
				if (distArrow >= -30.0 && distArrow <= 240.0) {
					arrows[i].active = false; // Deflected and destroyed!
				}
			}

			// Hit Hero (crouch can dodge high arrows)
			if (arrows[i].active && arrows[i].x <= charX + 110 && arrows[i].x >= charX - 20) {
				if (currentState != SIT) {
					playerHealth -= 10;
					if (playerHealth < 0) playerHealth = 0;
					arrows[i].active = false;
				}
			}

			// Hit Friendly Allies
			if (arrows[i].active) {
				for (int a = 0; a < MAX_ALLIES; a++) {
					if (allies[a].active && allies[a].alive) {
						if (arrows[i].x <= allies[a].x + 110 && arrows[i].x >= allies[a].x - 20) {
							allies[a].health -= 10;
							if (allies[a].health <= 0) {
								allies[a].health = 0;
								allies[a].alive = false;
								allies[a].active = false;
							}
							arrows[i].active = false;
							break;
						}
					}
				}
			}

			if (arrows[i].x < -100 || arrows[i].x > SCREEN_W + 100) {
				arrows[i].active = false;
			}
		}
	}

	/* --- Update Friendly Summoned Allies (N Power Horde) --- */
	for (int i = 0; i < MAX_ALLIES; i++) {
		if (allies[i].active && allies[i].alive) {
			// Find closest active enemy strictly IN FRONT of the ally (enemies[e].x >= allies[i].x)
			int targetIdx = -1;
			double minDist = 9999.0;
			for (int e = 0; e < MAX_ENEMIES; e++) {
				if (enemies[e].active && enemies[e].alive) {
					if (enemies[e].x >= allies[i].x) {
						double dist = enemies[e].x - allies[i].x;
						if (dist < minDist) {
							minDist = dist;
							targetIdx = e;
						}
					}
				}
			}

			if (allies[i].type == ALLY_GHOST) {
				allies[i].timer++;
				if (allies[i].timer > 6) {
					allies[i].frame = (allies[i].frame + 1) % 2;
					allies[i].timer = 0;
				}

				allies[i].floatTimer++;
				allies[i].y = allies[i].baseY + 18.0 * sin(allies[i].floatTimer * 0.1);

				if (targetIdx != -1) {
					if (minDist > 100.0) {
						// Advance strictly forward (right) towards enemy target in front
						if (allies[i].x < SCREEN_W * 0.75) {
							allies[i].x += 4.5;
						}
					}
					else {
						// In combat range in front: attack enemy with Ghost Damage Power = 5
						if (allies[i].floatTimer % 18 == 0) {
							enemies[targetIdx].health -= 5;
							if (enemies[targetIdx].health <= 0) {
								enemies[targetIdx].alive = false;
								enemies[targetIdx].active = false;
								if (enemies[targetIdx].type == ENEMY_SOLDIER) soldierKillCount++;
								else if (enemies[targetIdx].type == ENEMY_ARCHER) archerKillCount++;
								killCount++;
								gSaveSystem.recordValidKill(enemies[targetIdx].type);
							}
						}
					}
				}
				else {
					// No enemy in front: advance strictly forward up to 75% of screen width
					if (allies[i].x < SCREEN_W * 0.75) {
						allies[i].x += 4.0;
					}
				}

				// Clamp strictly to 75% screen width max
				if (allies[i].x > SCREEN_W * 0.75) {
					allies[i].x = SCREEN_W * 0.75;
				}
			}
			else if (allies[i].type == ALLY_SKELETON) {
				allies[i].y = GROUND_Y;

				if (targetIdx != -1) {
					if (minDist > 110.0) {
						allies[i].isFighting = false;
						// Advance strictly forward (right) towards enemy target in front
						if (allies[i].x < SCREEN_W * 0.75) {
							allies[i].x += 4.8;
						}
						allies[i].timer++;
						if (allies[i].timer > 4) {
							allies[i].frame = (allies[i].frame + 1) % 5;
							allies[i].timer = 0;
						}
					}
					else {
						// In combat range in front: fight and attack
						allies[i].isFighting = true;
						allies[i].fightTimer++;
						if (allies[i].fightTimer > 4) {
							allies[i].fightFrame = (allies[i].fightFrame + 1) % 5;
							allies[i].fightTimer = 0;
						}

						allies[i].attackTimer++;
						if (allies[i].attackTimer >= 18) {
							// Skeleton Damage Power = 10 on Soldier / Archer
							enemies[targetIdx].health -= 10;
							if (enemies[targetIdx].health <= 0) {
								enemies[targetIdx].alive = false;
								enemies[targetIdx].active = false;
								if (enemies[targetIdx].type == ENEMY_SOLDIER) soldierKillCount++;
								else if (enemies[targetIdx].type == ENEMY_ARCHER) archerKillCount++;
								killCount++;
								gSaveSystem.recordValidKill(enemies[targetIdx].type);
							}
							allies[i].attackTimer = 0;
						}
					}
				}
				else {
					allies[i].isFighting = false;
					if (allies[i].x < SCREEN_W * 0.75) {
						allies[i].x += 4.2;
					}
					allies[i].timer++;
					if (allies[i].timer > 4) {
						allies[i].frame = (allies[i].frame + 1) % 5;
						allies[i].timer = 0;
					}
				}

				// Clamp strictly to 75% screen width max
				if (allies[i].x > SCREEN_W * 0.75) {
					allies[i].x = SCREEN_W * 0.75;
				}
			}
		}
	}

	/* --- Update Active Standard & Level 3 Enemies --- */
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active && enemies[i].alive) {
			if (levelDone) continue;

			if (enemies[i].type == ENEMY_GHOST) {
				enemies[i].timer++;
				if (enemies[i].timer > 6) {
					enemies[i].frame = (enemies[i].frame + 1) % 2;
					enemies[i].timer = 0;
				}

				enemies[i].floatTimer++;
				enemies[i].y = enemies[i].baseY + 20.0 * sin(enemies[i].floatTimer * 0.1);

				double targetDist = 110.0;
				double dist = enemies[i].x - charX;

				if (enemies[i].fromRight) {
					// In Level 3, Ghost cannot move beyond 65% of screen width (stops at 35% from left = 448px)
					if (currentLevel == 3) {
						if (enemies[i].x > SCREEN_W * 0.35 && dist > targetDist) {
							enemies[i].x -= 4.0;
						}
					}
					else {
						if (dist > targetDist) enemies[i].x -= 4.0;
					}
				}
				else {
					if (currentLevel == 3) {
						if (enemies[i].x < SCREEN_W * 0.65 && dist < -targetDist) {
							enemies[i].x += 5.5;
						}
					}
					else {
						if (dist < -targetDist) enemies[i].x += 6.0;
					}
				}

				if (abs(dist) <= targetDist + 15.0) {
					if (enemies[i].floatTimer % 20 == 0) {
						playerHealth -= 5;
						if (playerHealth < 0) playerHealth = 0;
					}
				}

				if (enemies[i].x < -200 || enemies[i].x > SCREEN_W + 200) {
					enemies[i].active = false;
				}
			}
			else if (enemies[i].type == ENEMY_SKELETON) {
				enemies[i].y = GROUND_Y;
				double targetDist = 115.0;
				double dist = enemies[i].x - charX;

				if (dist > targetDist + 10.0) {
					enemies[i].isFighting = false;
					enemies[i].fromRight = true;
					// In Level 3, Skeleton cannot move beyond 65% of screen width
					if (currentLevel == 3) {
						if (enemies[i].x > SCREEN_W * 0.35) {
							enemies[i].x -= 5.2;
						}
					}
					else {
						enemies[i].x -= 5.5;
					}

					enemies[i].timer++;
					if (enemies[i].timer > 4) {
						enemies[i].frame = (enemies[i].frame + 1) % 5;
						enemies[i].timer = 0;
					}
				}
				else if (dist < -targetDist - 10.0) {
					enemies[i].isFighting = false;
					enemies[i].fromRight = false;
					if (currentLevel == 3) {
						if (enemies[i].x < SCREEN_W * 0.65) {
							enemies[i].x += 5.2;
						}
					}
					else {
						enemies[i].x += 5.2;
					}

					enemies[i].timer++;
					if (enemies[i].timer > 4) {
						enemies[i].frame = (enemies[i].frame + 1) % 5;
						enemies[i].timer = 0;
					}
				}
				else {
					enemies[i].isFighting = true;
					if (dist >= 0) enemies[i].fromRight = true;
					else enemies[i].fromRight = false;

					enemies[i].fightTimer++;
					if (enemies[i].fightTimer > 5) {
						enemies[i].fightFrame = (enemies[i].fightFrame + 1) % 5;
						enemies[i].fightTimer = 0;
					}

					enemies[i].attackTimer++;
					if (enemies[i].attackTimer >= 20) {
						playerHealth -= 10;
						if (playerHealth < 0) playerHealth = 0;
						enemies[i].attackTimer = 0;
					}
				}
			}
			else if (enemies[i].type == ENEMY_SOLDIER) {
				enemies[i].y = GROUND_Y;

				// Find closest target between Hero and all active Allies
				double distHero = abs(enemies[i].x - charX);
				int allyTargetIdx = -1;
				double minAllyDist = 9999.0;
				for (int a = 0; a < MAX_ALLIES; a++) {
					if (allies[a].active && allies[a].alive) {
						double d = abs(enemies[i].x - allies[a].x);
						if (d < minAllyDist) {
							minAllyDist = d;
							allyTargetIdx = a;
						}
					}
				}

				// Target is closest between Hero and Ally
				bool targetIsAlly = (allyTargetIdx != -1 && minAllyDist < distHero);
				double combatDist = targetIsAlly ? minAllyDist : distHero;
				double targetX = targetIsAlly ? allies[allyTargetIdx].x : charX;

				// Set facing direction based on target position
				enemies[i].fromRight = (enemies[i].x >= targetX);

				if (combatDist > 115.0) {
					enemies[i].isFighting = false;
					if (enemies[i].x > targetX) {
						enemies[i].x -= 5.2;
					}
					else {
						enemies[i].x += 5.2;
					}

					enemies[i].timer++;
					if (enemies[i].timer > 3) {
						enemies[i].frame = (enemies[i].frame + 1) % 5;
						enemies[i].timer = 0;
					}
				}
				else {
					// In combat range: attack target!
					enemies[i].isFighting = true;
					enemies[i].fightTimer++;
					if (enemies[i].fightTimer > 4) {
						enemies[i].fightFrame = (enemies[i].fightFrame + 1) % 6;
						enemies[i].fightTimer = 0;
					}

					enemies[i].attackTimer++;
					if (enemies[i].attackTimer >= 20) {
						if (targetIsAlly && allyTargetIdx != -1) {
							allies[allyTargetIdx].health -= 10; // 10 damage to ally
							if (allies[allyTargetIdx].health <= 0) {
								allies[allyTargetIdx].health = 0;
								allies[allyTargetIdx].alive = false;
								allies[allyTargetIdx].active = false;
							}
						}
						else {
							playerHealth -= 10;
							if (playerHealth < 0) playerHealth = 0;
						}
						enemies[i].attackTimer = 0;
					}
				}
			}
			else if (enemies[i].type == ENEMY_ARCHER) {
				enemies[i].y = GROUND_Y;

				// Target is closest between Hero and Allies
				double distHero = abs(enemies[i].x - charX);
				double dist = distHero;
				double targetX = charX;

				for (int a = 0; a < MAX_ALLIES; a++) {
					if (allies[a].active && allies[a].alive) {
						double d = abs(enemies[i].x - allies[a].x);
						if (d < dist) {
							dist = d;
							targetX = allies[a].x;
						}
					}
				}

				// Set facing direction based on target position
				enemies[i].fromRight = (enemies[i].x >= targetX);

				// Archer must fully enter inside the screen before shooting (even if target is within 0-500px)
				bool isArcherOnScreen = (enemies[i].x >= 120.0 && enemies[i].x <= SCREEN_W - 320.0);

				if (!isArcherOnScreen || dist > 450.0) {
					// Advance onto the screen and closer towards target (strictly NO shooting offscreen or on screen boundary!)
					enemies[i].isFighting = false;
					enemies[i].fightFrame = 0;
					enemies[i].fightTimer = 0;
					enemies[i].shootCooldown = 0;

					if (!isArcherOnScreen) {
						// Force movement onto the visible screen
						if (enemies[i].x > SCREEN_W - 320.0) {
							enemies[i].x -= 3.5;
						}
						else if (enemies[i].x < 120.0) {
							enemies[i].x += 3.5;
						}
					}
					else {
						if (enemies[i].x > targetX) {
							enemies[i].x -= 3.5;
						}
						else {
							enemies[i].x += 3.5;
						}
					}

					enemies[i].timer++;
					if (enemies[i].timer > 3) {
						enemies[i].frame = (enemies[i].frame + 1) % 8; // 8 walk frames
						enemies[i].timer = 0;
					}
				}
				else {
					// On screen AND within range (0 to 450px) -> Shoot Arrows!
					enemies[i].shootCooldown++;
					if (enemies[i].shootCooldown >= 50) {
						enemies[i].isFighting = true;
						enemies[i].fightTimer++;
						if (enemies[i].fightTimer > 4) {
							enemies[i].fightFrame = (enemies[i].fightFrame + 1) % 5;
							enemies[i].fightTimer = 0;

							// Release frame (frame 2): spawn arrow
							if (enemies[i].fightFrame == 2) {
								for (int a = 0; a < MAX_ARROWS; a++) {
									if (!arrows[a].active) {
										arrows[a].active = true;
										arrows[a].fromRight = enemies[i].fromRight;
										if (enemies[i].fromRight) {
											arrows[a].x = enemies[i].x - 30;
										}
										else {
											arrows[a].x = enemies[i].x + 180;
										}
										arrows[a].y = GROUND_Y + 125;
										arrows[a].speed = 12.0;
										break;
									}
								}
							}

							if (enemies[i].fightFrame >= 4) {
								enemies[i].isFighting = false;
								enemies[i].fightFrame = 0;
								enemies[i].shootCooldown = 0;
							}
						}
					}
					else {
						enemies[i].isFighting = false;
					}
				}
			}
		}
	}

	/* ================================================================
	   BOSS SAINT & FLASH PROJECTILE UPDATE
	   ================================================================ */
	if (bossSaint.active && bossSaint.alive) {
		if (bossSaint.isEntering) {
			bossSaint.x -= 5.2;
			bossSaint.walkTimer++;
			if (bossSaint.walkTimer > 4) {
				bossSaint.walkFrame = (bossSaint.walkFrame + 1) % 4;
				bossSaint.walkTimer = 0;
			}
			if (bossSaint.x <= SCREEN_W - 320) {
				bossSaint.x = SCREEN_W - 320;
				bossSaint.isEntering = false;
			}
		}
		else if (!levelDone) {
			double dist = bossSaint.x - charX;

			bossSaint.flashCooldown++;
			if (bossSaint.flashCooldown >= 80 && !bossFlash.active && dist > 160.0) {
				bossFlash.active = true;
				bossFlash.x = bossSaint.x - 30;
				bossFlash.y = GROUND_Y + 70;
				bossFlash.speed = 10.0;
				bossSaint.flashCooldown = 0;
				bossSaint.isFighting = true;
				bossSaint.fightFrame = 1;
			}

			if (dist <= 150.0 && dist >= -50.0) {
				bossSaint.isFighting = true;
				bossSaint.fightTimer++;
				if (bossSaint.fightTimer > 4) {
					bossSaint.fightFrame = (bossSaint.fightFrame + 1) % 4;
					bossSaint.fightTimer = 0;
				}

				bossSaint.attackCooldown++;
				if (bossSaint.attackCooldown >= 16) {
					playerHealth -= 15;
					if (playerHealth < 0) playerHealth = 0;
					bossSaint.attackCooldown = 0;
				}
			}
			else {
				bossSaint.isFighting = false;
				bossSaint.walkTimer++;
				if (bossSaint.walkTimer > 4) {
					bossSaint.walkFrame = (bossSaint.walkFrame + 1) % 4;
					bossSaint.walkTimer = 0;
				}

				if (dist > 150.0) {
					bossSaint.x -= 4.8;
				}
				else if (dist < 80.0) {
					bossSaint.x += 4.8;
				}
			}
		}
	}

	// Update Flash projectile
	if (bossFlash.active) {
		bossFlash.x -= bossFlash.speed;

		if ((currentState == FIGHT_RIGHT || currentState == FIGHT_LEFT)) {
			if (bossFlash.x >= charX + 20 && bossFlash.x <= charX + 240) {
				bossFlash.active = false;
			}
		}

		if (bossFlash.active && bossFlash.x <= charX + 110 && bossFlash.x >= charX - 30) {
			playerHealth -= 15;
			if (playerHealth < 0) playerHealth = 0;
			bossFlash.active = false;
		}

		if (bossFlash.x < -100) {
			bossFlash.active = false;
		}
	}

	/* ================================================================
	   BOSS MESH (KING / MAIN VILLAIN - LEVEL 4) UPDATE
	   ================================================================ */
	if (bossMesh.active && bossMesh.alive) {
		if (bossMesh.isEntering) {
			bossMesh.x -= 4.5;
			bossMesh.walkTimer++;
			if (bossMesh.walkTimer > 4) {
				bossMesh.walkFrame = (bossMesh.walkFrame + 1) % 5;
				bossMesh.walkTimer = 0;
			}
			if (bossMesh.x <= SCREEN_W - 320) {
				bossMesh.x = SCREEN_W - 320;
				bossMesh.isEntering = false;
			}
		}
		else if (!levelDone) {
			// Determine facing direction relative to Hero
			bossMesh.fromRight = (bossMesh.x >= charX);

			double dist = abs(bossMesh.x - charX);

			// Combat & Movement with Natural Fighting Distance (no sprite overlap)
			if (dist <= 185.0) {
				bossMesh.isFighting = true;
				bossMesh.fightTimer++;
				if (bossMesh.fightTimer > 4) {
					bossMesh.fightFrame = (bossMesh.fightFrame + 1) % 5;
					bossMesh.fightTimer = 0;
				}

				bossMesh.attackCooldown++;
				if (bossMesh.attackCooldown >= 18) {
					playerHealth -= 15;
					if (playerHealth < 0) playerHealth = 0;
					bossMesh.attackCooldown = 0;
				}
			}
			else {
				bossMesh.isFighting = false;
				bossMesh.walkTimer++;
				if (bossMesh.walkTimer > 4) {
					bossMesh.walkFrame = (bossMesh.walkFrame + 1) % 5;
					bossMesh.walkTimer = 0;
				}

				if (bossMesh.x > charX + 175.0) {
					bossMesh.x -= 4.2;
				}
				else if (bossMesh.x < charX - 175.0) {
					bossMesh.x += 4.2;
				}
			}
		}
	}
}

/* ================================================================
   CIRCULAR COOLDOWN DIAL HUD RENDERER
   ================================================================ */
void drawCooldownDial(double cx, double cy, double radius, double progress, const char* keyLetter, const char* label, bool ready)
{
	// 1. Dark outer container
	iSetColor(15, 18, 30);
	iFilledCircle(cx, cy, radius + 4);

	// 2. White / Light Background base
	iSetColor(240, 240, 248);
	iFilledCircle(cx, cy, radius);

	// 3. Blue Circular Filling Arc (0.0 to 1.0)
	if (progress > 0.0) {
		if (ready) iSetColor(0, 215, 255); // Radiant Cyan
		else iSetColor(30, 144, 255);      // Vivid Blue

		int numSegments = 40;
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
	iFilledCircle(cx, cy, radius - 6);

	// 5. Border Ring
	if (ready) {
		iSetColor(255, 215, 0); // Gold border when ready!
	}
	else {
		iSetColor(130, 150, 190);
	}
	iCircle(cx, cy, radius);
	iCircle(cx, cy, radius + 1);

	// 6. Center Hotkey Letter
	if (ready) iSetColor(255, 255, 255);
	else iSetColor(170, 180, 200);
	iText(cx - 7, cy - 7, (char*)keyLetter, GLUT_BITMAP_TIMES_ROMAN_24);

	// 7. Label and Status underneath
	iSetColor(255, 255, 255);
	iText(cx - 24, cy - radius - 15, (char*)label, GLUT_BITMAP_HELVETICA_10);
	if (ready) {
		iSetColor(50, 255, 120);
		iText(cx - 18, cy - radius - 26, "READY", GLUT_BITMAP_HELVETICA_10);
	}
	else {
		iSetColor(180, 210, 240);
		char pctStr[16];
		sprintf_s(pctStr, "%d%%", (int)(progress * 100.0));
		iText(cx - 10, cy - radius - 26, pctStr, GLUT_BITMAP_HELVETICA_10);
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
		iText(SCREEN_W / 2 - 180, 50, "Press 'R' to Restart | 'H' for Home | ESC to Exit", GLUT_BITMAP_HELVETICA_18);
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
		iText(SCREEN_W / 2 - 240, 84, winTitle, GLUT_BITMAP_TIMES_ROMAN_24);

		iSetColor(255, 215, 0);
		if (currentLevel == 1) {
			iText(SCREEN_W / 2 - 365, 58, "Press 'N' / [Right Arrow] for Level 2 | 'R' to Restart | 'H' for Home | ESC to Exit", GLUT_BITMAP_HELVETICA_18);
		}
		else if (currentLevel == 2) {
			iText(SCREEN_W / 2 - 365, 58, "Press 'N' / [Right Arrow] for Level 3 | 'R' to Restart | 'H' for Home | ESC to Exit", GLUT_BITMAP_HELVETICA_18);
		}
		else if (currentLevel == 3) {
			iText(SCREEN_W / 2 - 380, 58, "Press 'N' / [Right Arrow] for Final Boss (Level 4) | 'R' to Restart | 'H' for Home | ESC to Exit", GLUT_BITMAP_HELVETICA_18);
		}
		else {
			iText(SCREEN_W / 2 - 370, 58, "Press 'N' / [Right Arrow] / [Enter] for Victory End Card | 'R' to Restart | 'H' for Home", GLUT_BITMAP_HELVETICA_18);
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
		iText((int)bossSaint.x + 85, (int)bossSaint.y + 288, "SAINT", GLUT_BITMAP_HELVETICA_12);
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

	// HERO / TIM HEALTH CARD (ALWAYS ON TOP-LEFT) - Large, high-definition, transparent PNG
	int cardW = 310;
	int cardH = 120;
	int timCardX = 25;
	int timCardY = SCREEN_H - 130;
	iShowImage(timCardX, timCardY, cardW, cardH, imgHealthTim);

	// Red HP text centered inside Tim card's slot
	iSetColor(245, 30, 30);
	char timHpText[32];
	sprintf_s(timHpText, "%d / 200", playerHealth);
	iText(timCardX + 125, timCardY + 48, timHpText, GLUT_BITMAP_TIMES_ROMAN_24);

	/* Top-Center Special Abilities HUD (N & M Cooldown Dials for Level 3) */
	if (currentLevel == 3) {
		drawCooldownDial(SCREEN_W / 2 - 65, SCREEN_H - 42, 24, nCooldown, "N", "SUMMON", (nCooldown >= 1.0));
		drawCooldownDial(SCREEN_W / 2 + 65, SCREEN_H - 42, 24, mCooldown, "M", "MANIPULATE", (mCooldown >= 1.0));
	}

	// ENEMY / BOSS HEALTH CARDS OR PROGRESS BAR (ALWAYS ON TOP-RIGHT) - Large, transparent PNG
	int enemyCardX = SCREEN_W - 335;
	int enemyCardY = SCREEN_H - 130;

	if (currentLevel == 4) {
		// King Mesh Health Card (Level 4 - 400 HP)
		iShowImage(enemyCardX, enemyCardY, cardW, cardH, imgHealthMesh);
		iSetColor(245, 30, 30);
		char meshHpText[32];
		sprintf_s(meshHpText, "%d / 400", bossMesh.health);
		iText(enemyCardX + 50, enemyCardY + 48, meshHpText, GLUT_BITMAP_TIMES_ROMAN_24);
	}
	else if (currentLevel == 3) {
		if (lvl3Stage == 1) {
			// Soldier Health Card (Level 3 Stage 1)
			iShowImage(enemyCardX, enemyCardY, cardW, cardH, imgHealthSoldier);
			iSetColor(245, 30, 30);
			char soldierText[32];
			int soldiersRemaining = 100 - soldierKillCount;
			if (soldiersRemaining < 0) soldiersRemaining = 0;
			sprintf_s(soldierText, "%d / 100", soldiersRemaining);
			iText(enemyCardX + 50, enemyCardY + 48, soldierText, GLUT_BITMAP_TIMES_ROMAN_24);
		}
		else {
			// Archer Health Card (Level 3 Stage 2)
			iShowImage(enemyCardX, enemyCardY, cardW, cardH, imgHealthArcher);
			iSetColor(245, 30, 30);
			char archerText[32];
			int archersRemaining = 100 - archerKillCount;
			if (archersRemaining < 0) archersRemaining = 0;
			sprintf_s(archerText, "%d / 100", archersRemaining);
			iText(enemyCardX + 50, enemyCardY + 48, archerText, GLUT_BITMAP_TIMES_ROMAN_24);
		}
	}
	else if (isBossArena && bossSaint.active && bossSaint.alive) {
		// Saint Health Card (Level 2 Boss Arena - 300 HP)
		iShowImage(enemyCardX, enemyCardY, cardW, cardH, imgHealthSaint);
		iSetColor(245, 30, 30);
		char saintHpText[32];
		sprintf_s(saintHpText, "%d / 300", bossSaint.health);
		iText(enemyCardX + 50, enemyCardY + 48, saintHpText, GLUT_BITMAP_TIMES_ROMAN_24);
	}
	else {
		// Level 1 & Level 2 Journey: Progress Bar Card (Numerical percentage centered - NO blue fill)
		int pbW = 310;
		int pbH = 100;
		int pbX = SCREEN_W - 335;
		int pbY = SCREEN_H - 120;
		iShowImage(pbX, pbY, pbW, pbH, imgProgressBar);

		double progress = bgOffset / (maxOffset > 0 ? maxOffset : 1.0);
		if (progress > 1.0) progress = 1.0;

		// Clean numerical percentage horizontally & vertically center-aligned
		iSetColor(255, 215, 0);
		char progStr[32];
		sprintf_s(progStr, "%d%%", (int)(progress * 100.0));
		iText(pbX + 130, pbY + 38, progStr, GLUT_BITMAP_TIMES_ROMAN_24);

		// Synchronize progress to binary save
		gSaveSystem.updateLevelProgress(currentLevel, (int)(progress * 100.0));
	}

	if (isPaused) {
		iSetColor(255, 255, 255);
		iText(SCREEN_W / 2 - 80, SCREEN_H / 2, "GAME PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
		iText(SCREEN_W / 2 - 120, SCREEN_H / 2 - 30, "Press SPACE to Resume", GLUT_BITMAP_HELVETICA_18);
	}

	iSetColor(230, 230, 230);
	if (currentLevel == 4) {
		iText(SCREEN_W / 2 - 300, 18, "[Arrows/WASD] Move | [Click] Sword Attack | Defeat King Mesh!", GLUT_BITMAP_HELVETICA_12);
	}
	else if (currentLevel == 3) {
		iText(SCREEN_W / 2 - 340, 18, "[N] Summon Army | [M] Manipulate Wave | [Arrows/WASD] Move | [Click] Sword Attack", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		iText(SCREEN_W / 2 - 360 + 100, 18, "[D/Right] Move | [W/Up] Jump | [S/Down] Crouch | [Space] Pause | [R] Restart | [Click] Attack", GLUT_BITMAP_HELVETICA_12);
	}
}

void iDraw()
{
	iClear();
	if (currentScreen == SCREEN_SPLASH)   { drawSplash();  return; }
	if (currentScreen == SCREEN_MENU)     { drawMenu();    return; }
	if (currentScreen == SCREEN_OPTIONS)  { drawOptions(); return; }
	if (currentScreen == SCREEN_STORY)    { drawStory();   return; }
	if (currentScreen == SCREEN_END_CARD) { drawEndCard(); return; }
	if (currentScreen == SCREEN_CREDITS)  { drawCredits(); return; }
	drawGame();
}

/* ================================================================
   INPUT HANDLERS
   ================================================================ */
void exitGame()
{
	gSaveSystem.clearAllData();
	gAudio.shutdown();
	exit(0);
}

void iKeyboard(unsigned char key)
{
	// 1. If Name Input modal is active, process all typed characters (including 'h', 'f', Enter, Backspace, Esc)
	if (isNamingPlayer) {
		if (key == 13 || key == '\r' || key == '\n') {
			if (currentScreen == SCREEN_MENU) {
				if (nameInputLen == 0) {
					strcpy_s(nameInputBuffer, sizeof(nameInputBuffer), "Hero");
					nameInputLen = (int)strlen(nameInputBuffer);
				}
				gSaveSystem.addNewPlayer(nameInputBuffer);
				isNamingPlayer = false;
				currentScreen = SCREEN_STORY;
				storyIndex = 1;
				return;
			}
			else if (currentScreen == SCREEN_OPTIONS) {
				if (nameInputLen > 0) {
					if (namingTargetSlot == -1) {
						gSaveSystem.addNewPlayer(nameInputBuffer);
						optionsPlayerPage = gSaveSystem.getActivePlayerIndex() / 3;
					}
					else if (namingTargetSlot >= 0 && namingTargetSlot < gSaveSystem.getNumPlayers()) {
						gSaveSystem.setPlayerName(namingTargetSlot, nameInputBuffer);
					}
					isNamingPlayer = false;
				}
				return;
			}
		}
		if (key == 27) { // ESC cancels modal
			isNamingPlayer = false;
			return;
		}
		if (key == '\b' || key == 8) { // Backspace
			if (nameInputLen > 0) {
				nameInputLen--;
				nameInputBuffer[nameInputLen] = '\0';
			}
			return;
		}
		if (key >= 32 && key <= 126) { // Any printable ASCII characters
			if (nameInputLen < 24) {
				nameInputBuffer[nameInputLen++] = (char)key;
				nameInputBuffer[nameInputLen] = '\0';
			}
			return;
		}
		return;
	}

	if (key == 27) {
		if (currentScreen == SCREEN_OPTIONS || currentScreen == SCREEN_CREDITS) {
			currentScreen = SCREEN_MENU;
			return;
		}
		exitGame();
	}

	if (key == 'f' || key == 'F') {
		iToggleFullScreen();
		return;
	}

	if (key == 'h' || key == 'H') {
		currentScreen = SCREEN_MENU;
		isNamingPlayer = false;
		showWinCard = false;
		levelDone = false;
		isEntering = false;
		isExiting = false;
		isPaused = false;
		optionsPlayerPage = 0;
		gAudio.playMenuBGM();
		return;
	}

	if (currentScreen == SCREEN_SPLASH) {
		if (key == 13 || key == '\r') {
			currentScreen = SCREEN_MENU;
			gAudio.playMenuBGM();
		}
		return;
	}

	if (currentScreen == SCREEN_MENU) {
		if (key == 'o' || key == 'O') {
			currentScreen = SCREEN_OPTIONS;
			return;
		}
		if (key == 'c' || key == 'C') {
			currentScreen = SCREEN_CREDITS;
			return;
		}
		if (key == 13 || key == '\r' || key == ' ') {
			isNamingPlayer = true;
			namingTargetSlot = -1;
			nameInputBuffer[0] = '\0';
			nameInputLen = 0;
			return;
		}
		return;
	}

	if (currentScreen == SCREEN_OPTIONS) {
		if (key == 'r' || key == 'R' || key == 'e' || key == 'E') {
			isNamingPlayer = true;
			namingTargetSlot = gSaveSystem.getActivePlayerIndex();
			strcpy_s(nameInputBuffer, sizeof(nameInputBuffer), gSaveSystem.getActivePlayer().name);
			nameInputLen = strlen(nameInputBuffer);
			return;
		}
		if (key == 'm' || key == 'M') {
			gAudio.toggleMusic();
			gSaveSystem.setMusicEnabled(gAudio.isMusicEnabled());
			return;
		}
		if (key == 's' || key == 'S') {
			gAudio.toggleSound();
			gSaveSystem.setSoundEnabled(gAudio.isSoundEnabled());
			return;
		}
		if (key == 'n' || key == 'N' || key == '+' || key == '=') {
			if (gSaveSystem.getNumPlayers() < MAX_SAVED_PLAYERS) {
				isNamingPlayer = true;
				namingTargetSlot = -1; // New player mode
				nameInputBuffer[0] = '\0';
				nameInputLen = 0;
			}
			return;
		}
		if (key == '[' || key == '<' || key == ',') {
			if (optionsPlayerPage > 0) optionsPlayerPage--;
			return;
		}
		if (key == ']' || key == '>' || key == '.') {
			if ((optionsPlayerPage + 1) * 3 < gSaveSystem.getNumPlayers()) {
				optionsPlayerPage++;
			}
			return;
		}
		int startIdx = optionsPlayerPage * 3;
		if (key == '1') {
			if (startIdx < gSaveSystem.getNumPlayers()) gSaveSystem.setActivePlayerIndex(startIdx);
			return;
		}
		if (key == '2') {
			if (startIdx + 1 < gSaveSystem.getNumPlayers()) gSaveSystem.setActivePlayerIndex(startIdx + 1);
			return;
		}
		if (key == '3') {
			if (startIdx + 2 < gSaveSystem.getNumPlayers()) gSaveSystem.setActivePlayerIndex(startIdx + 2);
			return;
		}
		if (key == 'b' || key == 'B' || key == ' ' || key == 27) {
			currentScreen = SCREEN_MENU;
			return;
		}
		return;
	}

	if (currentScreen == SCREEN_STORY) {
		if (key == 's' || key == 'S') {
			skipStory();
			return;
		}
		if (key == 'n' || key == 'N' || key == 13 || key == '\r' || key == ' ') {
			advanceStory();
			return;
		}
		return;
	}

	if (currentScreen == SCREEN_END_CARD) {
		if (key == 'h' || key == 'H' || key == 13 || key == '\r' || key == ' ') {
			currentScreen = SCREEN_MENU;
			isNamingPlayer = false;
			showWinCard = false;
			levelDone = false;
			isEntering = false;
			isExiting = false;
			isPaused = false;
			optionsPlayerPage = 0;
			gAudio.playMenuBGM();
		}
		return;
	}

	if (currentScreen == SCREEN_CREDITS) {
		if (key == 'b' || key == 'B' || key == 13 || key == '\r' || key == ' ' || key == 27) {
			currentScreen = SCREEN_MENU;
		}
		return;
	}

	if (key == 'r' || key == 'R') {
		restartGame();
		return;
	}

	if (showWinCard) {
		if (key == 'n' || key == 'N' || key == 13 || key == '\r' || key == ' ') {
			if (currentLevel == 1) {
				startLevel(2);
			}
			else if (currentLevel == 2) {
				startLevel(3);
			}
			else if (currentLevel == 3) {
				startLevel(4);
			}
			else if (currentLevel == 4) {
				currentScreen = SCREEN_END_CARD;
			}
			return;
		}
	}

	if (key == ' ') {
		isPaused = !isPaused;
		return;
	}

	if (isPaused) return;

	if (key == 'n' || key == 'N') {
		if (currentLevel == 3 && nCooldown >= 1.0) {
			castNPower();
		}
	}
	else if (key == 'd' || key == 'D') {
		moveRight();
	}
	else if (key == 'm' || key == 'M') {
		if (currentLevel == 3) {
			castMPower();
		}
	}
	else if (key == 'a' || key == 'A') moveLeft();
	else if (key == 'w' || key == 'W') jump();
	else if (key == 's' || key == 'S') sit();
}

void iSpecialKeyboard(unsigned char key)
{
	if (key == GLUT_KEY_F11) {
		iToggleFullScreen();
		return;
	}

	if (currentScreen == SCREEN_STORY) {
		if (key == GLUT_KEY_RIGHT) {
			advanceStory();
			return;
		}
	}

	if (currentScreen != SCREEN_GAME) return;

	if (showWinCard) {
		if (key == GLUT_KEY_RIGHT) {
			if (currentLevel == 1) {
				startLevel(2);
			}
			else if (currentLevel == 2) {
				startLevel(3);
			}
			else if (currentLevel == 3) {
				startLevel(4);
			}
			else if (currentLevel == 4) {
				currentScreen = SCREEN_END_CARD;
			}
			return;
		}
	}

	if (isPaused) return;

	if (key == GLUT_KEY_RIGHT) moveRight();
	else if (key == GLUT_KEY_LEFT) moveLeft();
	else if (key == GLUT_KEY_UP) jump();
	else if (key == GLUT_KEY_DOWN) sit();
}

void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}

void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		if (currentScreen == SCREEN_SPLASH) {
			currentScreen = SCREEN_MENU;
			gAudio.playMenuBGM();
			return;
		}

		if (currentScreen == SCREEN_MENU) {
			if (isNamingPlayer) {
				// Confirm Button in modal -> Start Game
				if (mx >= BTN_MODAL_CONFIRM_X1 && mx <= BTN_MODAL_CONFIRM_X2 &&
					my >= BTN_MODAL_CONFIRM_Y1 && my <= BTN_MODAL_CONFIRM_Y2) {
					if (nameInputLen == 0) {
						strcpy_s(nameInputBuffer, sizeof(nameInputBuffer), "Hero");
						nameInputLen = (int)strlen(nameInputBuffer);
					}
					gSaveSystem.addNewPlayer(nameInputBuffer);
					isNamingPlayer = false;
					currentScreen = SCREEN_STORY;
					storyIndex = 1;
					return;
				}

				// Cancel Button in modal
				if (mx >= BTN_MODAL_CANCEL_X1 && mx <= BTN_MODAL_CANCEL_X2 &&
					my >= BTN_MODAL_CANCEL_Y1 && my <= BTN_MODAL_CANCEL_Y2) {
					isNamingPlayer = false;
					return;
				}

				// If clicked inside modal window, consume click
				if (mx >= 300 && mx <= 980 && my >= 140 && my <= 560) {
					return;
				}

				// Click outside modal cancels
				isNamingPlayer = false;
				return;
			}

			// Start Game Button -> Opens Name Input Modal
			if (mx >= BTN_START_X1 && mx <= BTN_START_X2 &&
				my >= BTN_START_Y1 && my <= BTN_START_Y2) {
				isNamingPlayer = true;
				namingTargetSlot = -1;
				nameInputBuffer[0] = '\0';
				nameInputLen = 0;
				return;
			}

			// Options Button
			if (mx >= BTN_OPTIONS_X1 && mx <= BTN_OPTIONS_X2 &&
				my >= BTN_OPTIONS_Y1 && my <= BTN_OPTIONS_Y2) {
				currentScreen = SCREEN_OPTIONS;
				return;
			}

			// Credits Button
			if (mx >= BTN_CREDITS_X1 && mx <= BTN_CREDITS_X2 &&
				my >= BTN_CREDITS_Y1 && my <= BTN_CREDITS_Y2) {
				currentScreen = SCREEN_CREDITS;
				return;
			}

			// Exit Button
			if (mx >= BTN_EXIT_X1 && mx <= BTN_EXIT_X2 &&
				my >= BTN_EXIT_Y1 && my <= BTN_EXIT_Y2) {
				exitGame();
			}
			return;
		}

		if (currentScreen == SCREEN_OPTIONS) {
			if (isNamingPlayer) {
				// Confirm Button in modal
				if (mx >= BTN_MODAL_CONFIRM_X1 && mx <= BTN_MODAL_CONFIRM_X2 &&
					my >= BTN_MODAL_CONFIRM_Y1 && my <= BTN_MODAL_CONFIRM_Y2) {
					if (nameInputLen > 0) {
						if (namingTargetSlot == -1) {
							gSaveSystem.addNewPlayer(nameInputBuffer);
							optionsPlayerPage = gSaveSystem.getActivePlayerIndex() / 3;
						}
						else if (namingTargetSlot >= 0 && namingTargetSlot < gSaveSystem.getNumPlayers()) {
							gSaveSystem.setPlayerName(namingTargetSlot, nameInputBuffer);
						}
						isNamingPlayer = false;
					}
					return;
				}

				// Cancel Button in modal
				if (mx >= BTN_MODAL_CANCEL_X1 && mx <= BTN_MODAL_CANCEL_X2 &&
					my >= BTN_MODAL_CANCEL_Y1 && my <= BTN_MODAL_CANCEL_Y2) {
					isNamingPlayer = false;
					return;
				}

				// If clicked inside modal window, consume click
				if (mx >= 300 && mx <= 980 && my >= 140 && my <= 560) {
					return;
				}

				// Click outside modal cancels
				isNamingPlayer = false;
				return;
			}

			// Rename Active Player Button
			if (mx >= BTN_OPT_RENAME_X1 && mx <= BTN_OPT_RENAME_X2 &&
				my >= BTN_OPT_RENAME_Y1 && my <= BTN_OPT_RENAME_Y2) {
				isNamingPlayer = true;
				namingTargetSlot = gSaveSystem.getActivePlayerIndex();
				strcpy_s(nameInputBuffer, sizeof(nameInputBuffer), gSaveSystem.getActivePlayer().name);
				nameInputLen = strlen(nameInputBuffer);
				return;
			}

			// Music Toggle
			if (mx >= BTN_OPT_MUSIC_X1 && mx <= BTN_OPT_MUSIC_X2 &&
				my >= BTN_OPT_MUSIC_Y1 && my <= BTN_OPT_MUSIC_Y2) {
				gAudio.toggleMusic();
				gSaveSystem.setMusicEnabled(gAudio.isMusicEnabled());
				return;
			}

			// Sound Toggle
			if (mx >= BTN_OPT_SOUND_X1 && mx <= BTN_OPT_SOUND_X2 &&
				my >= BTN_OPT_SOUND_Y1 && my <= BTN_OPT_SOUND_Y2) {
				gAudio.toggleSound();
				gSaveSystem.setSoundEnabled(gAudio.isSoundEnabled());
				return;
			}

			// Prev Page Button
			if (mx >= BTN_OPT_PREV_X1 && mx <= BTN_OPT_PREV_X2 &&
				my >= BTN_OPT_PREV_Y1 && my <= BTN_OPT_PREV_Y2) {
				if (optionsPlayerPage > 0) optionsPlayerPage--;
				return;
			}

			// Next Page Button
			if (mx >= BTN_OPT_NEXT_X1 && mx <= BTN_OPT_NEXT_X2 &&
				my >= BTN_OPT_NEXT_Y1 && my <= BTN_OPT_NEXT_Y2) {
				if ((optionsPlayerPage + 1) * 3 < gSaveSystem.getNumPlayers()) {
					optionsPlayerPage++;
				}
				return;
			}

			// Add New Player Button
			if (mx >= BTN_OPT_ADD_X1 && mx <= BTN_OPT_ADD_X2 &&
				my >= BTN_OPT_ADD_Y1 && my <= BTN_OPT_ADD_Y2) {
				if (gSaveSystem.getNumPlayers() < MAX_SAVED_PLAYERS) {
					isNamingPlayer = true;
					namingTargetSlot = -1; // New player mode
					nameInputBuffer[0] = '\0';
					nameInputLen = 0;
				}
				return;
			}

			int startIdx = optionsPlayerPage * 3;

			// Slot 1
			if (mx >= BTN_OPT_P1_X1 && mx <= BTN_OPT_P1_X2 &&
				my >= BTN_OPT_P1_Y1 && my <= BTN_OPT_P1_Y2) {
				if (startIdx < gSaveSystem.getNumPlayers()) {
					gSaveSystem.setActivePlayerIndex(startIdx);
				}
				else if (startIdx == gSaveSystem.getNumPlayers() && gSaveSystem.getNumPlayers() < MAX_SAVED_PLAYERS) {
					isNamingPlayer = true;
					namingTargetSlot = -1;
					nameInputBuffer[0] = '\0';
					nameInputLen = 0;
				}
				return;
			}

			// Slot 2
			if (mx >= BTN_OPT_P2_X1 && mx <= BTN_OPT_P2_X2 &&
				my >= BTN_OPT_P2_Y1 && my <= BTN_OPT_P2_Y2) {
				if (startIdx + 1 < gSaveSystem.getNumPlayers()) {
					gSaveSystem.setActivePlayerIndex(startIdx + 1);
				}
				else if (startIdx + 1 == gSaveSystem.getNumPlayers() && gSaveSystem.getNumPlayers() < MAX_SAVED_PLAYERS) {
					isNamingPlayer = true;
					namingTargetSlot = -1;
					nameInputBuffer[0] = '\0';
					nameInputLen = 0;
				}
				return;
			}

			// Slot 3
			if (mx >= BTN_OPT_P3_X1 && mx <= BTN_OPT_P3_X2 &&
				my >= BTN_OPT_P3_Y1 && my <= BTN_OPT_P3_Y2) {
				if (startIdx + 2 < gSaveSystem.getNumPlayers()) {
					gSaveSystem.setActivePlayerIndex(startIdx + 2);
				}
				else if (startIdx + 2 == gSaveSystem.getNumPlayers() && gSaveSystem.getNumPlayers() < MAX_SAVED_PLAYERS) {
					isNamingPlayer = true;
					namingTargetSlot = -1;
					nameInputBuffer[0] = '\0';
					nameInputLen = 0;
				}
				return;
			}

			// Back to Menu
			if (mx >= BTN_OPT_BACK_X1 && mx <= BTN_OPT_BACK_X2 &&
				my >= BTN_OPT_BACK_Y1 && my <= BTN_OPT_BACK_Y2) {
				currentScreen = SCREEN_MENU;
				return;
			}
			return;
		}

		if (currentScreen == SCREEN_CREDITS) {
			if (mx >= BTN_BACK_X1 && mx <= BTN_BACK_X2 &&
				my >= BTN_BACK_Y1 && my <= BTN_BACK_Y2) {
				currentScreen = SCREEN_MENU;
			}
			return;
		}

		if (currentScreen == SCREEN_STORY) {
			if (storyIndex >= 1 && storyIndex <= 3) {
				// Skip Button (Left)
				if (mx >= BTN_STORY_SKIP_X1 && mx <= BTN_STORY_SKIP_X2 &&
					my >= BTN_STORY_SKIP_Y1 && my <= BTN_STORY_SKIP_Y2) {
					skipStory();
					return;
				}
				// Next Button (Right)
				if (mx >= BTN_STORY_NEXT_X1 && mx <= BTN_STORY_NEXT_X2 &&
					my >= BTN_STORY_NEXT_Y1 && my <= BTN_STORY_NEXT_Y2) {
					advanceStory();
					return;
				}
			}
			else if (storyIndex == 4) {
				// Next / Continue Button (Right)
				if (mx >= BTN_STORY_NEXT_X1 && mx <= BTN_STORY_NEXT_X2 &&
					my >= BTN_STORY_NEXT_Y1 && my <= BTN_STORY_NEXT_Y2) {
					advanceStory();
					return;
				}
			}
			return;
		}

		if (currentScreen == SCREEN_END_CARD) {
			// "Go to Home" Button
			if (mx >= BTN_END_HOME_X1 && mx <= BTN_END_HOME_X2 &&
				my >= BTN_END_HOME_Y1 && my <= BTN_END_HOME_Y2) {
				currentScreen = SCREEN_MENU;
				isNamingPlayer = false;
				showWinCard = false;
				levelDone = false;
				isEntering = false;
				isExiting = false;
				isPaused = false;
				optionsPlayerPage = 0;
				gAudio.playMenuBGM();
			}
			return;
		}

		if (currentScreen == SCREEN_GAME) {
			performHeroAttack();
			return;
		}
	}
}

/* ================================================================
   INITIALIZATION & MAIN
   ================================================================ */
void initGame()
{
	imgSplash = iLoadImage("Images/splash.png");
	imgMenu = iLoadImage("Images/Home.png");
	imgGameOver = iLoadImage("Images/Game over.png");

	imgStory[0] = iLoadImage("Images/Story Part 1.png");
	imgStory[1] = iLoadImage("Images/Story Part 2.png");
	imgStory[2] = iLoadImage("Images/Story Part 3.png");
	imgStory[3] = iLoadImage("Images/Story Part 4.png");

	imgYouWin[0] = iLoadImage("Images/you_win_frame1.png");
	imgYouWin[1] = iLoadImage("Images/you_win_frame2.png");
	imgYouWin[2] = iLoadImage("Images/you_win_frame3.png");

	bg[0] = iLoadImage("Images/B1.png");
	bg[1] = iLoadImage("Images/B2.png");
	bg[2] = iLoadImage("Images/B3.png");
	bg[3] = iLoadImage("Images/B4.png");
	bg[4] = iLoadImage("Images/B5.png");

	// Level 3 Backgrounds
	imgBL1 = iLoadImage("Images/BL1.png");
	imgBL2 = iLoadImage("Images/BL2.png");
	imgBL3 = iLoadImage("Images/BL3.png");

	printf("--- Loading Character and Enemy Animations ---\n");

	// Right-run: 9 frames (Run1.png to Run9.png)
	for (int i = 0; i < 9; i++) {
		char path[128];
		sprintf_s(path, "Images/Run%d.png", i + 1);
		imgRunRight[i] = iLoadImage(path);
	}

	// Left-run: 9 frames (Runl1.png to Runl9.png)
	for (int i = 0; i < 9; i++) {
		char path[128];
		sprintf_s(path, "Images/Runl%d.png", i + 1);
		imgRunLeft[i] = iLoadImage(path);
	}

	// Fight-right: 5 frames (fr1.png to fr5.png)
	for (int i = 0; i < 5; i++) {
		char path[128];
		sprintf_s(path, "Images/fr%d.png", i + 1);
		imgFightRight[i] = iLoadImage(path);
	}

	// Fight-left: 5 frames (fl1.png to fl5.png)
	for (int i = 0; i < 5; i++) {
		char path[128];
		sprintf_s(path, "Images/fl%d.png", i + 1);
		imgFightLeft[i] = iLoadImage(path);
	}

	// Jump Animation: 8 frames (j1.png to j8.png)
	for (int i = 0; i < 8; i++) {
		char path[128];
		sprintf_s(path, "Images/j%d.png", i + 1);
		imgJump[i] = iLoadImage(path);
	}

	// Sitting/Crouch: 3 frames (sit1.png to sit3.png)
	for (int i = 0; i < 3; i++) {
		char path[128];
		sprintf_s(path, "Images/sit%d.png", i + 1);
		imgSit[i] = iLoadImage(path);
	}

	// Fireballs: 5 frames (Fire Ball1.png to Fire Ball5.png)
	for (int i = 0; i < 5; i++) {
		char path[128];
		sprintf_s(path, "Images/Fire Ball%d.png", i + 1);
		imgBall[i] = iLoadImage(path);
	}

	// Bats: 3 frames (bat1.png to bat3.png)
	for (int i = 0; i < 3; i++) {
		char path[128];
		sprintf_s(path, "Images/bat%d.png", i + 1);
		imgBat[i] = iLoadImage(path);
	}

	// Idle: 1 frame right (idle.png), 1 frame left (idlel.png)
	imgIdle = iLoadImage("Images/idle.png");
	imgIdleLeft = iLoadImage("Images/idlel.png");

	// Ghost frames: g1, g2 (facing left), gl1, gl2 (facing right)
	imgGhostLeft[0] = iLoadImage("Images/g1.png");
	imgGhostLeft[1] = iLoadImage("Images/g2.png");
	imgGhostRight[0] = iLoadImage("Images/gl1.png");
	imgGhostRight[1] = iLoadImage("Images/gl2.png");

	// Skeleton Running Right: 5 frames (sRun1.png to sRun5.png)
	for (int i = 0; i < 5; i++) {
		char path[128];
		sprintf_s(path, "Images/sRun%d.png", i + 1);
		imgSkeletonRunRight[i] = iLoadImage(path);
	}

	// Skeleton Running Left: 5 frames (sRunl1.png to sRunl5.png)
	for (int i = 0; i < 5; i++) {
		char path[128];
		sprintf_s(path, "Images/sRunl%d.png", i + 1);
		imgSkeletonRunLeft[i] = iLoadImage(path);
	}

	// Skeleton Fighting Right: 5 frames (sFight1.png to sFight5.png)
	for (int i = 0; i < 5; i++) {
		char path[128];
		sprintf_s(path, "Images/sFight%d.png", i + 1);
		imgSkeletonFightRight[i] = iLoadImage(path);
	}

	// Skeleton Fighting Left: 5 frames (sFightl1.png to sFightl5.png)
	for (int i = 0; i < 5; i++) {
		char path[128];
		sprintf_s(path, "Images/sFightl%d.png", i + 1);
		imgSkeletonFightLeft[i] = iLoadImage(path);
	}

	// Saint Walking & Fighting
	imgSaintWalk[0] = iLoadImage("Images/saint walk 1.png");
	imgSaintWalk[1] = iLoadImage("Images/saint walk 2.png");
	imgSaintWalk[2] = iLoadImage("Images/saint walk 3.png");
	imgSaintWalk[3] = iLoadImage("Images/saint walk 4.png");

	imgSaintFight[0] = iLoadImage("Images/Saint fight 1.png");
	imgSaintFight[1] = iLoadImage("Images/Saint fight 2.png");
	imgSaintFight[2] = iLoadImage("Images/Saint fight 3.png");
	imgSaintFight[3] = iLoadImage("Images/Saint fight 4.png");

	// Flash Projectile
	imgFlash = iLoadImage("Images/Flash.png");

	// Level 3 Soldier Animations (Walk: SoilderW1..5, Fight: f1..6)
	for (int i = 0; i < 5; i++) {
		char path[128];
		sprintf_s(path, "Images/SoilderW%d.png", i + 1);
		imgSoldierWalk[i] = iLoadImage(path);
	}
	for (int i = 0; i < 6; i++) {
		char path[128];
		sprintf_s(path, "Images/f%d.png", i + 1);
		imgSoldierFight[i] = iLoadImage(path);
	}

	// Level 3 Archer Animations (Walk: walk frame 1..8, Fight/Shoot: fight frame 1..5)
	const char* archerWalkPaths[8] = {
		"Images/walk frame 01 without background.png",
		"Images/walk frame 02 without background.png",
		"Images/walk frame 3 without background.png",
		"Images/walk frame 4 without background.png",
		"Images/walk frame 5 without background.png",
		"Images/walk frame 6 without background.png",
		"Images/walk frame 07 without background.png",
		"Images/walk frame 8 without background.png"
	};
	for (int i = 0; i < 8; i++) {
		imgArcherWalk[i] = iLoadImage((char*)archerWalkPaths[i]);
	}

	const char* archerFightPaths[5] = {
		"Images/fight frame 1 without background.png",
		"Images/fight frame 2 without background.png",
		"Images/fight frame 3 without background.png",
		"Images/fight frame 4 without background.png",
		"Images/fight frame 5 without background.png"
	};
	for (int i = 0; i < 5; i++) {
		imgArcherFight[i] = iLoadImage((char*)archerFightPaths[i]);
	}

	// Level 3 Arrow Projectile
	imgArrow = iLoadImage("Images/Arrow.png");

	// Level 4 Boss Mesh Animations (Walk: mv1..5, Fight: mvf1..5)
	for (int i = 0; i < 5; i++) {
		char path[128];
		sprintf_s(path, "Images/mv%d.png", i + 1);
		imgMeshWalk[i] = iLoadImage(path);
	}
	for (int i = 0; i < 5; i++) {
		char path[128];
		sprintf_s(path, "Images/mvf%d.png", i + 1);
		imgMeshFight[i] = iLoadImage(path);
	}

	// Health Cards & Progress Bar Assets (Clean standard RGBA loading - no chroma corruption)
	imgHealthTim = iLoadImage("Images/Tim health.png");
	imgHealthMesh = iLoadImage("Images/Mesh health.png");
	imgHealthSaint = iLoadImage("Images/Saint health.png");
	imgHealthArcher = iLoadImage("Images/Archer health.png");
	imgHealthSoldier = iLoadImage("Images/Soilder health.png");
	imgProgressBar = iLoadImage("Images/Progress Bar.png");

	initEnemies();
	initObstacles();
	initBoss();
	initMesh();
	initAllies();
	initArrows();

	gSaveSystem.init();
	gAudio.init();
	gAudio.setMusicEnabled(gSaveSystem.isMusicEnabled());
	gAudio.setSoundEnabled(gSaveSystem.isSoundEnabled());

	printf("All assets and binary save data loaded successfully!\n");
}

int main()
{
	DWORD dwAttrib = GetFileAttributesA("Images");
	if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
		if (GetFileAttributesA("maingame\\Images") != INVALID_FILE_ATTRIBUTES) {
			SetCurrentDirectoryA("maingame");
		}
		else if (GetFileAttributesA("..\\maingame\\Images") != INVALID_FILE_ATTRIBUTES) {
			SetCurrentDirectoryA("..\\maingame");
		}
	}

	iInitialize(SCREEN_W, SCREEN_H, "Running Game - AUST CSE-1200");
	initGame();
	iSetTimer(16, updatePhysics);

	iStart();
	return 0;
}
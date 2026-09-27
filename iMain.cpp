#include "GameCommon.h"
#include "Player.h"
#include "Enemy.h"
#include "MenuSystem.h"
#include "AudioManager.h"
#include "SaveSystem.h"
#include "EnemyAI.h"
#include "Rendering.h"


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
int        selectedLevelToStart = 0;
bool       heroAttackSoundPlayed = false;
bool       heroAttackDamageApplied = false;

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
int nextMeshTeleportThreshold = 350;
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

// Level 1 Section Spawning (Distributed across 7 background sections)
int lvl1SectionGhostsSpawned[7] = { 0 };
int lvl1SectionSkeletonsSpawned[7] = { 0 };
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
bool meshHasTeleported = false;
int lvl4SoldierSpawnCount = 0;
int lvl4ArcherSpawnCount = 0;
int lastLvl4MinionSpawnTimer = 0;
int imgMeshWalk[5];
int imgMeshFight[5];

// Level 3 Counters & Variables (BL1: 35 Soldiers + 15 Archers, BL2: 15 Soldiers + 35 Archers)
int bl1SoldierSpawnCount = 0;
int bl1ArcherSpawnCount = 0;
int bl1KillCount = 0;
int bl2SoldierSpawnCount = 0;
int bl2ArcherSpawnCount = 0;
int bl2KillCount = 0;
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

bool isAttackButtonHeld = false;
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
GAME LOGIC & MOVEMENT
================================================================ */
void startLevel(int level)
{
	currentLevel = level;
	gAudio.stopBatSound();
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
	heroAttackSoundPlayed = false;
	heroAttackDamageApplied = false;
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
	for (int s = 0; s < 7; s++) {
		lvl1SectionGhostsSpawned[s] = 0;
		lvl1SectionSkeletonsSpawned[s] = 0;
	}
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
	bl1SoldierSpawnCount = 0;
	bl1ArcherSpawnCount = 0;
	bl1KillCount = 0;
	bl2SoldierSpawnCount = 0;
	bl2ArcherSpawnCount = 0;
	bl2KillCount = 0;
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
	nextMeshTeleportThreshold = 350;
	meshHasTeleported = false;
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
		if (dist > 0 && dist < 210.0) {
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
			if (currentLevel == 4 && bossMesh.active && bossMesh.alive && bossMesh.x > charX) {
				if (charX > bossMesh.x - 210.0) {
					charX = bossMesh.x - 210.0;
				}
			}
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
		if (dist < 0 && dist > -220.0) {
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
			if (currentLevel == 4 && bossMesh.active && bossMesh.alive && bossMesh.x < charX) {
				if (charX < bossMesh.x + 220.0) {
					charX = bossMesh.x + 220.0;
				}
			}
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

	/* Check Boss Mesh hit (Level 4 - King / Main Villain - Damage: 40 HP per strike) */
	if (bossMesh.active && bossMesh.alive) {
		double distMesh = bossMesh.x - charX;
		bool hitMesh = false;
		if (facingRight && distMesh > 0 && distMesh < 320.0) {
			hitMesh = true;
		}
		else if (!facingRight && distMesh < 0 && distMesh > -320.0) {
			hitMesh = true;
		}

		if (hitMesh) {
			otherEnemyHit = true;
			bossMesh.health -= 40; // Exactly 40 HP damage per strike on King Mesh (Level 4)
			if (bossMesh.health < 0) bossMesh.health = 0;

			// Teleport whenever HP reaches or drops to/below the next 50 HP threshold (350, 300, 250, 200, 150, 100, 50)
			if (bossMesh.health <= nextMeshTeleportThreshold && bossMesh.health > 0 && nextMeshTeleportThreshold >= 50) {
				while (nextMeshTeleportThreshold >= 50 && bossMesh.health <= nextMeshTeleportThreshold) {
					nextMeshTeleportThreshold -= 50;
				}
				bossMesh.vanishEffectTimer = 18;

				if (bossMesh.fromRight) {
					// Teleport to the other side (behind hero / left)
					bossMesh.x = charX - 220.0;
					if (bossMesh.x < 40.0) bossMesh.x = 40.0;
					bossMesh.fromRight = false;
				}
				else {
					// Teleport to the other side (in front of hero / right)
					bossMesh.x = charX + 210.0;
					if (bossMesh.x > SCREEN_W - 200.0) bossMesh.x = SCREEN_W - 200.0;
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
						if (currentLevel == 3) {
							if (lvl3Stage == 1) bl1KillCount++;
							else if (lvl3Stage == 2) bl2KillCount++;
						}
						// Health recovery: +20 HP per 3 soldier kills by Hero!
						if (heroSoldierKillCount % 3 == 0) {
							playerHealth += 20;
							if (playerHealth > 200) playerHealth = 200;
						}
					}
					else if (enemies[i].type == ENEMY_ARCHER) {
						archerKillCount++;
						if (currentLevel == 3) {
							if (lvl3Stage == 1) bl1KillCount++;
							else if (lvl3Stage == 2) bl2KillCount++;
						}
						// Health recovery: +20 HP per 3 archer kills by Hero!
						if (archerKillCount % 3 == 0) {
							playerHealth += 20;
							if (playerHealth > 200) playerHealth = 200;
						}
					}

					if (currentLevel == 3) {
						if (lvl3Stage == 1 && bl1KillCount >= 50 && !isExiting) {
							isExiting = true;
							for (int a = 0; a < MAX_ALLIES; a++) {
								allies[a].active = false;
								allies[a].alive = false;
							}
						}
						else if (lvl3Stage == 2 && bl2KillCount >= 50 && !isExiting && !levelDone) {
							isExiting = true;
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
			}
		}
	}

	// Play combat audio independently: Ghost sound plays if a Ghost 
	// was hit in this swing, Hero/Sword sound plays if any other 
	// enemy (Skeleton/Soldier/Archer/Saint/Mesh) was hit in this 
	// swing. If both happen in the SAME swing, BOTH sounds must play 
	// together — hence two separate `if` blocks, NOT if/else.
	if (ghostFightHit) {
		gAudio.playGhostSwordFightSound(globalGameTimer);
	}
	if (otherEnemyHit) {
		gAudio.playHeroSwordFightSound(globalGameTimer);
	}
}

bool isEnemyInAttackRange()
{
	if (bossSaint.active && bossSaint.alive) {
		double dist = bossSaint.x - charX;
		if (facingRight && dist > 0 && dist < 280.0) return true;
		if (!facingRight && dist < 0 && dist > -280.0) return true;
	}
	if (bossMesh.active && bossMesh.alive) {
		double dist = bossMesh.x - charX;
		if (facingRight && dist > 0 && dist < 320.0) return true;
		if (!facingRight && dist < 0 && dist > -320.0) return true;
	}
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active && enemies[i].alive) {
			double dist = enemies[i].x - charX;
			if (facingRight && dist > 0 && dist < 260.0) return true;
			if (!facingRight && dist < 0 && dist > -260.0) return true;
		}
	}
	return false;
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
	heroAttackSoundPlayed = false;
	heroAttackDamageApplied = false;

	applyHeroAttackDamage();
	heroAttackSoundPlayed = true;
	heroAttackDamageApplied = true;
}

/* ================================================================
PHYSICS & TICK UPDATE
================================================================ */
void updatePhysics()
{
	globalGameTimer++;

	bool isGameplayActive = (currentScreen == SCREEN_GAME && !isPaused && !showWinCard && playerHealth > 0);
	gAudio.updateBGM((int)currentScreen, isGameplayActive, globalGameTimer);

	// Check if any bats are currently active on screen in Level 2
	bool hasBatsOnScreen = false;
	if (isGameplayActive && currentLevel == 2 && !levelDone && !isEntering && !isExiting) {
		for (int i = 0; i < MAX_OBSTACLES; i++) {
			if (obstacles[i].active && obstacles[i].type == OBSTACLE_BAT) {
				if (obstacles[i].x >= -200.0 && obstacles[i].x <= SCREEN_W + 200.0) {
					hasBatsOnScreen = true;
					break;
				}
			}
		}
	}
	gAudio.updateBatSound(hasBatsOnScreen);

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
				nextMeshTeleportThreshold = 350;
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
					// Defeated 50 enemies (35 soldiers + 15 archers) in BL1 -> Transition to BL2!
					lvl3Stage = 2;
					isExiting = false;
					isEntering = true;
					charX = -120;
					targetStartX = (int)(SCREEN_W * 0.25); // 25% screen width = 320px
					currentState = RUN_RIGHT;
					facingRight = true;
					gSaveSystem.updateLevelProgress(3, 50);

					// Clear old enemies, bots & projectiles
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

					bl2SoldierSpawnCount = 0;
					bl2ArcherSpawnCount = 0;
					bl2KillCount = 0;
					lastSoldierSpawnTimer = globalGameTimer;
					lastArcherSpawnTimer = globalGameTimer;
					nCooldown = 1.0;
					mCooldown = 1.0;
				}
				else if (lvl3Stage == 2) {
					// Defeated 50 enemies (15 soldiers + 35 archers) in BL2 -> Show Level 3 Victory Win Card!
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
				bool shouldContinueChain = (attackQueued > 0) && (isAttackButtonHeld || isEnemyInAttackRange());

				if (shouldContinueChain) {
					// Chain queued attack immediately (plays next 3 images)
					attackQueued = 0;
					charFrame = 0;
					fightTimer = 0;
					heroAttackSoundPlayed = false;
					heroAttackDamageApplied = false;
					if (facingRight) currentState = FIGHT_RIGHT;
					else currentState = FIGHT_LEFT;
					applyHeroAttackDamage();
					heroAttackSoundPlayed = true;
					heroAttackDamageApplied = true;
				}
				else {
					currentState = IDLE;
					charFrame = 0;
					attackQueued = 0;
					heroAttackSoundPlayed = false;
					heroAttackDamageApplied = false;
					gAudio.stopCombatSounds();
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
				if (currentLevel == 4 && bossMesh.active && bossMesh.alive && bossMesh.x > charX) {
					if (charX > bossMesh.x - 210.0) {
						charX = bossMesh.x - 210.0;
					}
				}
			}
			else if (!facingRight && charX > 40) {
				charX -= jumpMoveSpeed;
				if (currentLevel == 4 && bossMesh.active && bossMesh.alive && bossMesh.x < charX) {
					if (charX < bossMesh.x + 220.0) {
						charX = bossMesh.x + 220.0;
					}
				}
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
	updateEnemySpawning();

	/* --- LEVEL 2 BOSS ARENA: Minions (3 Ghosts & 2 Skeletons enter from LEFT side) --- */
	updateBossArenaMinionSpawning();

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
	updateAllies();

	/* --- Update Active Standard & Level 3 Enemies --- */
	updateEnemyAI();

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
			double entranceStopX = SCREEN_W - 320.0;
			if (entranceStopX < charX + 210.0) entranceStopX = charX + 210.0;
			if (bossMesh.x <= entranceStopX) {
				bossMesh.x = entranceStopX;
				bossMesh.isEntering = false;
			}
		}
		else if (!levelDone) {
			// Determine facing direction relative to Hero
			bossMesh.fromRight = (bossMesh.x >= charX);

			if (bossMesh.fromRight) {
				// Mesh is on the RIGHT side of Hero: Maintain strict combat safety gap
				double targetX = charX + 210.0;
				if (targetX > SCREEN_W - 200.0) targetX = SCREEN_W - 200.0;

				double distToTarget = bossMesh.x - targetX;

				if (distToTarget > 4.5) {
					// Move left towards Hero, stopping strictly at targetX
					bossMesh.x -= 4.2;
					if (bossMesh.x < targetX) bossMesh.x = targetX;
					bossMesh.isFighting = false;
					bossMesh.walkTimer++;
					if (bossMesh.walkTimer > 4) {
						bossMesh.walkFrame = (bossMesh.walkFrame + 1) % 5;
						bossMesh.walkTimer = 0;
					}
				}
				else if (distToTarget < -4.5) {
					// If closer than minimum distance, step back right to targetX
					bossMesh.x += 4.2;
					if (bossMesh.x > targetX) bossMesh.x = targetX;
					bossMesh.isFighting = false;
					bossMesh.walkTimer++;
					if (bossMesh.walkTimer > 4) {
						bossMesh.walkFrame = (bossMesh.walkFrame + 1) % 5;
						bossMesh.walkTimer = 0;
					}
				}
				else {
					// At exact combat distance
					bossMesh.x = targetX;
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
			}
			else {
				// Mesh is on the LEFT side of Hero: Maintain strict combat safety gap
				double targetX = charX - 220.0;
				if (targetX < 40.0) targetX = 40.0;

				double distToTarget = targetX - bossMesh.x;

				if (distToTarget > 4.5) {
					// Move right towards Hero, stopping strictly at targetX
					bossMesh.x += 4.2;
					if (bossMesh.x > targetX) bossMesh.x = targetX;
					bossMesh.isFighting = false;
					bossMesh.walkTimer++;
					if (bossMesh.walkTimer > 4) {
						bossMesh.walkFrame = (bossMesh.walkFrame + 1) % 5;
						bossMesh.walkTimer = 0;
					}
				}
				else if (distToTarget < -4.5) {
					// If closer than minimum distance, step back left to targetX
					bossMesh.x -= 4.2;
					if (bossMesh.x < targetX) bossMesh.x = targetX;
					bossMesh.isFighting = false;
					bossMesh.walkTimer++;
					if (bossMesh.walkTimer > 4) {
						bossMesh.walkFrame = (bossMesh.walkFrame + 1) % 5;
						bossMesh.walkTimer = 0;
					}
				}
				else {
					// At exact combat distance
					bossMesh.x = targetX;
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
			}
		}
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
			if (nameInputLen == 0) {
				strcpy_s(nameInputBuffer, sizeof(nameInputBuffer), "Hero");
				nameInputLen = (int)strlen(nameInputBuffer);
			}
			if (currentScreen == SCREEN_MENU) {
				gSaveSystem.addNewPlayer(nameInputBuffer);
				isNamingPlayer = false;
				currentScreen = SCREEN_STORY;
				storyIndex = 1;
				selectedLevelToStart = 0;
				return;
			}
			else if (currentScreen == SCREEN_OPTIONS) {
				if (selectedLevelToStart > 0) {
					gSaveSystem.addNewPlayer(nameInputBuffer);
					isNamingPlayer = false;
					int lvl = selectedLevelToStart;
					selectedLevelToStart = 0;
					startLevel(lvl);
					return;
				}
				else {
					if (namingTargetSlot == -1) {
						gSaveSystem.addNewPlayer(nameInputBuffer);
						optionsPlayerPage = gSaveSystem.getActivePlayerIndex() / 3;
					}
					else if (namingTargetSlot >= 0 && namingTargetSlot < gSaveSystem.getNumPlayers()) {
						gSaveSystem.setPlayerName(namingTargetSlot, nameInputBuffer);
					}
					isNamingPlayer = false;
					return;
				}
			}
		}
		if (key == 27) { // ESC cancels modal
			isNamingPlayer = false;
			selectedLevelToStart = 0;
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
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		isAttackButtonHeld = false;
		return;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		isAttackButtonHeld = true;

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
					selectedLevelToStart = 0;
					currentScreen = SCREEN_STORY;
					storyIndex = 1;
					return;
				}

				// Cancel Button in modal
				if (mx >= BTN_MODAL_CANCEL_X1 && mx <= BTN_MODAL_CANCEL_X2 &&
					my >= BTN_MODAL_CANCEL_Y1 && my <= BTN_MODAL_CANCEL_Y2) {
					isNamingPlayer = false;
					selectedLevelToStart = 0;
					return;
				}

				// If clicked inside modal window, consume click
				if (mx >= 300 && mx <= 980 && my >= 140 && my <= 560) {
					return;
				}

				// Click outside modal cancels
				isNamingPlayer = false;
				selectedLevelToStart = 0;
				return;
			}

			// Start Game Button -> Opens Name Input Modal
			if (mx >= BTN_START_X1 && mx <= BTN_START_X2 &&
				my >= BTN_START_Y1 && my <= BTN_START_Y2) {
				selectedLevelToStart = 1;
				isNamingPlayer = true;
				namingTargetSlot = -2;
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
					if (nameInputLen == 0) {
						strcpy_s(nameInputBuffer, sizeof(nameInputBuffer), "Hero");
						nameInputLen = (int)strlen(nameInputBuffer);
					}
					if (selectedLevelToStart > 0) {
						gSaveSystem.addNewPlayer(nameInputBuffer);
						isNamingPlayer = false;
						int lvl = selectedLevelToStart;
						selectedLevelToStart = 0;
						startLevel(lvl);
						return;
					}
					else {
						if (namingTargetSlot == -1) {
							gSaveSystem.addNewPlayer(nameInputBuffer);
							optionsPlayerPage = gSaveSystem.getActivePlayerIndex() / 3;
						}
						else if (namingTargetSlot >= 0 && namingTargetSlot < gSaveSystem.getNumPlayers()) {
							gSaveSystem.setPlayerName(namingTargetSlot, nameInputBuffer);
						}
						isNamingPlayer = false;
						return;
					}
				}

				// Cancel Button in modal
				if (mx >= BTN_MODAL_CANCEL_X1 && mx <= BTN_MODAL_CANCEL_X2 &&
					my >= BTN_MODAL_CANCEL_Y1 && my <= BTN_MODAL_CANCEL_Y2) {
					isNamingPlayer = false;
					selectedLevelToStart = 0;
					return;
				}

				// If clicked inside modal window, consume click
				if (mx >= 300 && mx <= 980 && my >= 140 && my <= 560) {
					return;
				}

				// Click outside modal cancels
				isNamingPlayer = false;
				selectedLevelToStart = 0;
				return;
			}

			// Level 1 Select Button
			if (mx >= BTN_OPT_LVL1_X1 && mx <= BTN_OPT_LVL1_X2 &&
				my >= BTN_OPT_LVL1_Y1 && my <= BTN_OPT_LVL1_Y2) {
				selectedLevelToStart = 1;
				isNamingPlayer = true;
				namingTargetSlot = -2;
				nameInputBuffer[0] = '\0';
				nameInputLen = 0;
				return;
			}

			// Level 2 Select Button
			if (mx >= BTN_OPT_LVL2_X1 && mx <= BTN_OPT_LVL2_X2 &&
				my >= BTN_OPT_LVL2_Y1 && my <= BTN_OPT_LVL2_Y2) {
				selectedLevelToStart = 2;
				isNamingPlayer = true;
				namingTargetSlot = -2;
				nameInputBuffer[0] = '\0';
				nameInputLen = 0;
				return;
			}

			// Level 3 Select Button
			if (mx >= BTN_OPT_LVL3_X1 && mx <= BTN_OPT_LVL3_X2 &&
				my >= BTN_OPT_LVL3_Y1 && my <= BTN_OPT_LVL3_Y2) {
				selectedLevelToStart = 3;
				isNamingPlayer = true;
				namingTargetSlot = -2;
				nameInputBuffer[0] = '\0';
				nameInputLen = 0;
				return;
			}

			// Level 4 Select Button
			if (mx >= BTN_OPT_LVL4_X1 && mx <= BTN_OPT_LVL4_X2 &&
				my >= BTN_OPT_LVL4_Y1 && my <= BTN_OPT_LVL4_Y2) {
				selectedLevelToStart = 4;
				isNamingPlayer = true;
				namingTargetSlot = -2;
				nameInputBuffer[0] = '\0';
				nameInputLen = 0;
				return;
			}

			// Rename Active Player Button
			if (mx >= BTN_OPT_RENAME_X1 && mx <= BTN_OPT_RENAME_X2 &&
				my >= BTN_OPT_RENAME_Y1 && my <= BTN_OPT_RENAME_Y2) {
				selectedLevelToStart = 0;
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
					selectedLevelToStart = 0;
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
					selectedLevelToStart = 0;
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
					selectedLevelToStart = 0;
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
					selectedLevelToStart = 0;
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

	iInitialize(SCREEN_W, SCREEN_H, "Eclipse Crown");
	initGame();
	iSetTimer(16, updatePhysics);

	iStart();
	return 0;
}
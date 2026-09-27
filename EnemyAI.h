#ifndef ENEMY_AI_H
#define ENEMY_AI_H

#include "GameCommon.h"
#include "Enemy.h"
#include "Player.h"
#include "SaveSystem.h"
#include "AudioManager.h"

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

	// Manipulation Power সফলভাবে কাস্ট হচ্ছে — spell-cast সাউন্ড কিউ হিসেবে 
	// Ghost Fight Sound রিইউজ করা হচ্ছে। এই power যতবারই থ্রো করা হবে 
	// (প্রতিবার M চাপলেই, শুধু প্রথমবার না), ততবারই এই সাউন্ডটা বাজবে।
	gAudio.playGhostSwordFightSound(globalGameTimer);

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
					if (currentLevel == 3) {
						if (lvl3Stage == 1) bl1KillCount++;
						else if (lvl3Stage == 2) bl2KillCount++;
					}
					gSaveSystem.recordValidKill(2);
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
SPAWNING & LEVEL STAGE MECHANICS
================================================================ */
void updateEnemySpawning()
{
	if (!levelDone && !isEntering && !isBossArena) {
		if (currentLevel == 1) {
			/* --- LEVEL 1: PROGRESSIVE SECTION-BASED ARRIVAL ACROSS ALL BACKGROUNDS ---
			Total Enemies = exactly 39 (19 Ghosts + 20 Skeletons)
			Section 0 (B1): 2 Ghosts + 3 Skeletons = 5
			Section 1 (B2): 3 Ghosts + 2 Skeletons = 5
			Section 2 (B3): 2 Ghosts + 3 Skeletons = 5
			Section 3 (B2): 3 Ghosts + 3 Skeletons = 6
			Section 4 (B3): 3 Ghosts + 2 Skeletons = 5
			Section 5 (B2): 2 Ghosts + 3 Skeletons = 5
			Section 6 (B3): 4 Ghosts + 4 Skeletons = 8
			Total Ghosts = 19, Total Skeletons = 20, Total = 39 enemies
			*/
			static const int lvl1GhostsTarget[7] = { 2, 3, 2, 3, 3, 2, 4 };
			static const int lvl1SkeletonsTarget[7] = { 3, 2, 3, 3, 2, 3, 4 };

			int currentSection = (int)(bgOffset / TILE_W);
			if (currentSection < 0) currentSection = 0;
			if (currentSection > 6) currentSection = 6;

			int activeCount = 0;
			for (int i = 0; i < MAX_ENEMIES; i++) {
				if (enemies[i].active && enemies[i].alive) activeCount++;
			}

			if (activeCount < 6) {
				for (int s = 0; s <= currentSection; s++) {
					// Ghost Spawning for section s
					if (lvl1SectionGhostsSpawned[s] < lvl1GhostsTarget[s]) {
						if (globalGameTimer > 15 && (globalGameTimer - lastSpawnTimer > 38)) {
							for (int i = 0; i < MAX_ENEMIES; i++) {
								if (!enemies[i].active) {
									enemies[i].type = ENEMY_GHOST;
									enemies[i].active = true;
									enemies[i].alive = true;
									enemies[i].health = 100;
									enemies[i].maxHealth = 100;
									enemies[i].baseY = 260.0;
									enemies[i].y = enemies[i].baseY;
									bool fromRightSide = ((lvl1SectionGhostsSpawned[s] + s) % 4 != 0);
									enemies[i].fromRight = fromRightSide;
									if (fromRightSide) enemies[i].x = SCREEN_W + 30;
									else enemies[i].x = -80;
									enemies[i].frame = 0;
									enemies[i].timer = 0;
									enemies[i].floatTimer = (lvl1SectionGhostsSpawned[s] * 10);

									lvl1SectionGhostsSpawned[s]++;
									spawnCount++;
									lastSpawnTimer = globalGameTimer;
									gAudio.playGhostComingSound(globalGameTimer);
									break;
								}
							}
						}
					}

					// Skeleton Spawning for section s
					if (lvl1SectionSkeletonsSpawned[s] < lvl1SkeletonsTarget[s]) {
						if (globalGameTimer > 25 && (globalGameTimer - lastSkeletonSpawnTimer > 48)) {
							for (int i = 0; i < MAX_ENEMIES; i++) {
								if (!enemies[i].active) {
									enemies[i].type = ENEMY_SKELETON;
									enemies[i].active = true;
									enemies[i].alive = true;
									enemies[i].health = 150;
									enemies[i].maxHealth = 150;
									enemies[i].baseY = GROUND_Y;
									enemies[i].y = GROUND_Y;
									bool spawnFromRight = ((lvl1SectionSkeletonsSpawned[s] + s) % 3 != 0);
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

									lvl1SectionSkeletonsSpawned[s]++;
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
			/* --- LEVEL 3 STAGE 1: BL1 (Total 50 Enemies: 35 Soldiers + 15 Archers) --- */
			if (lvl3Stage == 1) {
				int activeCount = 0;
				for (int i = 0; i < MAX_ENEMIES; i++) {
					if (enemies[i].active && enemies[i].alive) {
						activeCount++;
					}
				}

				int totalBL1Spawned = bl1SoldierSpawnCount + bl1ArcherSpawnCount;
				if (totalBL1Spawned < 50 && activeCount < 10) {
					if (globalGameTimer - lastSoldierSpawnTimer > 65) {
						int squadIdx = totalBL1Spawned / 4;
						bool spawnFromRightSide = (squadIdx % 3 != 2); // 2 squads Right, 1 squad Left (1/3 Left, 2/3 Right)

						int toSpawn = 4;
						if (totalBL1Spawned + toSpawn > 50) {
							toSpawn = 50 - totalBL1Spawned;
						}

						int spawnedInGroup = 0;
						for (int i = 0; i < MAX_ENEMIES && spawnedInGroup < toSpawn; i++) {
							if (!enemies[i].active) {
								// BL1: 35 Soldiers + 15 Archers
								bool spawnSoldier = true;
								if (bl1SoldierSpawnCount >= 35) {
									spawnSoldier = false;
								}
								else if (bl1ArcherSpawnCount < 15) {
									if (spawnedInGroup == 2 || (bl1SoldierSpawnCount > bl1ArcherSpawnCount * 2 && (rand() % 3 == 0))) {
										spawnSoldier = false;
									}
								}

								if (spawnSoldier && bl1SoldierSpawnCount < 35) {
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

									bl1SoldierSpawnCount++;
									soldierSpawnCount++;
									spawnedInGroup++;
								}
								else if (bl1ArcherSpawnCount < 15) {
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

									bl1ArcherSpawnCount++;
									archerSpawnCount++;
									spawnedInGroup++;
								}
							}
						}
						lastSoldierSpawnTimer = globalGameTimer;
						lastArcherSpawnTimer = globalGameTimer;
						gAudio.playGhostComingSound(globalGameTimer);
					}
				}

				// Check BL1 Stage Completion: 50 enemies (35 Soldiers + 15 Archers) Defeated!
				if (bl1KillCount >= 50 && !isExiting) {
					isExiting = true;
					for (int a = 0; a < MAX_ALLIES; a++) {
						allies[a].active = false;
						allies[a].alive = false;
					}
				}
			}
			/* --- LEVEL 3 STAGE 2: BL2 (Total 50 Enemies: 15 Soldiers + 35 Archers) --- */
			else if (lvl3Stage == 2) {
				int activeCount = 0;
				for (int i = 0; i < MAX_ENEMIES; i++) {
					if (enemies[i].active && enemies[i].alive) {
						activeCount++;
					}
				}

				int totalBL2Spawned = bl2SoldierSpawnCount + bl2ArcherSpawnCount;
				if (totalBL2Spawned < 50 && activeCount < 8) {
					if (globalGameTimer - lastArcherSpawnTimer > 65) {
						int squadIdx = totalBL2Spawned / 4;
						bool spawnFromRightSide = (squadIdx % 3 != 2); // 2 squads Right, 1 squad Left (1/3 Left, 2/3 Right)

						int toSpawn = 4;
						if (totalBL2Spawned + toSpawn > 50) {
							toSpawn = 50 - totalBL2Spawned;
						}

						int spawnedInGroup = 0;
						for (int i = 0; i < MAX_ENEMIES && spawnedInGroup < toSpawn; i++) {
							if (!enemies[i].active) {
								// BL2: 15 Soldiers + 35 Archers
								bool spawnArcher = true;
								if (bl2ArcherSpawnCount >= 35) {
									spawnArcher = false;
								}
								else if (bl2SoldierSpawnCount < 15) {
									if (spawnedInGroup == 1 || (bl2ArcherSpawnCount > bl2SoldierSpawnCount * 2 && (rand() % 3 == 0))) {
										spawnArcher = false;
									}
								}

								if (spawnArcher && bl2ArcherSpawnCount < 35) {
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

									bl2ArcherSpawnCount++;
									archerSpawnCount++;
									spawnedInGroup++;
								}
								else if (bl2SoldierSpawnCount < 15) {
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

									bl2SoldierSpawnCount++;
									soldierSpawnCount++;
									spawnedInGroup++;
								}
							}
						}
						lastArcherSpawnTimer = globalGameTimer;
						lastSoldierSpawnTimer = globalGameTimer;
						gAudio.playGhostComingSound(globalGameTimer);
					}
				}

				// Check BL2 Level 3 Completion: 50 enemies (15 Soldiers + 35 Archers) Defeated!
				if (bl2KillCount >= 50 && !isExiting && !levelDone) {
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
}

/* --- LEVEL 2 BOSS ARENA: Minions (3 Ghosts & 2 Skeletons enter from LEFT side) --- */
void updateBossArenaMinionSpawning()
{
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
}

/* --- Update Friendly Summoned Allies (N Power Horde) --- */
void updateAllies()
{
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
								if (enemies[targetIdx].type == ENEMY_SOLDIER) {
									soldierKillCount++;
									if (currentLevel == 3) {
										if (lvl3Stage == 1) bl1KillCount++;
										else if (lvl3Stage == 2) bl2KillCount++;
									}
								}
								else if (enemies[targetIdx].type == ENEMY_ARCHER) {
									archerKillCount++;
									if (currentLevel == 3) {
										if (lvl3Stage == 1) bl1KillCount++;
										else if (lvl3Stage == 2) bl2KillCount++;
									}
								}
								killCount++;
								gSaveSystem.recordValidKill(enemies[targetIdx].type);

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
								if (enemies[targetIdx].type == ENEMY_SOLDIER) {
									soldierKillCount++;
									if (currentLevel == 3) {
										if (lvl3Stage == 1) bl1KillCount++;
										else if (lvl3Stage == 2) bl2KillCount++;
									}
								}
								else if (enemies[targetIdx].type == ENEMY_ARCHER) {
									archerKillCount++;
									if (currentLevel == 3) {
										if (lvl3Stage == 1) bl1KillCount++;
										else if (lvl3Stage == 2) bl2KillCount++;
									}
								}
								killCount++;
								gSaveSystem.recordValidKill(enemies[targetIdx].type);

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
}

/* --- Update Active Standard & Level 3 Enemies --- */
void updateEnemyAI()
{
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
}

#endif

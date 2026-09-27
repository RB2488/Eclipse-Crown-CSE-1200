#ifndef ENEMY_H
#define ENEMY_H

#include "GameCommon.h"

#define MAX_ENEMIES 100
#define MAX_ALLIES 60
#define MAX_ARROWS 50

enum EnemyType {
	ENEMY_GHOST,
	ENEMY_SKELETON,
	ENEMY_SOLDIER,
	ENEMY_ARCHER
};

struct Enemy {
	EnemyType type;
	double x;
	double y;
	double baseY;
	bool active;
	bool alive;
	bool fromRight;
	int frame;
	int timer;
	int floatTimer;
	int health;
	int maxHealth;

	/* Skeleton & Soldier combat fields */
	bool isFighting;
	int fightFrame;
	int fightTimer;
	int attackTimer;

	/* Archer shooting fields */
	int shootCooldown;
	int shootTimer;
};

/* ---------- Friendly Summoned Allies (D Power) ---------- */
enum AllyType {
	ALLY_GHOST,
	ALLY_SKELETON
};

struct Ally {
	AllyType type;
	double x;
	double y;
	double baseY;
	bool active;
	bool alive;
	int frame;
	int timer;
	int floatTimer;
	int health;
	int maxHealth;

	bool isFighting;
	int fightFrame;
	int fightTimer;
	int attackTimer;
};

/* ---------- Arrow Projectiles (Archer Attacks) ---------- */
struct ArrowProjectile {
	double x;
	double y;
	bool active;
	double speed;
	bool fromRight;
};

extern Enemy enemies[MAX_ENEMIES];
extern Ally  allies[MAX_ALLIES];
extern ArrowProjectile arrows[MAX_ARROWS];

extern int globalGameTimer;
extern int lastSpawnTimer;
extern int spawnCount;
extern int killCount;
extern int ghostKillCount;
extern int skeletonSpawnCount;
extern int skeletonRightSpawnCount;
extern int skeletonLeftSpawnCount;
extern int skeletonKillCount;
extern int lastSkeletonSpawnTimer;

/* Level 1 Section Spawning (Distributed across 7 background sections) */
extern int lvl1SectionGhostsSpawned[7];
extern int lvl1SectionSkeletonsSpawned[7];
extern int lvl1LastStageGhostCount;
extern int lvl1LastStageSkeletonCount;
extern int lastLvl1Stage2SpawnTimer;

/* Level 2 Boss Fight Minions (4 Ghosts, 2 Skeletons) */
extern int bossGhostSpawnCount;
extern int bossSkeletonSpawnCount;
extern int lastBossMinionSpawnTimer;

/* Level 3 Counters & Variables (BL1: 35 Soldiers + 15 Archers, BL2: 15 Soldiers + 35 Archers) */
extern int bl1SoldierSpawnCount;
extern int bl1ArcherSpawnCount;
extern int bl1KillCount;
extern int bl2SoldierSpawnCount;
extern int bl2ArcherSpawnCount;
extern int bl2KillCount;
extern int soldierSpawnCount;
extern int soldierKillCount;
extern int archerSpawnCount;
extern int archerKillCount;
extern int lastSoldierSpawnTimer;
extern int lastArcherSpawnTimer;
extern bool nSummonedOnce;

/* Enemy Image Handles */
extern int imgGhostLeft[2];
extern int imgGhostRight[2];
extern int imgSkeletonRunRight[5];
extern int imgSkeletonRunLeft[5];
extern int imgSkeletonFightRight[5];
extern int imgSkeletonFightLeft[5];

/* Level 3 Enemy & Weapon Image Handles */
extern int imgSoldierWalk[5];
extern int imgSoldierFight[6];
extern int imgArcherWalk[8];
extern int imgArcherFight[5];
extern int imgArrow;

void initEnemies() {
	for (int s = 0; s < 7; s++) {
		lvl1SectionGhostsSpawned[s] = 0;
		lvl1SectionSkeletonsSpawned[s] = 0;
	}
	lvl1LastStageGhostCount = 0;
	lvl1LastStageSkeletonCount = 0;
	lastLvl1Stage2SpawnTimer = 0;
	bl1SoldierSpawnCount = 0;
	bl1ArcherSpawnCount = 0;
	bl1KillCount = 0;
	bl2SoldierSpawnCount = 0;
	bl2ArcherSpawnCount = 0;
	bl2KillCount = 0;
	for (int i = 0; i < MAX_ENEMIES; i++) {
		enemies[i].type = ENEMY_GHOST;
		enemies[i].active = false;
		enemies[i].alive = true;
		enemies[i].baseY = 260.0; /* Chest height for taller hero */
		enemies[i].y = enemies[i].baseY;
		enemies[i].fromRight = true;
		enemies[i].frame = 0;
		enemies[i].timer = 0;
		enemies[i].floatTimer = i * 10;
		enemies[i].health = 100;
		enemies[i].maxHealth = 100;
		enemies[i].isFighting = false;
		enemies[i].fightFrame = 0;
		enemies[i].fightTimer = 0;
		enemies[i].attackTimer = 0;
		enemies[i].shootCooldown = 0;
		enemies[i].shootTimer = 0;
	}
}

void initAllies() {
	for (int i = 0; i < MAX_ALLIES; i++) {
		allies[i].type = ALLY_GHOST;
		allies[i].active = false;
		allies[i].alive = false;
		allies[i].x = -200;
		allies[i].baseY = 260.0;
		allies[i].y = allies[i].baseY;
		allies[i].frame = 0;
		allies[i].timer = 0;
		allies[i].floatTimer = i * 10;
		allies[i].health = 50; // Reduced Bot health
		allies[i].maxHealth = 50;
		allies[i].isFighting = false;
		allies[i].fightFrame = 0;
		allies[i].fightTimer = 0;
		allies[i].attackTimer = 0;
	}
}

void initArrows() {
	for (int i = 0; i < MAX_ARROWS; i++) {
		arrows[i].active = false;
		arrows[i].x = -300;
		arrows[i].y = GROUND_Y;
		arrows[i].speed = 12.0;
		arrows[i].fromRight = true;
	}
}

/* ---------- Obstacles (Balls & Bats) ---------- */
enum ObstacleType {
	OBSTACLE_BALL,
	OBSTACLE_BAT
};

struct Obstacle {
	ObstacleType type;
	double x;
	double y;
	int width;
	int height;
	bool active;
	bool hitPlayer;
	bool fromRight;
	int frame;
	int frameTimer;
	double speed;
};

#define MAX_OBSTACLES 80
extern Obstacle obstacles[MAX_OBSTACLES];
extern int ballSpawnCount;
extern int batSpawnCount;
extern int lastObstacleSpawnTimer;

extern int imgBall[5];
extern int imgBat[3];
extern int imgSit[3];

void initObstacles() {
	ballSpawnCount = 0;
	batSpawnCount = 0;
	lastObstacleSpawnTimer = 0;
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		obstacles[i].type = OBSTACLE_BALL;
		obstacles[i].active = false;
		obstacles[i].hitPlayer = false;
		obstacles[i].fromRight = true;
		obstacles[i].x = -300;
		obstacles[i].y = GROUND_Y;
		obstacles[i].width = 180; // 2x ball size
		obstacles[i].height = 180;
		obstacles[i].frame = 0;
		obstacles[i].frameTimer = 0;
		obstacles[i].speed = 8.5;
	}
}

/* ---------- Boss Saint & Flash Projectile ---------- */
struct BossSaint {
	double x;
	double y;
	int health;
	int maxHealth;
	bool active;
	bool alive;
	bool isEntering;
	bool isFighting;
	int walkFrame;
	int walkTimer;
	int fightFrame;
	int fightTimer;
	int attackCooldown;
	int flashCooldown;
};

struct FlashProjectile {
	double x;
	double y;
	bool active;
	double speed;
};

extern BossSaint bossSaint;
extern FlashProjectile bossFlash;
extern bool isBossArena;
extern bool bossTriggered;

/* Saint & Flash Image Handles */
extern int imgSaintWalk[4];
extern int imgSaintFight[4];
extern int imgFlash;

void initBoss() {
	isBossArena = false;
	bossTriggered = false;
	bossGhostSpawnCount = 0;
	bossSkeletonSpawnCount = 0;
	lastBossMinionSpawnTimer = 0;
	bossSaint.x = SCREEN_W + 100;
	bossSaint.y = GROUND_Y;
	bossSaint.health = 300;
	bossSaint.maxHealth = 300;
	bossSaint.active = false;
	bossSaint.alive = true;
	bossSaint.isEntering = false;
	bossSaint.isFighting = false;
	bossSaint.walkFrame = 0;
	bossSaint.walkTimer = 0;
	bossSaint.fightFrame = 0;
	bossSaint.fightTimer = 0;
	bossSaint.attackCooldown = 0;
	bossSaint.flashCooldown = 0;

	bossFlash.x = -200;
	bossFlash.y = GROUND_Y + 70;
	bossFlash.active = false;
	bossFlash.speed = 10.0;
}

/* ---------- Level 4 Boss Mesh (Main Villain / King) ---------- */
struct BossMesh {
	double x;
	double y;
	int health;
	int maxHealth;
	bool active;
	bool alive;
	bool isEntering;
	bool isFighting;
	bool fromRight;
	int walkFrame;
	int walkTimer;
	int fightFrame;
	int fightTimer;
	int attackCooldown;
	int teleportTimer;
	int damageSinceTeleport;
	int vanishEffectTimer;
};

extern BossMesh bossMesh;
extern bool meshHasTeleported;
extern int nextMeshTeleportThreshold;
extern int lvl4SoldierSpawnCount;
extern int lvl4ArcherSpawnCount;
extern int lastLvl4MinionSpawnTimer;

/* Mesh Image Handles */
extern int imgMeshWalk[5];
extern int imgMeshFight[5];

void initMesh() {
	bossMesh.x = SCREEN_W + 100;
	bossMesh.y = GROUND_Y;
	bossMesh.health = 400;
	bossMesh.maxHealth = 400;
	bossMesh.active = false;
	bossMesh.alive = true;
	bossMesh.isEntering = false;
	bossMesh.isFighting = false;
	bossMesh.fromRight = true;
	bossMesh.walkFrame = 0;
	bossMesh.walkTimer = 0;
	bossMesh.fightFrame = 0;
	bossMesh.fightTimer = 0;
	bossMesh.attackCooldown = 0;
	bossMesh.teleportTimer = 0;
	bossMesh.damageSinceTeleport = 0;
	bossMesh.vanishEffectTimer = 0;
	meshHasTeleported = false;
	nextMeshTeleportThreshold = 350;

	lvl4SoldierSpawnCount = 0;
	lvl4ArcherSpawnCount = 0;
	lastLvl4MinionSpawnTimer = 0;
}

#endif
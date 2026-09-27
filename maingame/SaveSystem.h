#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include <windows.h>
#include <stdio.h>
#include <string.h>

#define SAVE_MAGIC 0x45435334 /* 'E', 'C', 'S', '4' - Eclipse Crown Save v4 (Dynamic Players) */
#define SAVE_VERSION 4
#define MAX_SAVED_PLAYERS 20

#pragma pack(push, 1)
struct PlayerRecord {
	char name[32];
	int score;                /* Valid enemy kills count */
	int highestLevel;         /* 1 to 4 */
	int levelProgressPct[4];  /* Progress percentage for Levels 1-4 */
	bool levelCompleted[4];   /* Completion flag for Levels 1-4 */
	int ghostKills;
	int skeletonKills;
	int soldierKills;
	int archerKills;
	int saintKills;
	int meshKills;
	int totalKills;
};

struct GameSaveData {
	unsigned int magic;
	unsigned int version;
	int numPlayers;           /* Total count of created player profiles (1..MAX_SAVED_PLAYERS) */
	int activePlayerIndex;    /* 0 to numPlayers - 1 */
	PlayerRecord players[MAX_SAVED_PLAYERS];
	bool musicEnabled;
	bool soundEnabled;
	unsigned int checksum;
};
#pragma pack(pop)

class SaveSystem {
private:
	GameSaveData data;
	char savePath[MAX_PATH];

	unsigned int computeChecksum(const GameSaveData* pData) {
		unsigned int sum = 0x55AA55AA;
		const unsigned char* bytes = (const unsigned char*)pData;
		size_t checkSize = sizeof(GameSaveData) - sizeof(unsigned int);
		for (size_t i = 0; i < checkSize; i++) {
			sum = ((sum << 5) + sum) + bytes[i];
		}
		return sum;
	}

	void resolveDataDirectory() {
		DWORD dwAttrib = GetFileAttributesA("data");
		if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
			CreateDirectoryA("data", NULL);
		}

		if (GetFileAttributesA("data") != INVALID_FILE_ATTRIBUTES) {
			sprintf_s(savePath, "data/savegame.bin");
		}
		else if (GetFileAttributesA("maingame/data") != INVALID_FILE_ATTRIBUTES) {
			CreateDirectoryA("maingame/data", NULL);
			sprintf_s(savePath, "maingame/data/savegame.bin");
		}
		else {
			sprintf_s(savePath, "data/savegame.bin");
		}
	}

	void initPlayerSlot(int idx, const char* name = NULL) {
		if (idx < 0 || idx >= MAX_SAVED_PLAYERS) return;
		if (name && strlen(name) > 0) {
			sprintf_s(data.players[idx].name, "%s", name);
		}
		else {
			sprintf_s(data.players[idx].name, "Hero %d", idx + 1);
		}
		data.players[idx].score = 0;
		data.players[idx].highestLevel = 1;
		data.players[idx].ghostKills = 0;
		data.players[idx].skeletonKills = 0;
		data.players[idx].soldierKills = 0;
		data.players[idx].archerKills = 0;
		data.players[idx].saintKills = 0;
		data.players[idx].meshKills = 0;
		data.players[idx].totalKills = 0;

		for (int lvl = 0; lvl < 4; lvl++) {
			data.players[idx].levelProgressPct[lvl] = 0;
			data.players[idx].levelCompleted[lvl] = false;
		}
	}

public:
	SaveSystem() {
		memset(&data, 0, sizeof(GameSaveData));
		savePath[0] = '\0';
	}

	void initDefaults() {
		data.magic = SAVE_MAGIC;
		data.version = SAVE_VERSION;
		data.numPlayers = 0; // Starts with 0 players! Players are created dynamically upon name input!
		data.activePlayerIndex = 0;
		data.musicEnabled = true;
		data.soundEnabled = true;

		memset(data.players, 0, sizeof(data.players));

		data.checksum = computeChecksum(&data);
	}

	void init() {
		resolveDataDirectory();

		FILE* fp = NULL;
		errno_t err = fopen_s(&fp, savePath, "rb");
		bool valid = false;

		if (err == 0 && fp != NULL) {
			GameSaveData loaded;
			size_t readCount = fread(&loaded, sizeof(GameSaveData), 1, fp);
			fclose(fp);

			if (readCount == 1) {
				if (loaded.magic == SAVE_MAGIC && loaded.version == SAVE_VERSION) {
					unsigned int expectedChecksum = computeChecksum(&loaded);
					if (loaded.checksum == expectedChecksum) {
						if (loaded.numPlayers >= 0 && loaded.numPlayers <= MAX_SAVED_PLAYERS) {
							data = loaded;
							valid = true;
							printf("[SaveSystem] Loaded dynamic save data from %s (%d Players)\n",
								savePath, data.numPlayers);
						}
					}
				}
			}
		}

		if (!valid) {
			printf("[SaveSystem] Initializing fresh dynamic binary save data (0 players)...\n");
			initDefaults();
			save();
		}
	}

	bool save() {
		resolveDataDirectory();
		data.magic = SAVE_MAGIC;
		data.version = SAVE_VERSION;
		data.checksum = computeChecksum(&data);

		FILE* fp = NULL;
		errno_t err = fopen_s(&fp, savePath, "wb");
		if (err == 0 && fp != NULL) {
			size_t written = fwrite(&data, sizeof(GameSaveData), 1, fp);
			fclose(fp);
			return (written == 1);
		}
		return false;
	}

	int getNumPlayers() const {
		return data.numPlayers;
	}

	int getActivePlayerIndex() const {
		return data.activePlayerIndex;
	}

	void setActivePlayerIndex(int idx) {
		if (idx >= 0 && idx < data.numPlayers) {
			data.activePlayerIndex = idx;
			save();
		}
	}

	void setPlayerName(int idx, const char* newName) {
		if (idx >= 0 && idx < data.numPlayers && newName && strlen(newName) > 0) {
			sprintf_s(data.players[idx].name, "%s", newName);
			save();
			printf("[SaveSystem] Renamed player slot %d to: %s\n", idx + 1, data.players[idx].name);
		}
	}

	bool addNewPlayer(const char* optionalName = NULL) {
		if (data.numPlayers < MAX_SAVED_PLAYERS) {
			int newIdx = data.numPlayers;
			initPlayerSlot(newIdx, optionalName);
			data.numPlayers++;
			data.activePlayerIndex = newIdx;
			save();
			printf("[SaveSystem] Added new dynamic player: %s (Total: %d)\n", data.players[newIdx].name, data.numPlayers);
			return true;
		}
		return false;
	}

	PlayerRecord& getActivePlayer() {
		if (data.numPlayers == 0) {
			static PlayerRecord dummy = {"Hero", 0, 1, {0,0,0,0}, {false,false,false,false}, 0, 0, 0, 0, 0, 0, 0};
			return dummy;
		}
		if (data.activePlayerIndex < 0 || data.activePlayerIndex >= data.numPlayers) {
			data.activePlayerIndex = 0;
		}
		return data.players[data.activePlayerIndex];
	}

	const PlayerRecord& getPlayer(int idx) const {
		if (data.numPlayers == 0) {
			static PlayerRecord dummy = {"Hero", 0, 1, {0,0,0,0}, {false,false,false,false}, 0, 0, 0, 0, 0, 0, 0};
			return dummy;
		}
		if (idx < 0 || idx >= data.numPlayers) idx = 0;
		return data.players[idx];
	}

	/* High score rule: +1 per valid enemy kill, single death event count */
	void recordValidKill(int enemyType) {
		PlayerRecord& p = getActivePlayer();
		p.score++;
		p.totalKills++;

		switch (enemyType) {
		case 0: /* ENEMY_GHOST */
			p.ghostKills++;
			break;
		case 1: /* ENEMY_SKELETON */
			p.skeletonKills++;
			break;
		case 2: /* ENEMY_SOLDIER */
			p.soldierKills++;
			break;
		case 3: /* ENEMY_ARCHER */
			p.archerKills++;
			break;
		case 4: /* BOSS_SAINT */
			p.saintKills++;
			break;
		case 5: /* BOSS_MESH */
			p.meshKills++;
			break;
		default:
			break;
		}

		save();
	}

	void updateLevelProgress(int level, int percentage) {
		if (level >= 1 && level <= 4) {
			PlayerRecord& p = getActivePlayer();
			if (percentage > p.levelProgressPct[level - 1]) {
				p.levelProgressPct[level - 1] = percentage;
				if (percentage > 100) p.levelProgressPct[level - 1] = 100;
			}
			if (level > p.highestLevel) {
				p.highestLevel = level;
			}
			save();
		}
	}

	void completeLevel(int level) {
		if (level >= 1 && level <= 4) {
			PlayerRecord& p = getActivePlayer();
			p.levelProgressPct[level - 1] = 100;
			p.levelCompleted[level - 1] = true;
			if (level < 4 && p.highestLevel <= level) {
				p.highestLevel = level + 1;
			}
			save();
		}
	}

	bool isMusicEnabled() const { return data.musicEnabled; }
	bool isSoundEnabled() const { return data.soundEnabled; }

	void setMusicEnabled(bool enabled) {
		data.musicEnabled = enabled;
		save();
	}

	void setSoundEnabled(bool enabled) {
		data.soundEnabled = enabled;
		save();
	}

	void clearAllData() {
		initDefaults();
		if (savePath[0] != '\0') {
			DeleteFileA(savePath);
		}
		printf("[SaveSystem] Cleared and freed all binary save data on exit.\n");
	}
};

extern SaveSystem gSaveSystem;

#endif

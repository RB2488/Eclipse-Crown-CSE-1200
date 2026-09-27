#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfuuid.lib")

enum BGMState {
	BGM_STATE_NONE,
	BGM_STATE_MENU,
	BGM_STATE_GAMEPLAY
};

class AudioManager {
private:
	BGMState currentBGM;
	bool mfInitialized;

	bool musicEnabled;
	bool soundEnabled;

	// Media Foundation handles for Game Bg.mp3
	IMFMediaSession* pMenuSession;
	IMFSourceResolver* pMenuResolver;
	IUnknown* pMenuSourceUnk;
	IMFMediaSource* pMenuSource;
	IMFPresentationDescriptor* pMenuPD;
	IMFTopology* pMenuTopology;
	bool menuBGMPlaying;

	// Media Foundation handles for Bat sound.m4a (Level 2 Bat Swarm)
	IMFMediaSession* pBatSession;
	IMFSourceResolver* pBatResolver;
	IUnknown* pBatSourceUnk;
	IMFMediaSource* pBatSource;
	IMFPresentationDescriptor* pBatPD;
	IMFTopology* pBatTopology;
	bool batAudioLoaded;
	bool batAudioPlaying;

	// MCI alias registration status
	bool gpbgLoaded;
	bool ghostEnterLoaded;
	bool hurtLoaded;
	bool heroFightLoaded;
	bool ghostFightLoaded;

	// Debounce timers (tick counts)
	int lastGhostComingTick;
	int lastHeroHurtTick;
	int lastHeroFightTick;
	int lastGhostFightTick;

	static bool resolvePath(const char* filename, char* outPath, int maxLen) {
		if (GetFileAttributesA(filename) != INVALID_FILE_ATTRIBUTES) {
			GetFullPathNameA(filename, maxLen, outPath, NULL);
			return true;
		}
		char temp[MAX_PATH];
		sprintf_s(temp, "Audios/%s", filename);
		if (GetFileAttributesA(temp) != INVALID_FILE_ATTRIBUTES) {
			GetFullPathNameA(temp, maxLen, outPath, NULL);
			return true;
		}
		sprintf_s(temp, "maingame/Audios/%s", filename);
		if (GetFileAttributesA(temp) != INVALID_FILE_ATTRIBUTES) {
			GetFullPathNameA(temp, maxLen, outPath, NULL);
			return true;
		}
		sprintf_s(temp, "../maingame/Audios/%s", filename);
		if (GetFileAttributesA(temp) != INVALID_FILE_ATTRIBUTES) {
			GetFullPathNameA(temp, maxLen, outPath, NULL);
			return true;
		}
		sprintf_s(outPath, maxLen, "%s", filename);
		return false;
	}

public:
	AudioManager() {
		currentBGM = BGM_STATE_NONE;
		mfInitialized = false;
		musicEnabled = true;
		soundEnabled = true;

		pMenuSession = NULL;
		pMenuResolver = NULL;
		pMenuSourceUnk = NULL;
		pMenuSource = NULL;
		pMenuPD = NULL;
		pMenuTopology = NULL;
		menuBGMPlaying = false;

		pBatSession = NULL;
		pBatResolver = NULL;
		pBatSourceUnk = NULL;
		pBatSource = NULL;
		pBatPD = NULL;
		pBatTopology = NULL;
		batAudioLoaded = false;
		batAudioPlaying = false;

		gpbgLoaded = false;
		ghostEnterLoaded = false;
		hurtLoaded = false;
		heroFightLoaded = false;
		ghostFightLoaded = false;

		lastGhostComingTick = -100;
		lastHeroHurtTick = -100;
		lastHeroFightTick = -100;
		lastGhostFightTick = -100;
	}

	void init() {
		HRESULT hr = MFStartup(MF_VERSION);
		mfInitialized = SUCCEEDED(hr);

		// 1. Setup Media Foundation for Game Bg.mp3 (Home/Menu background music)
		if (mfInitialized) {
			char bgPath[MAX_PATH];
			resolvePath("Game Bg.mp3", bgPath, MAX_PATH);
			WCHAR wPath[MAX_PATH];
			MultiByteToWideChar(CP_ACP, 0, bgPath, -1, wPath, MAX_PATH);

			hr = MFCreateMediaSession(NULL, &pMenuSession);
			hr = MFCreateSourceResolver(&pMenuResolver);
			MF_OBJECT_TYPE objType = MF_OBJECT_INVALID;
			hr = pMenuResolver->CreateObjectFromURL(wPath, MF_RESOLUTION_MEDIASOURCE, NULL, &objType, &pMenuSourceUnk);
			if (SUCCEEDED(hr) && pMenuSourceUnk) {
				pMenuSourceUnk->QueryInterface(IID_IMFMediaSource, (void**)&pMenuSource);
				hr = pMenuSource->CreatePresentationDescriptor(&pMenuPD);
				hr = MFCreateTopology(&pMenuTopology);

				DWORD cStreams = 0;
				if (pMenuPD) pMenuPD->GetStreamDescriptorCount(&cStreams);
				for (DWORD i = 0; i < cStreams; i++) {
					BOOL fSelected = FALSE;
					IMFStreamDescriptor* pSD = NULL;
					pMenuPD->GetStreamDescriptorByIndex(i, &fSelected, &pSD);
					if (fSelected) {
						IMFTopologyNode* pSourceNode = NULL;
						IMFTopologyNode* pOutputNode = NULL;
						IMFActivate* pSinkActivate = NULL;

						MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pSourceNode);
						pSourceNode->SetUnknown(MF_TOPONODE_SOURCE, pMenuSource);
						pSourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pMenuPD);
						pSourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD);

						MFCreateAudioRendererActivate(&pSinkActivate);
						MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pOutputNode);
						pOutputNode->SetObject(pSinkActivate);

						pMenuTopology->AddNode(pSourceNode);
						pMenuTopology->AddNode(pOutputNode);
						pSourceNode->ConnectOutput(0, pOutputNode, 0);

						pSourceNode->Release();
						pOutputNode->Release();
						pSinkActivate->Release();
					}
					if (pSD) pSD->Release();
				}
				pMenuSession->SetTopology(0, pMenuTopology);
			}
		}

		// 1b. Setup Media Foundation for Bat sound.m4a (Level 2 Bat Swarm looping audio)
		if (mfInitialized) {
			char batPath[MAX_PATH];
			resolvePath("Bat sound.m4a", batPath, MAX_PATH);
			WCHAR wBatPath[MAX_PATH];
			MultiByteToWideChar(CP_ACP, 0, batPath, -1, wBatPath, MAX_PATH);

			hr = MFCreateMediaSession(NULL, &pBatSession);
			hr = MFCreateSourceResolver(&pBatResolver);
			MF_OBJECT_TYPE objTypeBat = MF_OBJECT_INVALID;
			if (pBatResolver) {
				hr = pBatResolver->CreateObjectFromURL(wBatPath, MF_RESOLUTION_MEDIASOURCE, NULL, &objTypeBat, &pBatSourceUnk);
			}
			if (SUCCEEDED(hr) && pBatSourceUnk) {
				pBatSourceUnk->QueryInterface(IID_IMFMediaSource, (void**)&pBatSource);
				hr = pBatSource->CreatePresentationDescriptor(&pBatPD);
				hr = MFCreateTopology(&pBatTopology);

				DWORD cStreams = 0;
				if (pBatPD) pBatPD->GetStreamDescriptorCount(&cStreams);
				for (DWORD i = 0; i < cStreams; i++) {
					BOOL fSelected = FALSE;
					IMFStreamDescriptor* pSD = NULL;
					pBatPD->GetStreamDescriptorByIndex(i, &fSelected, &pSD);
					if (fSelected) {
						IMFTopologyNode* pSourceNode = NULL;
						IMFTopologyNode* pOutputNode = NULL;
						IMFActivate* pSinkActivate = NULL;

						MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pSourceNode);
						pSourceNode->SetUnknown(MF_TOPONODE_SOURCE, pBatSource);
						pSourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pBatPD);
						pSourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD);

						MFCreateAudioRendererActivate(&pSinkActivate);
						MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pOutputNode);
						pOutputNode->SetObject(pSinkActivate);

						pBatTopology->AddNode(pSourceNode);
						pBatTopology->AddNode(pOutputNode);
						pSourceNode->ConnectOutput(0, pOutputNode, 0);

						pSourceNode->Release();
						pOutputNode->Release();
						pSinkActivate->Release();
					}
					if (pSD) pSD->Release();
				}
				if (pBatSession && pBatTopology) {
					pBatSession->SetTopology(0, pBatTopology);
					batAudioLoaded = true;
				}
			}
		}

		// 2. Setup MCI for AIFF audio files
		char path[MAX_PATH];
		char cmd[512];

		// Gameplay BG
		resolvePath("GamePlay Bg sound.aiff", path, MAX_PATH);
		sprintf_s(cmd, "open \"%s\" alias gpbg", path);
		if (mciSendStringA(cmd, NULL, 0, NULL) == 0) {
			gpbgLoaded = true;
			mciSendStringA("setaudio gpbg volume to 450", NULL, 0, NULL);
		}

		// Ghost Coming Screen
		resolvePath("Gosht coming screen..aiff", path, MAX_PATH);
		sprintf_s(cmd, "open \"%s\" alias ghost_enter", path);
		if (mciSendStringA(cmd, NULL, 0, NULL) == 0) ghostEnterLoaded = true;

		// Hero Hurt / Heart
		resolvePath("Hero hurt .aiff", path, MAX_PATH);
		sprintf_s(cmd, "open \"%s\" alias hero_hurt", path);
		if (mciSendStringA(cmd, NULL, 0, NULL) == 0) hurtLoaded = true;

		// Hero Sword Fight (Hero vs Skeletons, Soldiers, Archers, Saint, Mesh)
		resolvePath("Hero sowrd fight .aiff", path, MAX_PATH);
		sprintf_s(cmd, "open \"%s\" alias hero_fight", path);
		if (mciSendStringA(cmd, NULL, 0, NULL) == 0) heroFightLoaded = true;

		// Ghost Sword Fight (Hero vs Flying Ghost)
		resolvePath("Soward gosh fight.aiff", path, MAX_PATH);
		sprintf_s(cmd, "open \"%s\" alias ghost_fight", path);
		if (mciSendStringA(cmd, NULL, 0, NULL) == 0) ghostFightLoaded = true;

		printf("[Audio] Initialized audio assets.\n");
	}

	bool isMusicEnabled() const { return musicEnabled; }
	bool isSoundEnabled() const { return soundEnabled; }

	void setMusicEnabled(bool enabled) {
		musicEnabled = enabled;
		if (!musicEnabled) {
			stopMenuBGM();
			stopGameplayBGM();
		}
	}

	void setSoundEnabled(bool enabled) {
		soundEnabled = enabled;
		if (!soundEnabled) {
			stopBatSound();
		}
	}

	void toggleMusic() {
		setMusicEnabled(!musicEnabled);
	}

	void toggleSound() {
		setSoundEnabled(!soundEnabled);
	}

	void playMenuBGM() {
		if (!musicEnabled) return;
		if (currentBGM == BGM_STATE_MENU && menuBGMPlaying) return;

		// Stop gameplay BGM if running
		stopGameplayBGM();

		if (pMenuSession) {
			PROPVARIANT varStart;
			PropVariantInit(&varStart);
			varStart.vt = VT_I8;
			varStart.hVal.QuadPart = 0; // Start from beginning
			pMenuSession->Start(&GUID_NULL, &varStart);
			menuBGMPlaying = true;
			currentBGM = BGM_STATE_MENU;
		}
	}

	void stopMenuBGM() {
		if (pMenuSession && menuBGMPlaying) {
			pMenuSession->Stop();
			menuBGMPlaying = false;
		}
		if (currentBGM == BGM_STATE_MENU) currentBGM = BGM_STATE_NONE;
	}

	void playGameplayBGM() {
		if (!musicEnabled) return;

		// Stop menu BGM if running
		stopMenuBGM();

		if (gpbgLoaded) {
			mciSendStringA("stop gpbg", NULL, 0, NULL);
			mciSendStringA("seek gpbg to start", NULL, 0, NULL);
			mciSendStringA("play gpbg from 0", NULL, 0, NULL);
			mciSendStringA("setaudio gpbg volume to 450", NULL, 0, NULL);
			currentBGM = BGM_STATE_GAMEPLAY;
		}
	}

	void stopGameplayBGM() {
		if (gpbgLoaded) {
			mciSendStringA("stop gpbg", NULL, 0, NULL);
		}
		if (currentBGM == BGM_STATE_GAMEPLAY) currentBGM = BGM_STATE_NONE;
	}

	// Update background music state depending on active screen & music switch
	void updateBGM(int screenId, bool isGameplayActive, int currentTick) {
		// Screen 0 is SCREEN_SPLASH -> strictly NO music on Splash Screen!
		if (!musicEnabled || screenId == 0) {
			if (currentBGM == BGM_STATE_MENU) stopMenuBGM();
			if (currentBGM == BGM_STATE_GAMEPLAY) stopGameplayBGM();
			return;
		}

		if (isGameplayActive) {
			if (currentBGM != BGM_STATE_GAMEPLAY) {
				playGameplayBGM();
			}
			else if (gpbgLoaded && currentTick % 30 == 0) {
				// Periodic check (every ~0.5s) for seamless continuous looping of gameplay BGM without MCI bus flooding
				char statusBuf[64] = { 0 };
				mciSendStringA("status gpbg mode", statusBuf, sizeof(statusBuf), NULL);
				if (strcmp(statusBuf, "playing") != 0 && strcmp(statusBuf, "seeking") != 0) {
					mciSendStringA("play gpbg from 0", NULL, 0, NULL);
				}
			}
		}
		else {
			// Non-gameplay screens (Home, Story, Options, Credits, End Card)
			if (currentBGM != BGM_STATE_MENU) {
				playMenuBGM();
			}
			else if (pMenuSession && menuBGMPlaying) {
				// Loop Media Foundation menu BGM when ended
				IMFMediaEvent* pEvent = NULL;
				HRESULT hr = pMenuSession->GetEvent(MF_EVENT_FLAG_NO_WAIT, &pEvent);
				if (SUCCEEDED(hr) && pEvent) {
					MediaEventType meType = MEUnknown;
					pEvent->GetType(&meType);
					if (meType == MEEndOfPresentation || meType == MESessionEnded) {
						PROPVARIANT varStart;
						PropVariantInit(&varStart);
						varStart.vt = VT_I8;
						varStart.hVal.QuadPart = 0;
						pMenuSession->Start(&GUID_NULL, &varStart);
					}
					pEvent->Release();
				}
			}
		}
	}

	void resetDebounceTimers() {
		lastGhostComingTick = -100;
		lastHeroHurtTick = -100;
		lastHeroFightTick = -100;
		lastGhostFightTick = -100;
	}

	// 1. Ghost Coming sound on wave entrance (debounced per entry event)
	void playGhostComingSound(int currentTick) {
		if (!soundEnabled) return;
		if (currentTick >= lastGhostComingTick && (currentTick - lastGhostComingTick) < 45 && lastGhostComingTick >= 0) return;
		lastGhostComingTick = currentTick;
		if (ghostEnterLoaded) {
			mciSendStringA("seek ghost_enter to start", NULL, 0, NULL);
			mciSendStringA("play ghost_enter", NULL, 0, NULL);
		}
	}

	// 2. Ghost Fighting sound (ONLY Hero vs Flying Ghost)
	void playGhostSwordFightSound(int currentTick = 0) {
		if (!soundEnabled) return;
		lastGhostFightTick = currentTick;
		if (ghostFightLoaded) {
			mciSendStringA("play ghost_fight from 0", NULL, 0, NULL);
		}
	}

	// 3. Hero / Tim Fighting sound (Hero vs Skeleton, Soldier, Archer, Saint, Mesh / Air Swing)
	void playHeroSwordFightSound(int currentTick = 0) {
		if (!soundEnabled) return;
		lastHeroFightTick = currentTick;
		if (heroFightLoaded) {
			mciSendStringA("play hero_fight from 0", NULL, 0, NULL);
		}
	}

	void stopCombatSounds() {
		if (heroFightLoaded) {
			mciSendStringA("stop hero_fight", NULL, 0, NULL);
		}
		if (ghostFightLoaded) {
			mciSendStringA("stop ghost_fight", NULL, 0, NULL);
		}
	}

	void playBatSound() {
		if (!soundEnabled) return;
		if (batAudioPlaying) return;
		if (pBatSession && batAudioLoaded) {
			PROPVARIANT varStart;
			PropVariantInit(&varStart);
			varStart.vt = VT_I8;
			varStart.hVal.QuadPart = 0; // Start from beginning
			pBatSession->Start(&GUID_NULL, &varStart);
			batAudioPlaying = true;
		}
	}

	void stopBatSound() {
		if (pBatSession && batAudioPlaying) {
			pBatSession->Stop();
			batAudioPlaying = false;
		}
	}

	// Update looping bat sound when Level 2 bats are on screen
	void updateBatSound(bool hasActiveBats) {
		if (!soundEnabled || !hasActiveBats) {
			if (batAudioPlaying) {
				stopBatSound();
			}
			return;
		}

		if (!batAudioPlaying) {
			playBatSound();
		}
		else if (pBatSession && batAudioPlaying) {
			// Seamless looping when audio reaches end
			IMFMediaEvent* pEvent = NULL;
			HRESULT hr = pBatSession->GetEvent(MF_EVENT_FLAG_NO_WAIT, &pEvent);
			if (SUCCEEDED(hr) && pEvent) {
				MediaEventType meType = MEUnknown;
				pEvent->GetType(&meType);
				if (meType == MEEndOfPresentation || meType == MESessionEnded) {
					PROPVARIANT varStart;
					PropVariantInit(&varStart);
					varStart.vt = VT_I8;
					varStart.hVal.QuadPart = 0;
					pBatSession->Start(&GUID_NULL, &varStart);
				}
				pEvent->Release();
			}
		}
	}

	// 4. Hero Hurt / Heart sound on actual damage taken (Single trigger protection)
	void playHeroHurtSound(int currentTick) {
		if (!soundEnabled) return;
		if (currentTick >= lastHeroHurtTick && (currentTick - lastHeroHurtTick) < 25 && lastHeroHurtTick >= 0) return;
		lastHeroHurtTick = currentTick;
		if (hurtLoaded) {
			mciSendStringA("seek hero_hurt to start", NULL, 0, NULL);
			mciSendStringA("play hero_hurt", NULL, 0, NULL);
		}
	}

	void shutdown() {
		stopMenuBGM();
		stopGameplayBGM();
		stopBatSound();
		mciSendStringA("close all", NULL, 0, NULL);

		if (pBatTopology) { pBatTopology->Release(); pBatTopology = NULL; }
		if (pBatPD) { pBatPD->Release(); pBatPD = NULL; }
		if (pBatSource) { pBatSource->Release(); pBatSource = NULL; }
		if (pBatSourceUnk) { pBatSourceUnk->Release(); pBatSourceUnk = NULL; }
		if (pBatResolver) { pBatResolver->Release(); pBatResolver = NULL; }
		if (pBatSession) { pBatSession->Close(); pBatSession->Release(); pBatSession = NULL; }

		if (pMenuTopology) { pMenuTopology->Release(); pMenuTopology = NULL; }
		if (pMenuPD) { pMenuPD->Release(); pMenuPD = NULL; }
		if (pMenuSource) { pMenuSource->Release(); pMenuSource = NULL; }
		if (pMenuSourceUnk) { pMenuSourceUnk->Release(); pMenuSourceUnk = NULL; }
		if (pMenuResolver) { pMenuResolver->Release(); pMenuResolver = NULL; }
		if (pMenuSession) { pMenuSession->Close(); pMenuSession->Release(); pMenuSession = NULL; }
		if (mfInitialized) {
			MFShutdown();
			mfInitialized = false;
		}
	}
};

extern AudioManager gAudio;

#endif

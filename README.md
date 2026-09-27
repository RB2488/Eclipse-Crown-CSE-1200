# Eclipse Crown

## Game Description

**Eclipse Crown** is a 2D side-scrolling action-adventure game built using the **iGraphics** library in C/C++. The hero fights through 4 distinct levels filled with ghosts, skeletons, soldiers, and archers, culminating in boss battles against **Saint** and the final villain, **King Mesh**. The project demonstrates graphics rendering, sprite animation, collision detection, enemy AI, special abilities, and a persistent binary save system with dynamic player profiles.

## Features
- **4 Levels** with unique enemies and mechanics:
  - Level 1 – Ghosts & Skeletons (side-scrolling journey)
  - Level 2 – Fireball/Bat obstacle gauntlet + Boss Saint arena fight
  - Level 3 – Soldier & Archer horde (100 each) with special powers
  - Level 4 – Final Boss arena against King Mesh (with minion support)
- **Special Powers (Level 3):**
  - `N` – Summon Army: spawns 2 friendly Ghosts + 1 friendly Skeleton to fight alongside the hero
  - `M` – Manipulate Wave: unleashes a directional shockwave that instantly defeats nearby enemies and destroys arrows
- **Dynamic Player Profiles** – create, rename, and switch between multiple saved profiles (up to 20), each tracking score, kills per enemy type, and level completion/progress
- **Persistent Binary Save System** – all progress, stats, and settings are saved to `data/savegame.bin`
- **Full Audio System** – background music (menu & gameplay) and sound effects (combat, hits, ghost warnings) with independent on/off toggles
- **Animated HUD** – health cards, boss health bars, cooldown dials for special powers, and a level-progress bar
- **Story Screens** – narrative cutscenes between story beats
- **Sword Combat** – click-to-attack with directional hit detection, deflection of arrows/projectiles, and knockback-free enemy elimination

## Project Details
IDE: Visual Studio 2013

Language: C, C++

Platform: Windows PC

Genre: 2D Side-Scrolling Action-Adventure

## How to Run the Project

Make sure you have the following installed:
- **Visual Studio 2013**
- **MinGW Compiler** (if needed)
- **iGraphics Library** (included in this repository)

Open the project in Visual Studio 2013
- Open Visual Studio 2013.
- Go to File → Open → Project/Solution.
- Locate and select the `.sln` file from the cloned repository.
- Click Build → Build Solution
- Run the program by clicking Debug → Start Without Debugging

## How to Play

### **Controls**

| Action              | Key(s)                          |
|---------------------|----------------------------------|
| Move Left           | `A` / `←` Left Arrow             |
| Move Right          | `D` / `→` Right Arrow            |
| Jump                | `W` / `↑` Up Arrow                |
| Crouch / Sit         | `S` / `↓` Down Arrow              |
| Sword Attack         | Left Mouse Click                  |
| Summon Army (Lvl 3)  | `N`                                |
| Manipulate Wave (Lvl 3) | `M`                             |
| Pause / Resume       | `Space`                            |
| Restart Level        | `R`                                |
| Return to Home Menu  | `H`                                |
| Toggle Fullscreen    | `F` / `F11`                        |
| Back / Exit          | `Esc`                              |

### **Game Rules**

- The hero starts each level with **200 HP**.
- Enemies deal damage on contact/attack:
  - Ghost: -5 HP (proximity)
  - Skeleton: -10 HP (melee)
  - Soldier: -10 HP (melee)
  - Archer: -10 HP (arrow hit)
  - Boss Saint: -15 HP (melee) / -15 HP (Flash projectile)
  - Boss Mesh: -15 HP (melee)
- Defeating enemies restores HP in certain levels (e.g. every 2–3 kills of a given enemy type grants +20 HP).
- In Level 3, the **Manipulate** power instantly clears all enemies in a 500px directional zone, while **Summon Army** brings temporary allies into the fight.
- Boss Mesh teleports across the arena every time it crosses a 50 HP damage threshold.
- Completing a level records progress and stats to the active player's save profile.
- Defeating **King Mesh** in Level 4 completes the game and unlocks the Victory End Card.

## Project Contributors

1. MD Rabiul Islam Bhuiyan Seyam
2. Afia Farzana
3. Injamamul Haque Piash
4. MD Ajmain Abir

## Youtube Link
[CSE 1200 Project: Eclipse Crown](https://youtu.be/ol9r5b6XwZI)

## Project Report
[Project Report: Eclipse Crown](PASTE_REPORT_LINK_HERE) // This part hasn't been created yet

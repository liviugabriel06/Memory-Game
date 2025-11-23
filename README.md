# 🃏 Memory Game (WinAPI Implementation)

## 📘 Project Overview

This is a robust implementation of the classic **Memory / Concentration** game, developed entirely in **C** using the native **Windows API (WinAPI)**.

The project demonstrates advanced low-level programming concepts, including manual memory management, dynamic resource loading, DLL integration for game logic, and a custom-built UI state machine. It features a fully responsive design with selectable themes, dynamic difficulty, and persistent high scores.

## ✨ Key Features

### 🎮 Core Gameplay
* **Dynamic Difficulty:** Three grid sizes to choose from:
    * **Easy:** 4x4 (16 cards)
    * **Medium:** 6x6 (36 cards)
    * **Hard:** 10x10 (100 cards)
* **Preview Mode:** A 1.5-second "peek" at all cards before the game starts to test short-term memory.
* **Game Header:** A custom top bar displaying real-time **Time**, **Moves**, and the current **Player Name**.
* **Pause System:** Full Pause/Resume functionality that freezes the timer and locks the board interaction.

### 🎨 Customization & Themes
* **Card Themes:** Players can switch between 4 distinct card sets via the "Themes" menu:
    * 🍎 **Fruits** (Default)
    * 🥕 **Vegetables**
    * 🃏 **UNO Cards**
    * ♠️ **Classic Playing Cards**
* **Background Color:** A "Change Background" button cycles through **20 different colors**, ranging from classic felt green/brown to vibrant modern hues.

### 🏆 Scoring & Persistence
* **High Score System:** Tracks the best performances based on Time (primary) and Moves (secondary).
* **Local Persistence:** Scores are saved to `highscore.txt` and loaded dynamically to display a **Top 5 Leaderboard** for each difficulty level.
* **Player Identity:** Supports custom player names input before starting a match.

### ⚙️ Technical Architecture
* **DLL Integration:** The card shuffling algorithm is encapsulated in a separate Dynamic Link Library (`ProjectDLL.dll`) loaded at runtime.
* **Resource Management:** Uses a `.rc` resource script to manage bitmaps and menus. Supports dynamic switching of bitmap resources at runtime based on the selected theme.
* **Window Management:** The application window automatically centers itself on the screen and resizes precisely to fit the selected grid size + header.

---

## 🛠️ Build & Installation

This project was developed using **Code::Blocks** with the **MinGW** compiler.

### Prerequisites
* Windows OS (Required for WinAPI).
* MinGW/GCC Compiler.
* **Important:** All image assets must be **24-bit Bitmaps (.bmp)**.

### Folder Structure
To ensure correct compilation and resource loading, the project uses a flat structure for assets:

```text
/Project
    ├── main.c              # Main source code
    ├── resource.rc         # Resource script
    ├── resource.h          # Header for IDs
    ├── ProjectDLL.dll      # Compiled Logic Library
    ├── highscore.txt       # Score storage
    └── *.bmp               # All image assets (cherry.bmp, carrot.bmp, etc.) MUST be here
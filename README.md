# 🃏 Memory Game (Concentration) - WinAPI

## Project Overview

This project is an implementation of the classic **Memory Game (Concentration)**, developed in **C** using the native Windows API (WinAPI). It aims to provide a responsive and feature-rich gaming experience, focusing on dynamic board sizing and a comprehensive high-score system.

## ✨ Key Features

| Category | Feature | Description |
| :--- | :--- | :--- |
| **Game Modes** | **Dynamic Difficulty** | Supports three board sizes: **Easy (4x4)**, **Medium (6x6)**, and **Hard (10x10)**, dynamically adjusting card size (`cellSize`) and window dimensions. |
| **User Experience** | **Game Header** | A dedicated top bar displays live **Time (seconds)**, **Moves**, and the **Player Name** for quick tracking. |
| **Scoring** | **Persistent High Scores** | Scores (Name, Difficulty, Time, Moves) are saved to a local file (`highscore.txt`). The **Show Scores** feature displays the global Top 5 ranking per difficulty, sorted by the lowest time, then fewest moves. |
| **Control** | **Preview Mode** | Each game begins with a 1.5-second preview where all cards are face-up, testing the player's short-term memory before the timer starts. |
| **Control** | **Pause/Resume** | A dedicated button allows players to **pause** and **resume** the game, stopping the timer and interaction. |
| **Control** | **Escape Key (Exit)** | Pressing **ESC** immediately terminates the current game (or difficulty selection) and returns the player to the main menu. |
| **Architecture** | **DLL Integration** | The core board shuffling logic is designed to be loaded dynamically from an external **DLL** (`ProjectDLL.dll`), with a fallback shuffle implementation included. |

---

## 🛠️ Build and Requirements

### Requirements

* **Compiler:** A C compiler environment supporting WinAPI (e.g., MinGW/GCC, Visual Studio).
* **Dependencies:** The standard Windows API header (`windows.h`) and standard C libraries (`stdio.h`, `time.h`).
* **Resources:** Bitmap files (`IDB_BACK`, `IDB_IMG1`-`IDB_IMG4`) are expected to be included via a resource file (`.rc`).
* **DLL (Optional but Recommended):** `ProjectDLL.dll` must be present in the executable directory for the main shuffling logic.

### Building the Project

1.  **Compile Resources:** Compile the resource file (`.rc`).
2.  **Compile Code:** Compile `main.c` linking the resources and required libraries.
3.  **Ensure DLL is present:** Place the `ProjectDLL.dll` file in the same directory as the resulting executable.

### Running the Game

1.  Execute the compiled program. The window will open **centered** on your screen.
2.  Select **PLAY** to choose a difficulty level. You can enter your **Player Name** in the edit box provided.
3.  Enjoy the game! Try to match all pairs in the fastest time and fewest moves.

---

## 📈 Score Persistence

Scores are saved in the `highscore.txt` file using a pipe-separated format:
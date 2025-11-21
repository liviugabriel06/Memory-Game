# Memory Game (WinAPI / C)

A classic memory matching game developed using the native Windows API (WinAPI) and the C programming language. This project demonstrates core low-level programming skills, including manual window creation, message handling, custom graphics rendering, and dynamic DLL integration.

---

## ✨ Features

* **Custom UI:** The application uses native WinAPI components (not high-level frameworks) for the main window, controls, and rendering.
* **Difficulty Levels:** Supports three grid sizes to challenge the player:
    * **Easy:** 4x4 grid (16 cards)
    * **Medium:** 6x6 grid (36 cards)
    * **Hard:** 10x10 grid (100 cards)
* **Dynamic Graphics:** Cards are dynamically resized (`StretchBlt`) and padded based on the selected difficulty to ensure optimal visual fit.
* **Game State Management:** Implements a state machine to handle Menu, Difficulty Selection, and Active Game phases.
* **Preview Mode:** Displays all cards for 2 seconds at the start of the game for the classic memory challenge experience.
* **Timing & Scoring:** Includes a real-time timer and move counter.
* **High Score System:** Saves the best time and scores to a local file (`highscore.txt`).
* **Modular Design:** Uses a separate Dynamic Link Library (`ProjectDLL.dll`) to handle the core board initialization and shuffling logic.

---

## 🛠️ Build and Installation

This project was developed using **Code::Blocks** with the **MinGW** compiler.

### Prerequisites

* C Compiler (e.g., GCC/MinGW)
* Windows operating system (WinAPI required)

### Steps to Run Locally

1.  **Clone the Repository:**
    ```bash
    git clone [Your-GitHub-Repository-Link]
    cd memory-game-winapi
    ```
2.  **Build the DLL:** Open the `ProjectDLL` project file in Code::Blocks and build it.
3.  **Build the Main Executable:** Open the main project file (`.cbp`) and build the executable.
4.  **Copy DLL:** Ensure the generated `ProjectDLL.dll` is copied into the same directory as the main executable (`.exe`).
5.  **Run:** Execute the `.exe` file.

---

## 📜 Project Structure (Key Files)

* `main.c`: Main program entry point, handles all window procedures (`WndProc`), game logic, UI interactions, timers, and drawing.
* `resource.h`, `resource.rc`: Defines all button IDs and loads bitmap resources (images).
* `ProjectDLL/main.c`: Contains the exported function `InitBoard` for shuffling the cards (using Fisher-Yates algorithm).
* `highscore.txt`: Persistence file for saving best game results.
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "resource.h"

// --- CONFIGURARE ---
#define ORIGINAL_IMG_SIZE 100 // Dimensiunea reala a imaginilor BMP (100x100)
#define GAP 10                // Spatiul dintre carti (pixeli)
#define MAX_CARDS 100         // 10x10 maxim
#define PREVIEW_TIME 1500     // Durata de previzualizare (2000ms = 2 secunde)

// --- STARI ALE JOCULUI ---
enum GameState { STATE_MENU, STATE_DIFFICULTY, STATE_GAME };
enum GameState currentState = STATE_MENU;

// --- GLOBALE ---
typedef void (*LPFN_INITBOARD)(int*, int);
HMODULE hDll = NULL;
LPFN_INITBOARD pInitBoard = NULL;
HINSTANCE hInst;

BOOL previewRunning = FALSE;

int board[MAX_CARDS];
int state[MAX_CARDS];
HBITMAP hBmp[5];

int rows = 4, cols = 4;
int cellSize = 100; // --- MODIFICARE: Variabila, nu constanta (se schimba in fct de dificultate)
int totalCards = 16;
int firstSelection = -1;
int moves = 0;
int gameTime = 0;

// UI Elements
HWND hBtnPlay, hBtnHelp;
HWND hBtnEasy, hBtnMed, hBtnHard;

// --- FUNCTII ---

void SaveScore() {
    FILE *f = fopen("highscore.txt", "a");
    if (f) {
        fprintf(f, "Time: %d sec | Score(moves): %d\n", gameTime, moves);
        fclose(f);
    }
}

// Functie auxiliara pentru a calcula marimea ferestrei
void ResizeWindow(HWND hWnd) {
    if (currentState == STATE_MENU || currentState == STATE_DIFFICULTY) {
        // Marime fixa pentru meniu
        RECT rc = {0, 0, 400, 350};
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        SetWindowPos(hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
    }
    else if (currentState == STATE_GAME) {
        // --- MODIFICARE: Calculam latimea incluzand spatiile (GAPS) ---
        int width = cols * (cellSize + GAP) + GAP;
        int height = rows * (cellSize + GAP) + GAP;

        RECT rc = {0, 0, width, height};
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        // Centram fereastra pe ecran (optional, dar arata mai bine)
        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);
        int x = (screenW - (rc.right - rc.left)) / 2;
        int y = (screenH - (rc.bottom - rc.top)) / 2;

        SetWindowPos(hWnd, NULL, x, y, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
    }
}

void UpdateUI(HWND hWnd) {
    ShowWindow(hBtnPlay, SW_HIDE);
    ShowWindow(hBtnHelp, SW_HIDE);
    ShowWindow(hBtnEasy, SW_HIDE);
    ShowWindow(hBtnMed, SW_HIDE);
    ShowWindow(hBtnHard, SW_HIDE);

    if (currentState == STATE_MENU) {
        ShowWindow(hBtnPlay, SW_SHOW);
        ShowWindow(hBtnHelp, SW_SHOW);
        SetWindowText(hWnd, "Memory Game - Main Menu");
    }
    else if (currentState == STATE_DIFFICULTY) {
        ShowWindow(hBtnEasy, SW_SHOW);
        ShowWindow(hBtnMed, SW_SHOW);
        ShowWindow(hBtnHard, SW_SHOW);
        SetWindowText(hWnd, "Choose Difficulty");
    }
    else if (currentState == STATE_GAME) {
        SetTimer(hWnd, 1, 1000, NULL);
    }

    ResizeWindow(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);
}

void StartGame(HWND hWnd, int r, int c) {
    rows = r;
    cols = c;
    totalCards = rows * cols;
    moves = 0;
    gameTime = 0;
    firstSelection = -1;

    // Setam marimea celulei
    if (cols == 4) cellSize = 100;
    else if (cols == 6) cellSize = 80;
    else cellSize = 55;

    // --- LOGICA DE AMESTECARE ---
    if (pInitBoard) {
        // Varianta 1: Folosim DLL-ul (Ideal)
        pInitBoard(board, totalCards);
    } else {
        // Varianta 2: Fallback (Daca DLL lipseste) - AMESTECAM MANUAL AICI
        // Pas A: Umplem ordonat
        for(int i=0; i<totalCards; i++) {
             board[i] = (i / 2) + 1; // Perechi 1,1, 2,2...
        }

        // Pas B: Amestecam (Shuffle local)
        srand(time(NULL));
        for (int i = totalCards - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int temp = board[i];
            board[i] = board[j];
            board[j] = temp;
        }
    }

    // Setam toate cartile la vizibile (state = 1)
    for(int i=0; i<totalCards; i++) state[i] = 1;
    for(int i=totalCards; i<MAX_CARDS; i++) state[i] = 0;

    currentState = STATE_GAME;
    UpdateUI(hWnd);

    // --- MODIFICARE AICI: Setam variabila de stare la TRUE ---
    previewRunning = TRUE;

    // Pornim Timer-ul de Previzualizare (ID=2)
    SetTimer(hWnd, 2, PREVIEW_TIME, NULL);
}

void LoadImages(HINSTANCE hInst) {
    hBmp[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BACK));
    hBmp[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_IMG1));
    hBmp[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_IMG2));
    hBmp[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_IMG3));
    hBmp[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_IMG4));
}

void DrawBoard(HDC hdc) {
    if (currentState != STATE_GAME) return;

    HDC hMemDC = CreateCompatibleDC(hdc);
    // Setam modul de redimensionare sa fie de calitate (sa nu arate pixelat cand micsoram)
    SetStretchBltMode(hdc, HALFTONE);

    for (int i = 0; i < totalCards; i++) {
        int r = i / cols;
        int c = i % cols;

        // --- MODIFICARE: Calculam pozitia X si Y adaugand GAP-ul ---
        int x = GAP + c * (cellSize + GAP);
        int y = GAP + r * (cellSize + GAP);

        HBITMAP hToDraw;

        if (state[i] == 0) {
            hToDraw = hBmp[0];
        } else {
            int imgIndex = (board[i] - 1) % 4 + 1;
            hToDraw = hBmp[imgIndex];
        }

        SelectObject(hMemDC, hToDraw);

        // --- MODIFICARE: Folosim StretchBlt in loc de BitBlt ---
        // StretchBlt ia imaginea originala (100x100) si o "indesara" in noua marime (cellSize x cellSize)
        StretchBlt(hdc, x, y, cellSize, cellSize, hMemDC, 0, 0, ORIGINAL_IMG_SIZE, ORIGINAL_IMG_SIZE, SRCCOPY);
    }
    DeleteDC(hMemDC);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        hDll = LoadLibrary("ProjectDLL.dll");
        if (hDll) pInitBoard = (LPFN_INITBOARD)GetProcAddress(hDll, "InitBoard");

        LoadImages(((LPCREATESTRUCT)lParam)->hInstance);

        // Butoane Meniu
        hBtnPlay = CreateWindow("BUTTON", "PLAY", WS_CHILD | BS_PUSHBUTTON,
                                130, 80, 120, 40, hWnd, (HMENU)ID_BTN_PLAY, hInst, NULL);
        hBtnHelp = CreateWindow("BUTTON", "HOW TO PLAY", WS_CHILD | BS_PUSHBUTTON,
                                130, 140, 120, 40, hWnd, (HMENU)ID_BTN_HELP, hInst, NULL);

        // Butoane Dificultate
        hBtnEasy = CreateWindow("BUTTON", "EASY (4x4)", WS_CHILD | BS_PUSHBUTTON,
                                130, 60, 120, 30, hWnd, (HMENU)ID_BTN_EASY, hInst, NULL);
        hBtnMed  = CreateWindow("BUTTON", "MEDIUM (6x6)", WS_CHILD | BS_PUSHBUTTON,
                                130, 110, 120, 30, hWnd, (HMENU)ID_BTN_MED, hInst, NULL);
        hBtnHard = CreateWindow("BUTTON", "HARD (10x10)", WS_CHILD | BS_PUSHBUTTON,
                                130, 160, 120, 30, hWnd, (HMENU)ID_BTN_HARD, hInst, NULL);

        UpdateUI(hWnd);
        break;
    }

    case WM_TIMER:
        if (wParam == 1) { // Timer-ul de Joc
            if (currentState == STATE_GAME) {
                gameTime++;
                char title[100];
                sprintf(title, "Memory Game | Time: %d s | Moves: %d", gameTime, moves);
                SetWindowText(hWnd, title);
            }
        }

        // Timer-ul de Previzualizare (ID=2)
        else if (wParam == 2) {
            KillTimer(hWnd, 2); // Oprim timer-ul de previzualizare

            // Setam variabila de stare la FALSE. ACEASTA ESTE LINIA CRUCIALA.
            previewRunning = FALSE;

            // Ascundem toate cartile (setam starea la 0)
            for(int i=0; i<totalCards; i++) state[i] = 0;

            // Pornim Timer-ul de Joc (ID=1)
            SetTimer(hWnd, 1, 1000, NULL);

            InvalidateRect(hWnd, NULL, TRUE); // Redesenam ecranul
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rc; GetClientRect(hWnd, &rc);
        HBRUSH hBrush = CreateSolidBrush(RGB(139, 69, 19)); // Fundal Maro
        FillRect(hdc, &rc, hBrush);
        DeleteObject(hBrush);

        if (currentState == STATE_GAME) {
            DrawBoard(hdc);
        }
        else {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));
            if (currentState == STATE_MENU)
                TextOut(hdc, 150, 40, "MAIN MENU", 9);
            else if (currentState == STATE_DIFFICULTY)
                TextOut(hdc, 130, 30, "SELECT DIFFICULTY", 17);
        }

        EndPaint(hWnd, &ps);
    } break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case ID_BTN_PLAY:
                currentState = STATE_DIFFICULTY;
                UpdateUI(hWnd);
                break;

            case ID_BTN_HELP:
                MessageBox(hWnd,
                    "Click to reveal cards. Find matching pairs.\nGL & HF!",
                    "How To Play", MB_OK | MB_ICONINFORMATION);
                break;

            case ID_BTN_EASY: StartGame(hWnd, 4, 4); break;
            case ID_BTN_MED:  StartGame(hWnd, 6, 6); break;
            case ID_BTN_HARD: StartGame(hWnd, 10, 10); break;
        }
        break;

    case WM_LBUTTONDOWN: {
        // Blocheaza click-urile daca nu suntem in joc SAU daca previzualizarea ruleaza.
        if (currentState != STATE_GAME || previewRunning == TRUE) break;

        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);

        // Aflam pe ce coloana/rand suntem (fara a ne pasa de GAP la calculul indexului)
        int c = mouseX / (cellSize + GAP);
        int r = mouseY / (cellSize + GAP);

        // Daca indexul calculat este in afara tablei, sau click-ul a fost in GAP (zona maro)
        if (c >= cols || r >= rows) break;
        if (mouseX % (cellSize + GAP) < GAP || mouseY % (cellSize + GAP) < GAP) break;

        int idx = r * cols + c;

        // Blocheaza click-ul pe carti deja intoarse sau rezolvate
        if (state[idx] != 0) break;

        // Logica de joc (restul codului ramane la fel ca inainte)
        if (firstSelection == -1) {
            state[idx] = 1;
            firstSelection = idx;
            InvalidateRect(hWnd, NULL, FALSE);
        } else {
            state[idx] = 1;
            InvalidateRect(hWnd, NULL, FALSE);
            UpdateWindow(hWnd);

            moves++;
            char title[100];
            sprintf(title, "Memory Game | Time: %d s | Moves: %d", gameTime, moves);
            SetWindowText(hWnd, title);

            // Verificarea potrivirii vizuale (Modulo 4)
            int imgVizuala1 = (board[firstSelection] - 1) % 4;
            int imgVizuala2 = (board[idx] - 1) % 4;

            if (imgVizuala1 == imgVizuala2) {
                // ESTE MATCH!
                state[firstSelection] = 2;
                state[idx] = 2;

                Sleep(100);

                int finished = 1;
                for(int i=0; i<totalCards; i++) if(state[i] != 2) finished = 0;

                if(finished) {
                    KillTimer(hWnd, 1);
                    SaveScore();
                    char msg[100];
                    sprintf(msg, "Victory!\nTime: %d sec\nScore: %d moves", gameTime, moves);
                    MessageBox(hWnd, msg, "Winner", MB_OK);

                    currentState = STATE_MENU;
                    UpdateUI(hWnd);
                }

            } else {
                // NU SE POTRIVESC
                Sleep(500);
                state[firstSelection] = 0;
                state[idx] = 0;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            firstSelection = -1;
        }
    } break;

    case WM_DESTROY:
        if (hDll) FreeLibrary(hDll);
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance; // Initializare globala

    WNDCLASSEX wcex = {sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance,
                       LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
                       (HBRUSH)GetStockObject(BLACK_BRUSH),
                       NULL, "MemoryClass", NULL};

    RegisterClassEx(&wcex);
    HWND hWnd = CreateWindow("MemoryClass", "Memory Game", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, 0, 400, 300, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

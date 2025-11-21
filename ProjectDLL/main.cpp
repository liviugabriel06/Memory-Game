#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <process.h> // Pentru _getpid

// Exportăm funcția de amestecare
__declspec(dllexport) void InitBoard(int* board, int size) {
    int i;

    // 1. Umplem array-ul cu perechi ordonate: 1, 1, 2, 2, 3, 3...
    for (i = 0; i < size; i++) {
        board[i] = (i / 2) + 1;
    }

    // 2. Initializam generatorul de numere aleatoare
    // Combinam timpul curent cu ID-ul procesului pentru unicitate garantata
    srand(time(NULL) ^ _getpid());

    // 3. Amestecăm (Algoritmul Fisher-Yates Shuffle)
    // Parcurgem lista de la sfarsit spre inceput
    for (i = size - 1; i > 0; i--) {
        // Alegem un index aleatoriu intre 0 si i
        int j = rand() % (i + 1);

        // Schimbam elementul curent (i) cu cel aleatoriu (j)
        int temp = board[i];
        board[i] = board[j];
        board[j] = temp;
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return TRUE;
}

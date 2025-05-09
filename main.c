#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "game.h"
#include "move_validation.h"

int main() {
    GameState gs = initializeGame();
    char start[3], finish[3];
    int x1, y1, x2, y2;

    while (1) {
        // Afișează tabla de șah
        printTabla(gs.tabla);
        printf("Jucător curent: %s\n", gs.currentPlayer ? "Negru" : "Alb");

        // Solicită save/load/exit/mutarea piesei
        printf("Introduceți 'save' pentru a salva jocul, 'load' pentru a încărca un joc, 'exit' pentru a ieși sau mutarea (ex: e2 e4): ");
        scanf("%s", start);

        if (strcmp(start, "save") == 0) {
            salveazaJoc(&gs, "chess_game_save.txt");
            continue;
        } else if (strcmp(start, "load") == 0) {
            reincepereJoc(&gs, "chess_game_save.txt");
            continue;
        } else if (strcmp(start, "exit") == 0) {
            inchideJoc();
        }

        // Continuă cu mutarea
        scanf("%s", finish);
        transformareMiscare(start, finish, &x1, &y1, &x2, &y2);

        // Verifică validitatea mutării
        int valid = validareMiscare(x1, y1, x2, y2, &gs);
        if (valid == 1) {
            executa_mutare(x1, y1, x2, y2, &gs);
            if (isCheckmate(&gs)) {
                printf("Șah mat! %s câștigă!\n", gs.currentPlayer ? "Alb" : "Negru");
                break;
            }
            gs.currentPlayer = !gs.currentPlayer;
        } else if (valid == -1) {
            continue;
        } else {
            printf("Mutare invalidă!\n");
        }
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "move_validation.h"

int main() {
    GameState gs = initializeGame();
    char start[3], finish[3];
    int x1, y1, x2, y2;

    while (1) {
        printTabla(gs.tabla);
        printf("Introduceti mutarea (ex: e2 e4): ");
        scanf("%s %s", start, finish);

        transformareMiscare(start, finish, &x1, &y1, &x2, &y2);

        if (validareMiscare(x1, y1, x2, y2, &gs)) {
            //executa_mutare(x1, y1, x2, y2, &gs);
            if (isCheckmate(&gs)) {
                printf("Șah mat! %s câștigă!\n", gs.currentPlayer ? "Alb" : "Negru");
                break;
            }
            gs.currentPlayer = !gs.currentPlayer;
        } else {
            printf("Mutare invalidă!\n");
        }
    }
    return 0;
}

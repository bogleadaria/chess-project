#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "game.h"
#include "move_validation.h"
#include "ai.h"

// Verifică dacă există vreo mutare legală pentru jucătorul curent (pentru pat/remiză)
int existaMutareLegala(GameState *gs) {
    for (int x1 = 0; x1 < 8; x1++)
        for (int y1 = 0; y1 < 8; y1++)
            if ((gs->currentPlayer == 0 && isupper(gs->tabla[x1][y1])) ||
                (gs->currentPlayer == 1 && islower(gs->tabla[x1][y1])))
                for (int x2 = 0; x2 < 8; x2++)
                    for (int y2 = 0; y2 < 8; y2++)
                        if (validareMiscare(x1, y1, x2, y2, gs) == 1)
                            return 1;
    return 0;
}

int main() {

    printf("TEST\n"); fflush(stdout);

    GameState gs = initializeGame();

    printf("currentPlayer la start: %d\n", gs.currentPlayer); fflush(stdout);
    printf("Piesa la e2: %c\n", gs.tabla[6][4]); fflush(stdout);


    char start[3], finish[3];
    int x1, y1, x2, y2;

    printf("ȘAH cu AI (Alb: TU, Negru: ENGINE)\n");

    while (1) {
        printTabla(gs.tabla);
        printf("Jucător curent: %s\n", gs.currentPlayer ? "Negru (AI)" : "Alb (TU)");

        // Rândul AI-ului (negru)
        if (gs.currentPlayer == 1) {
            printf("Engine-ul gândește...\n");
            Move best = findBestMove(&gs);
            if (best.x1 == -1) {
                printf("Nicio mutare validă! Remiză sau mat.\n");
                break;
            }
            printf("Engine alege: %c%d-%c%d (Evaluare: %.2f)\n",
                   'a' + best.y1, 8 - best.x1, 'a' + best.y2, 8 - best.x2, best.score);
            executa_mutare(best.x1, best.y1, best.x2, best.y2, &gs);

            // Verifică șah mat
            if (isCheckmate(&gs)) {
                printTabla(gs.tabla);
                printf("Șah mat! Negru (AI) câștigă!\n");
                break;
            }

            // Verifică pat (remiză)
            if (!existaMutareLegala(&gs)) {
                if (isInCheck(&gs, gs.currentPlayer)) {
                    printf("Eroare logică: ar fi trebuit să fie șah mat!\n");
                } else {
                    printTabla(gs.tabla);
                    printf("Remiză prin pat! Nicio mutare legală și regele NU este în șah.\n");
                    break;
                }
            }

            // Verifică dacă adversarul este în șah după mutare
            if (isInCheck(&gs, gs.currentPlayer)) {
                printf("Atenție: %s este în șah!\n", gs.currentPlayer ? "Negru" : "Alb");
            }

            gs.currentPlayer = !gs.currentPlayer;
            continue;
        }

        // Rândul omului (alb)
        printf("Introduceți 'save' pentru a salva, 'load' pentru a încărca, 'exit' pentru a ieși sau mutarea (ex: e2 e4): ");
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

        scanf("%s", finish);
        transformareMiscare(start, finish, &x1, &y1, &x2, &y2);

        printf("Coordonate: x1=%d y1=%d x2=%d y2=%d\n", x1, y1, x2, y2); fflush(stdout);

        int valid = validareMiscare(x1, y1, x2, y2, &gs);

        if (valid == 1) {
            executa_mutare(x1, y1, x2, y2, &gs);

            // Verifică șah mat
            if (isCheckmate(&gs)) {
                printTabla(gs.tabla);
                printf("Șah mat! Alb (TU) câștigă!\n");
                break;
            }

            // Verifică pat (remiză)
            if (!existaMutareLegala(&gs)) {
                if (isInCheck(&gs, gs.currentPlayer)) {
                    printf("Eroare logică: ar fi trebuit să fie șah mat!\n");
                } else {
                    printTabla(gs.tabla);
                    printf("Remiză prin pat! Nicio mutare legală și regele NU este în șah.\n");
                    break;
                }
            }

            // Verifică dacă adversarul este în șah după mutare
            if (isInCheck(&gs, gs.currentPlayer)) {
                printf("Atenție: %s este în șah!\n", gs.currentPlayer ? "Negru" : "Alb");
            }

            gs.currentPlayer = !gs.currentPlayer;
        } else if (valid == 0) {
            char piece = gs.tabla[x1][y1];
            if (piece == ' ' ||
                (gs.currentPlayer == 0 && islower(piece)) ||
                (gs.currentPlayer == 1 && isupper(piece))) {
                printf("Jucător greșit! Nu poți muta piesa adversarului.\n");
            } else {
                printf("Mutare invalidă!\n");
            }
        } else if (valid == -1) {
            printf("Mutare ilegală! Nu poți lăsa regele în șah.\n");
        }
    }

    return 0;
}
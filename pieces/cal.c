#include <stdlib.h>
#include "pieces.h"

int Cal(int x1, int y1, int x2, int y2, GameState *gs) {
    char piece = gs->tabla[x1][y1];
    if ((piece == 'C' || piece == 'c') && x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0) {
        if ((abs(x1 - x2) == 2 && abs(y1 - y2) == 1) || (abs(x1 - x2) == 1 && abs(y1 - y2) == 2)) {
            if (piece == 'C') {
                // Cal alb poate captura piese negre sau merge pe gol
                if (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'a' && gs->tabla[x2][y2] <= 'z'))
                    return 1;
            } else {
                // Cal negru poate captura piese albe sau merge pe gol
                if (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'A' && gs->tabla[x2][y2] <= 'Z'))
                    return 1;
            }
        }
    }
    return 0;
}
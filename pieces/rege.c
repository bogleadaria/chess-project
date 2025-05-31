#include <stdlib.h>
#include "pieces.h"

int Rege(int x1, int y1, int x2, int y2, GameState *gs) {
    char piece = gs->tabla[x1][y1];
    if ((piece != 'R' && piece != 'r') || x2 < 0 || x2 >= 8 || y2 < 0 || y2 >= 8)
        return 0;

    if ((abs(x1 - x2) <= 1) && (abs(y1 - y2) <= 1) && !(x1 == x2 && y1 == y2)) {
        if (piece == 'R')
            return (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'a' && gs->tabla[x2][y2] <= 'z'));
        else
            return (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'A' && gs->tabla[x2][y2] <= 'Z'));
    }
    return 0;
}
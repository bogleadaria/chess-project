#include <stdlib.h>
#include "pieces.h"

int Dama(int x1, int y1, int x2, int y2, GameState *gs) {
    char piece = gs->tabla[x1][y1];
    if ((piece == 'D' || piece == 'd') && x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0) {
        int dx = (x2 > x1) ? 1 : -1;
        int dy = (y2 > y1) ? 1 : -1;
        int i, j;

        // Mutare pe linie (orizontal)
        if (x1 == x2) {
            for (j = y1 + dy; j != y2; j += dy)
                if (gs->tabla[x1][j] != ' ')
                    return 0;
        }
        // Mutare pe coloană (vertical)
        else if (y1 == y2) {
            for (i = x1 + dx; i != x2; i += dx)
                if (gs->tabla[i][y1] != ' ')
                    return 0;
        }
        // Mutare pe diagonală
        else if (abs(x1 - x2) == abs(y1 - y2)) {
            i = x1 + dx;
            j = y1 + dy;
            while (i != x2 && j != y2) {
                if (gs->tabla[i][j] != ' ')
                    return 0;
                i += dx;
                j += dy;
            }
        } else {
            return 0;
        }

        // Validare captură/corectitudine destinație
        if (piece == 'D')
            return (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'a' && gs->tabla[x2][y2] <= 'z'));
        else
            return (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'A' && gs->tabla[x2][y2] <= 'Z'));
    }
    return 0;
}
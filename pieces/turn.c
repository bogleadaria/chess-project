#include <stdlib.h>
#include "pieces.h"

int Turn(int x1, int y1, int x2, int y2, GameState *gs) {
    char piece = gs->tabla[x1][y1];
    if (piece != 'T' && piece != 't') return 0;

    // Mutarea trebuie să fie pe linie sau coloană
    if (x1 != x2 && y1 != y2) return 0;

    // Verifică obstacole între poziții
    if (x1 == x2) {
        int step = (y2 > y1) ? 1 : -1;
        for (int j = y1 + step; j != y2; j += step)
            if (gs->tabla[x1][j] != ' ')
                return 0;
    } else {
        int step = (x2 > x1) ? 1 : -1;
        for (int i = x1 + step; i != x2; i += step)
            if (gs->tabla[i][y1] != ' ')
                return 0;
    }

    // Validare captură/corectitudine destinație
    if (piece == 'T')
        return (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'a' && gs->tabla[x2][y2] <= 'z'));
    else
        return (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'A' && gs->tabla[x2][y2] <= 'Z'));
}
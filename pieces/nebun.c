#include <stdlib.h>
#include "pieces.h"

int Nebun(int x1, int y1, int x2, int y2, GameState *gs) {
    char piece = gs->tabla[x1][y1];
    // Verifică dacă piesa este nebun și dacă mutarea e pe diagonală și pe tablă
    if ((piece == 'N' || piece == 'n') &&
        x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0 &&
        abs(x1 - x2) == abs(y1 - y2)) {

        int dx = (x2 > x1) ? 1 : -1;
        int dy = (y2 > y1) ? 1 : -1;
        int i = x1 + dx, j = y1 + dy;
        while (i != x2 && j != y2) {
            if (gs->tabla[i][j] != ' ')
                return 0; // Există obstacol pe diagonală
            i += dx;
            j += dy;
        }
        // Captură/corectitudine destinație
        if (piece == 'N')
            return (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'a' && gs->tabla[x2][y2] <= 'z'));
        else
            return (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'A' && gs->tabla[x2][y2] <= 'Z'));
    }
    return 0;
}
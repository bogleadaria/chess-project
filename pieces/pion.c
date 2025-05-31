#include <stdio.h>
#include <stdlib.h>

#include "pieces.h"

int Pion(int x1, int y1, int x2, int y2, GameState *gs)
{
    if (gs->tabla[x1][y1] == 'P' && x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0)
    {
        if (y1 == y2 && x2 == x1 - 1 && gs->tabla[x2][y2] == ' ')
            return 1;
        if (y1 == y2 && x1 == 6 && x2 == 4 && gs->tabla[5][y1] == ' ' && gs->tabla[4][y1] == ' ')
            return 1;
        if ((abs(y1 - y2) == 1) && (x2 == x1 - 1) && (gs->tabla[x2][y2] >= 'a' && gs->tabla[x2][y2] <= 'z'))
            return 1;
    }
    else if (gs->tabla[x1][y1] == 'p')
    {
        if (y1 == y2 && x2 == x1 + 1 && gs->tabla[x2][y2] == ' ')
            return 1;
        if (y1 == y2 && x1 == 1 && x2 == 3 && gs->tabla[2][y1] == ' ' && gs->tabla[3][y1] == ' ')
            return 1;
        if ((abs(y1 - y2) == 1) && (x2 == x1 + 1) && gs->tabla[x2][y2] >= 'A' && gs->tabla[x2][y2] <= 'Z')
            return 1;
    }

    if (gs->enPassantTarget[0] == x2 && gs->enPassantTarget[1] == y2 &&
        abs(y2 - y1) == 1 && ((gs->tabla[x1][y1] == 'P' && x2 == x1 - 1) || (gs->tabla[x1][y1] == 'p' && x2 == x1 + 1)))
    {
        gs->tabla[x1][y2] = ' ';
        return 1;
    }
    return 0;
}
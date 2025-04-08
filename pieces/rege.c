#include <stdio.h>
#include <stdlib.h>

#include "pieces.h"

int Rege(int x1, int y1, int x2, int y2, GameState *gs)
{
    if (gs->tabla[x1][y1] == 'R' && x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0)
    {
        if ((x1 == x2) && (y2 == y1 + 1))
            return 1;
        if ((x1 == x2) && (y2 == y1 - 1))
            return 1;
        if ((y1 == y2) && (x2 == x1 + 1))
            return 1;
        if ((y1 == y2) && (x2 == x1 - 1))
            return 1;
        if ((x2 == x1 + 1) && (y2 == y1 + 1))
            return 1;
        if ((x2 == x1 + 1) && (y2 == y1 - 1))
            return 1;
        if ((x2 == x1 - 1) && (y2 == y1 + 1))
            return 1;
        if ((x2 == x1 - 1) && (y2 == y1 - 1))
            return 1;
    }
    return 0;
}

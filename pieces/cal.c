#include <stdio.h>
#include <stdlib.h>

#include "pieces.h"

int Cal(int x1, int y1, int x2, int y2, GameState *gs)
{
    if (gs->tabla[x1][y1] == 'C' && x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0)
    {
        if ((abs(x1 - x2) == 2 && abs(y1 - y2) == 1) || (abs(x1 - x2) == 1 && abs(y1 - y2) == 2))
        {
            if (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'a' && gs->tabla[x2][y2] <= 'z'))
            {
                return 1;
            }
        }
    }
    return 0;
}

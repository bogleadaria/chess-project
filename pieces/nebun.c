#include <stdio.h>
#include <stdlib.h>

#include "pieces.h"

int Nebun(int x1, int y1, int x2, int y2, GameState *gs)
{
    if (gs->tabla[x1][y1] == 'N' && x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0 && abs(x1 - x2) == abs(y1 - y2))
    {
        int dx = (x2 > x1) ? 1 : -1;
        int dy = (y2 > y1) ? 1 : -1;
        int i = x1 + dx, j = y1 + dy;
        while (i != x2 && j != y2)
        {
            if (gs->tabla[i][j] != ' ')
                return 0;
            i += dx;
            j += dy;
        }
        return (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'a' && gs->tabla[x2][y2] <= 'z'));
    }
    return 0;
}

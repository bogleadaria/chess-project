#include <stdio.h>
#include <stdlib.h>

#include "pieces.h"

int Turn(int x1, int y1, int x2, int y2, GameState *gs)
{
    if (gs->tabla[x1][y1] == 'T')
    {
        if (x1 == x2)
        {
            int dy = (y2 > y1) ? 1 : -1;
            for (int j = y1 + dy; j != y2; j += dy)
                if (gs->tabla[x1][j] != ' ')
                    return 0;
        }
        else if (y1 == y2)
        {
            int dx = (x2 > x1) ? 1 : -1;
            for (int i = x1 + dx; i != x2; i += dx)
                if (gs->tabla[i][y1] != ' ')
                    return 0;
        }
        else
            return 0;
        return (gs->tabla[x2][y2] == ' ' || (gs->tabla[x2][y2] >= 'a' && gs->tabla[x2][y2] <= 'z'));
    }
    return 0;
}

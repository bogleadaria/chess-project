#ifndef PIECES_H
#define PIECES_H

#include "game.h"

int Pion(int x1, int y1, int x2, int y2, GameState *gs);

int Cal(int x1, int y1, int x2, int y2, GameState *gs);

int Nebun(int x1, int y1, int x2, int y2, GameState *gs);

int Turn(int x1, int y1, int x2, int y2, GameState *gs);

int Dama(int x1, int y1, int x2, int y2, GameState *gs);

int Rege(int x1, int y1,int x2,int y2 ,GameState*);

#endif // PIECES_H

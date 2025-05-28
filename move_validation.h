#ifndef MOVE_VALIDATION_H
#define MOVE_VALIDATION_H

#include "game.h"

void transformareMiscare(char *start, char *finish, int *x1, int *y1, int *x2, int *y2, bool culoare_ai);
int validareMiscare(int x1, int y1, int x2, int y2, GameState *gs);
int validareRocada(int x1, int y1, int x2, int y2, GameState *gs);
void executa_mutare(int x1, int y1, int x2, int y2, GameState *gs);
int existaMutareLegala(GameState *gs);
int mutareIeseDinSah(int x1, int y1, int x2, int y2, GameState *gs);
#endif
#ifndef MOVE_VALIDATION_H
#define MOVE_VALIDATION_H

#include "game.h"

void transformareMiscare(char *start, char *finish, int *x1, int *y1, int *x2, int *y2);
int validareMiscare(int x1, int y1, int x2, int y2, GameState *gs);
int validareRocada(int x1, int y1, int x2, int y2, GameState *gs);
void executa_mutare( int x1, int y1, int x2, int y2, GameState *gs);
int existaMutareLegala(GameState *gs);
int mutareIeseDinSah(int x1, int y1, int x2, int y2, GameState *gs);
int esteRemiza(GameState *gs);
int endsWith(const char *fisier, const char *sufix);
void transformareInversa(int x1, int y1, int x2, int y2, int *x1_t, int *y1_t, int *x2_t, int *y2_t);
void salveazaIstoricMutari(GameState *gs, int x1, int y1, int x2, int y2);
#endif
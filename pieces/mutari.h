#ifndef MUTARI_H
#define MUTARI_H

#include "game.h"

typedef struct {
    int x1, y1, x2, y2;
    float scor;
} Move;

typedef struct {
    Move mutari[128];
    int count;
} MoveList;

void initMoveList(MoveList *lista);

void genereazaMutariTurn(int x, int y, GameState *gs, MoveList *lista);
void genereazaToateMutarileTurn(GameState *gs, MoveList *lista, int player);

void genereazaMutariRege(int x, int y, GameState *gs, MoveList *lista);
void genereazaToateMutarileRege(GameState *gs, MoveList *lista, int player);

void genereazaMutariPion(int x, int y, GameState *gs, MoveList *lista);
void genereazaToateMutarilePion(GameState *gs, MoveList *lista, int player);

void genereazaMutariNebun(int x, int y, GameState *gs, MoveList *lista);
void genereazaToateMutarileNebun(GameState *gs, MoveList *lista, int player);

void genereazaMutariDama(int x, int y, GameState *gs, MoveList *lista);
void genereazaToateMutarileDama(GameState *gs, MoveList *lista, int player);

void genereazaMutariCal(int x, int y, GameState *gs, MoveList *lista);
void genereazaToateMutarileCal(GameState *gs, MoveList *lista, int player);

#endif
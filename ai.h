#ifndef AI_H
#define AI_H

#include "game.h"
#include "mutari.h"

#define INF 100000
#define DEPTH 3

Move findBestMove(GameState *gs);
void inverseaza_sir(float *sir);
void initializeaza_tabele_negre();
float minimax(GameState *gs, int depth, float alpha, float beta, int maximizingPlayer);
float evaluatePosition(GameState *gs);

#endif



// INF este o valoare mare pentru reprezentarea infinitului
// DEPTH controlează adâncimea de căutare
// Funcția findBestMove va returna cea mai bună mutare
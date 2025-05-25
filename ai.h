#ifndef AI_H
#define AI_H

#include "game.h"

#define INF 100000
#define DEPTH 3

typedef struct {
    int x1, y1, x2, y2;
    float score;
} Move;

Move findBestMove(GameState *gs);
float minimax(GameState *gs, int depth, float alpha, float beta, int maximizingPlayer);
float evaluatePosition(GameState *gs);

#endif


// Structura Move stochează coordonatele și scorul mutării
// INF este o valoare mare pentru reprezentarea infinitului
// DEPTH controlează adâncimea de căutare
// Funcția findBestMove va returna cea mai bună mutare
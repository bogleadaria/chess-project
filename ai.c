#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "ai.h"
#include "move_validation.h"
#include "mutari.h"

// Tabele poziționale pentru piese albe
float tabla_pion_a[64] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.5, 1.0, 1.0, -2.0, -2.0, 1.0, 1.0, 0.5,
    0.5, -0.5, -1.0, 0.0, 0.0, -1.0, -0.5, 0.5,
    0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0,
    0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5,
    1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0,
    5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

float tabla_cal_a[64] = {
    -5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0,
    -4.0, -2.0, 0.0, 0.0, 0.0, 0.0, -2.0, -4.0,
    -3.0, 0.0, 1.0, 1.5, 1.5, 1.0, 0.0, -3.0,
    -3.0, 0.5, 1.5, 2.0, 2.0, 1.5, 0.5, -3.0,
    -3.0, 0.0, 1.5, 2.0, 2.0, 1.5, 0.0, -3.0,
    -3.0, 0.5, 1.0, 1.5, 1.5, 1.0, 0.5, -3.0,
    -4.0, -2.0, 0.0, 0.5, 0.5, 0.0, -2.0, -4.0,
    -5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0};

float tabla_nebun_a[64] = {
    -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0,
    -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0,
    -1.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0, -1.0,
    -1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, -1.0,
    -1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, -1.0,
    -1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0,
    -1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, -1.0,
    -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0};

float tabla_turn_a[64] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5,
    -0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
    -0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
    -0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
    -0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
    -0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
    0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0};

float tabla_dama_a[64] = {
    -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0,
    -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0,
    -1.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0,
    -0.5, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5,
    0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5,
    -1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0,
    -1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, -1.0,
    -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0};

float tabla_rege_a[64] = {
    -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
    -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
    -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
    -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
    -2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0,
    -1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0,
    2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0,
    2.0, 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0};

// Tabele poziționale pentru piese negre
float tabla_pion_n[64];
float tabla_cal_n[64];
float tabla_nebun_n[64];
float tabla_turn_n[64];
float tabla_dama_n[64];
float tabla_rege_n[64];

void inverseaza_sir(float *sir)
{
    int i, j;
    float aux;
    for (i = 0, j = 63; i < j; i++, j--)
    {
        aux = sir[i];
        sir[i] = sir[j];
        sir[j] = aux;
    }
}

void initializeaza_tabele_negre()
{
    memcpy(tabla_pion_n, tabla_pion_a, sizeof(tabla_pion_a));
    inverseaza_sir(tabla_pion_n);
    memcpy(tabla_cal_n, tabla_cal_a, sizeof(tabla_cal_a));
    inverseaza_sir(tabla_cal_n);
    memcpy(tabla_nebun_n, tabla_nebun_a, sizeof(tabla_nebun_a));
    inverseaza_sir(tabla_nebun_n);
    memcpy(tabla_turn_n, tabla_turn_a, sizeof(tabla_turn_a));
    inverseaza_sir(tabla_turn_n);
    memcpy(tabla_dama_n, tabla_dama_a, sizeof(tabla_dama_a));
    inverseaza_sir(tabla_dama_n);
    memcpy(tabla_rege_n, tabla_rege_a, sizeof(tabla_rege_a));
    inverseaza_sir(tabla_rege_n);
}

float evaluatePosition(GameState *gs)
{
    float scor = 0.0f;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            char piece = gs->tabla[i][j];
            int pos = i * 8 + j;
            switch (piece)
            {
            case 'P':
                scor += 1.0 + tabla_pion_a[pos];
                break;
            case 'C':
                scor += 3.0 + tabla_cal_a[pos];
                break;
            case 'N':
                scor += 3.0 + tabla_nebun_a[pos];
                break;
            case 'T':
                scor += 5.0 + tabla_turn_a[pos];
                break;
            case 'D':
                scor += 9.0 + tabla_dama_a[pos];
                break;
            case 'R':
                scor += 100.0 + tabla_rege_a[pos];
                break;
            case 'p':
                scor -= 1.0 + tabla_pion_n[63 - pos];
                break;
            case 'c':
                scor -= 3.0 + tabla_cal_n[63 - pos];
                break;
            case 'n':
                scor -= 3.0 + tabla_nebun_n[63 - pos];
                break;
            case 't':
                scor -= 5.0 + tabla_turn_n[63 - pos];
                break;
            case 'd':
                scor -= 9.0 + tabla_dama_n[63 - pos];
                break;
            case 'r':
                scor -= 100.0 + tabla_rege_n[63 - pos];
                break;
            }
        }
    }

    // Bonusuri/penalizări suplimentare
    if (gs->canWhiteCastleKingside)
        scor += 0.5;
    if (gs->canBlackCastleKingside)
        scor -= 0.5;
    return scor;
}

float minimax(GameState *gs, int depth, float alpha, float beta, int maximizingPlayer)
{
    if (depth == 0 || isCheckmate(gs))
    {
        return evaluatePosition(gs);
    }
    MoveList lista;
    initMoveList(&lista);

    int player = maximizingPlayer ? 0 : 1;

    // Generează toate mutările pentru jucătorul curent
    genereazaToateMutarilePion(gs, &lista, player);
    genereazaToateMutarileCal(gs, &lista, player);
    genereazaToateMutarileNebun(gs, &lista, player);
    genereazaToateMutarileTurn(gs, &lista, player);
    genereazaToateMutarileDama(gs, &lista, player);
    genereazaToateMutarileRege(gs, &lista, player);

    if (lista.count == 0)
    {
        return evaluatePosition(gs);
    }

    if (maximizingPlayer)
    {
        float maxEval = -INF;
        for (int i = 0; i < lista.count; i++)
        {
            GameState copie = *gs;
            Move m = lista.mutari[i];
            executa_mutare(m.x1, m.y1, m.x2, m.y2, &copie);
            float eval = minimax(&copie, depth - 1, alpha, beta, 0);
            if (eval > maxEval)
                maxEval = eval;
            if (eval > alpha)
                alpha = eval;
            if (beta <= alpha)
                break;
        }
        return maxEval;
    }
    else
    {
        float minEval = INF;
        for (int i = 0; i < lista.count; i++)
        {
            GameState copie = *gs;
            Move m = lista.mutari[i];
            executa_mutare(m.x1, m.y1, m.x2, m.y2, &copie);
            float eval = minimax(&copie, depth - 1, alpha, beta, 1);
            if (eval < minEval)
                minEval = eval;
            if (eval < beta)
                beta = eval;
            if (beta <= alpha)
                break;
        }
        return minEval;
    }
}

Move findBestMove(GameState *gs, bool culoare_ai, Move avoid)
{
    MoveList lista;
    initMoveList(&lista);
    int jucator = gs->currentPlayer;

    // Generate all moves
    genereazaToateMutarilePion(gs, &lista, jucator);
    genereazaToateMutarileCal(gs, &lista, jucator);
    genereazaToateMutarileNebun(gs, &lista, jucator);
    genereazaToateMutarileTurn(gs, &lista, jucator);
    genereazaToateMutarileDama(gs, &lista, jucator);
    genereazaToateMutarileRege(gs, &lista, jucator);

    Move bestMove;
    if (culoare_ai == 0)
    {
        bestMove.scor = -INF;
        for (int i = 0; i < lista.count; i++)
        {
            GameState copie = *gs;
            Move m = lista.mutari[i];
            executa_mutare(m.x1, m.y1, m.x2, m.y2, &copie);
            float scor = minimax(&copie, DEPTH - 1, -INF, INF, 0); // maximizingPlayer=0 pentru adversar (negru)
            if (scor > bestMove.scor || i == 0)
            {
                bestMove = m;
                bestMove.scor = scor;
            }
        }
    }
    else
    {
        bestMove.scor = INF;
        for (int i = 0; i < lista.count; i++)
        {
            GameState copie = *gs;
            Move m = lista.mutari[i];
            executa_mutare(m.x1, m.y1, m.x2, m.y2, &copie);
            float scor = minimax(&copie, DEPTH - 1, -INF, INF, 1); // maximizingPlayer=1 pentru adversar (alb)
            if (scor < bestMove.scor || i == 0)
            {
                bestMove = m;
                bestMove.scor = scor;
            }
        }
    }
    return bestMove;
}
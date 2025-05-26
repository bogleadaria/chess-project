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
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.5,  1.0,  1.0, -2.0, -2.0,  1.0,  1.0,  0.5,
    0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5,
    0.0,  0.0,  0.0,  2.0,  2.0,  0.0,  0.0,  0.0,
    0.5,  0.5,  1.0,  2.5,  2.5,  1.0,  0.5,  0.5,
    1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0,
    5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0
 };
 
 float tabla_cal_a[64] = {
     -5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0,
     -4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0,
     -3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0,
     -3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0,
     -3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0,
     -3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0,
     -4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0,
     -5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0
  };
 
  float tabla_nebun_a[64] = {
     -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0,
     -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
     -1.0,  0.0,  0.5,  1.0,  1.0,  0.5,  0.0, -1.0,
     -1.0,  0.5,  0.5,  1.0,  1.0,  0.5,  0.5, -1.0,
     -1.0,  0.0,  1.0,  1.0,  1.0,  1.0,  0.0, -1.0,
     -1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0,
     -1.0,  0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -1.0,
     -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0
  };
 
  float tabla_turn_a[64] = {
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.5,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  0.5,
   -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
   -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
   -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
   -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
   -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
    0.0,  0.0,  0.0,  0.5,  0.5,  0.0,  0.0,  0.0
 };
 
 float tabla_dama_a[64] = {
     -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0,
     -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
     -1.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0,
     -0.5,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5,
      0.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5,
     -1.0,  0.5,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0,
     -1.0,  0.0,  0.5,  0.0,  0.0,  0.0,  0.0, -1.0,
     -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0
 };
 
 float tabla_rege_a[64] = {
     -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
     -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
     -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
     -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
     -2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0,
     -1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0,
      2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0,
      2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0
 };
 
 // Tabele poziționale pentru piese negre
 float tabla_pion_n[64];
 float tabla_cal_n[64];
 float tabla_nebun_n[64];
 float tabla_turn_n[64];
 float tabla_dama_n[64];
 float tabla_rege_n[64];
 
 void inverseaza_sir(float *sir) {
    int i, j;
    float aux;

    for (i = 0, j = 63; i < j; i++, j--) {
        aux = sir[i];
        sir[i] = sir[j];
        sir[j] = aux;
    }
}

void initializeaza_tabele_negre() {
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

float evaluatePosition(GameState *gs) {
    float scor = 0.0f;
    
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            char piece = gs->tabla[i][j];
            int pos = i*8 + j;
            
            switch(piece) {
                case 'P': 
                    scor += 1.0 + tabla_pion_a[pos]; break;
                case 'C': 
                    scor += 3.0 + tabla_cal_a[pos]; break;
                case 'N': 
                    scor += 3.0 + tabla_nebun_a[pos]; break;
                case 'T': 
                    scor += 5.0 + tabla_turn_a[pos]; break;
                case 'D': 
                    scor += 9.0 + tabla_dama_a[pos]; break;
                case 'R': 
                    scor += 100.0 + tabla_rege_a[pos]; break;
                case 'p': 
                    scor -= 1.0 + tabla_pion_n[63-pos]; break;
                case 'c': 
                    scor -= 3.0 + tabla_cal_n[63-pos]; break;
                case 'n': 
                    scor -= 3.0 + tabla_nebun_n[63-pos]; break;
                case 't': 
                    scor -= 5.0 + tabla_turn_n[63-pos]; break;
                case 'd': 
                    scor -= 9.0 + tabla_dama_n[63-pos]; break;
                case 'r': 
                    scor -= 100.0 + tabla_rege_n[63-pos]; break;
            }
        }
    }
    
    // Bonusuri/penalizări suplimentare
    if(gs->canWhiteCastleKingside) scor += 0.5;
    if(gs->canBlackCastleKingside) scor -= 0.5;
    
    return scor;
}

float minimax(GameState *gs, int depth, float alpha, float beta, int maximizingPlayer) {
    if (depth == 0 || isCheckmate(gs)) {
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

    if (lista.count == 0) {
        return evaluatePosition(gs);
    }

    if (maximizingPlayer) {
        float maxEval = -INF;
        for (int i = 0; i < lista.count; i++) {
            GameState copie = *gs;
            Move m = lista.mutari[i];
            executa_mutare(m.x1, m.y1, m.x2, m.y2, &copie);
            float eval = minimax(&copie, depth - 1, alpha, beta, 0);
            if (eval > maxEval) maxEval = eval;
            if (eval > alpha) alpha = eval;
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        float minEval = INF;
        for (int i = 0; i < lista.count; i++) { 
            GameState copie = *gs;
            Move m = lista.mutari[i];
            executa_mutare(m.x1, m.y1, m.x2, m.y2, &copie);
            float eval = minimax(&copie, depth - 1, alpha, beta, 1);
            if (eval < minEval) minEval = eval;
            if (eval < beta) beta = eval;
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

Move findBestMove(GameState *gs) {
    MoveList lista;
    initMoveList(&lista);

    int jucator = gs->currentPlayer;

    // Generează toate mutările pentru piesele jucătorului curent
    genereazaToateMutarilePion(gs, &lista, jucator);
    genereazaToateMutarileCal(gs, &lista, jucator);
    genereazaToateMutarileNebun(gs, &lista, jucator);
    genereazaToateMutarileTurn(gs, &lista, jucator);
    genereazaToateMutarileDama(gs, &lista, jucator);

    // Pentru rege
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            char piece = gs->tabla[x][y];
            if ((jucator == 0 && piece == 'R') || (jucator == 1 && piece == 'r')) {
                genereazaMutariRege(x, y, gs, &lista);
            }
        }

    Move bestMove = { -1, -1, -1, -1, INF }; //{x1, y1, x2, y2, scor}
    for (int i = 0; i < lista.count; i++) {
        GameState copie = *gs;
        Move m = lista.mutari[i];
        executa_mutare(m.x1, m.y1, m.x2, m.y2, &copie);
        float scor = minimax(&copie, DEPTH-1, -INF, INF, 1);
        if (scor < bestMove.scor) {
            bestMove = m;
            bestMove.scor = scor;
        }
    }
    return bestMove;
}

// Generare mutări: Pentru fiecare piesă a jucătorului curent, generează toate mutările legale
// Simulare mutări: Creează o copie a stării jocului și aplică mutarea
// Evaluare recursivă: Apelează minimax() pentru starea nouă cu adâncime redusă
// Alpha-Beta Pruning: Taie ramurile care nu pot influența rezultatul final
// Selectare mutare: Alege mutarea cu cel mai bun scor evaluat
// Optimizări importante:
// Move ordering: Sortează mutările înainte de evaluare (capturi primele)
// Tabele poziționale: Asignă bonusuri/penalizări în funcție de poziția pieselor
// Iterative deepening: Poate fi adăugat pentru căutare progresivă
// Cum funcționează evaluarea:
// Piese albe: +1 pentru pion, +3 pentru cal/nebun etc.
// Piese negre: -1 pentru pion, -3 pentru cal/nebun etc.
// Bonusuri poziționale din tabele
// Bonusuri pentru rocadă și alte factori strategice
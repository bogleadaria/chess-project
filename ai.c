#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>

#include "ai.h"
#include "move_validation.h"

// Tabele poziționale pentru alb
const int tabla_pion[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    5,  10,  10, -20, -20,  10,  10,   5,
    5,  -5, -10,   0,   0, -10,  -5,   5,
    0,   0,   0,  20,  20,   0,   0,   0,
    5,   5,  10,  25,  25,  10,   5,   5,
   10,  10,  20,  30,  30,  20,  10,  10,
   50,  50,  50,  50,  50,  50,  50,  50,
    0,   0,   0,   0,   0,   0,   0,   0
};

const int tabla_cal[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20,   0,   0,   0,   0, -20, -40,
    -30,   0,  10,  15,  15,  10,   0, -30,
    -30,   5,  15,  20,  20,  15,   5, -30,
    -30,   0,  15,  20,  20,  15,   0, -30,
    -30,   5,  10,  15,  15,  10,   5, -30,
    -40, -20,   0,   5,   5,   0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
 };

 
 const int tabla_nebun[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   5,   5,  10,  10,   5,   5, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,  10,  10,  10,  10,  10,  10, -10,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
 };
 
 const int tabla_turn[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    5,  10,  10,  10,  10,  10,  10,   5,
   -5,   0,   0,   0,   0,   0,   0,  -5,
   -5,   0,   0,   0,   0,   0,   0,  -5,
   -5,   0,   0,   0,   0,   0,   0,  -5,
   -5,   0,   0,   0,   0,   0,   0,  -5,
   -5,   0,   0,   0,   0,   0,   0,  -5,
    0,   0,   0,   5,   5,   0,   0,   0
};

const int tabla_dama[64] = {
    -20, -10, -10,  -5,  -5, -10, -10, -20,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,   5,   5,   5,   0, -10,
     -5,   0,   5,   5,   5,   5,   0,  -5,
      0,   0,   5,   5,   5,   5,   0,  -5,
    -10,   5,   5,   5,   5,   5,   0, -10,
    -10,   0,   5,   0,   0,   0,   0, -10,
    -20, -10, -10,  -5,  -5, -10, -10, -20
 };

 const int tabla_rege[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
     20,  20,   0,   0,   0,   0,  20,  20,
     20,  30,  10,   0,   0,  10,  30,  20
 };

 const int tabla_rege_final[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -30, -20, -10,   0,   0, -10, -20, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -30,   0,   0,   0,   0, -30, -30,
    -50, -30, -30, -30, -30, -30, -30, -50
 };

float evaluatePosition(GameState *gs) {
    float score = 0.0f;
    
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            char piece = gs->tabla[i][j];
            int pos = i*8 + j;
            
            switch(piece) {
                case 'P': score += 1.0 + tabla_pion[pos]; break;
                case 'N': score += 3.0 + tabla_cal[pos]; break;
                case 'B': score += 3.0 + tabla_nebun[pos]; break;
                case 'R': score += 5.0 + tabla_turn[pos]; break;
                case 'Q': score += 9.0 + tabla_dama[pos]; break;
                case 'K': score += 100.0 + tabla_rege[pos]; break;
                case 'p': score -= 1.0 + tabla_pion[63-pos]; break;
                case 'n': score -= 3.0 + tabla_cal[63-pos]; break;
                case 'b': score -= 3.0 + tabla_nebun[63-pos]; break;
                case 'r': score -= 5.0 + tabla_turn[63-pos]; break;
                case 'q': score -= 9.0 + tabla_dama[63-pos]; break;
                case 'k': score -= 100.0 + tabla_rege[63-pos]; break;
            }
        }
    }
    
    // Bonusuri/penalizări suplimentare
    if(gs->canWhiteCastleKingside) score += 0.5;
    if(gs->canBlackCastleKingside) score -= 0.5;
    
    return score;
}

float minimax(GameState *gs, int depth, float alpha, float beta, int maximizingPlayer) {
    if(depth == 0 || isCheckmate(gs))
        return evaluatePosition(gs);

    Move moves[256];
    int moveCount = 0;

    // Generare mutări
    for(int x1=0; x1<8; x1++) {
        for(int y1=0; y1<8; y1++) {
            if ((maximizingPlayer && islower(gs->tabla[x1][y1])) || 
    (!maximizingPlayer && isupper(gs->tabla[x1][y1])))
{
                for(int x2=0; x2<8; x2++) {
                    for(int y2=0; y2<8; y2++) {
                        if(validareMiscare(x1,y1,x2,y2,gs)) {
                            moves[moveCount++] = (Move){x1,y1,x2,y2,0};
                        }
                    }
                }
            }
        }
    }

    if(maximizingPlayer) {
        float maxEval = -INF;
        for(int i=0; i<moveCount; i++) {
            GameState newState = *gs;
            executa_mutare(moves[i].x1, moves[i].y1, moves[i].x2, moves[i].y2, &newState);
            float eval = minimax(&newState, depth-1, alpha, beta, 0);
            
            if(eval > maxEval) {
                maxEval = eval;
                if(depth == DEPTH) moves[i].score = eval;
            }
            
            alpha = fmax(alpha, eval);
            if(beta <= alpha)
                break;
        }
        return maxEval;
    } else {
        float minEval = INF;
        for(int i=0; i<moveCount; i++) {
            GameState newState = *gs;
            executa_mutare(moves[i].x1, moves[i].y1, moves[i].x2, moves[i].y2, &newState);
            float eval = minimax(&newState, depth-1, alpha, beta, 1);
            
            if(eval < minEval) {
                minEval = eval;
                if(depth == DEPTH) moves[i].score = eval;
            }
            
            beta = fmin(beta, eval);
            if(beta <= alpha)
                break;
        }
        return minEval;
    }
}

Move findBestMove(GameState *gs) {
    Move bestMove = {-1,-1,-1,-1,-INF};
    Move moves[256];
    int moveCount = 0;

    // Generare mutări pentru engine
    for(int x1=0; x1<8; x1++) {
        for(int y1=0; y1<8; y1++) {
            if(isupper(gs->tabla[x1][y1])) {
                for(int x2=0; x2<8; x2++) {
                    for(int y2=0; y2<8; y2++) {
                        if(validareMiscare(x1,y1,x2,y2,gs)) {
                            moves[moveCount++] = (Move){x1,y1,x2,y2,0};
                        }
                    }
                }
            }
        }
    }

    // Evaluare fiecare mutare
    for(int i=0; i<moveCount; i++) {
        GameState newState = *gs;
        executa_mutare(moves[i].x1, moves[i].y1, moves[i].x2, moves[i].y2, &newState);
        float currentEval = minimax(&newState, DEPTH-1, -INF, INF, 0);
        
        if(currentEval > bestMove.score) {
            bestMove = moves[i];
            bestMove.score = currentEval;
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
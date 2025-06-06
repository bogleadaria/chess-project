#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdio.h>

typedef struct
{
    char event[250];
    char site[250];
    char date[50];
    int round;
    char white[50];
    char black[50];
    char result[10];
    char history[120][6];
    int historyCount;
    int Rocada[120]; // 0 = nu, 1 = mică, 2 = mare
    int Captura[120]; // 0 = nu, 1 = pion, 2 = altceva
} PGN;

typedef struct
{
    char tabla[8][8];
    bool culoare_ai; // 0 = alb, 1 = negru
    int currentPlayer;
    int canWhiteCastleKingside;
    int canWhiteCastleQueenside;
    int canBlackCastleKingside;
    int canBlackCastleQueenside;
    int enPassantTarget[2];
    int halfmoveClock;
    int fullmoveNumber;
    bool isPGN;
    PGN pgn;
} GameState;

GameState initializeGame();
PGN initializarePGN();
void printTabla(char tabla[8][8], bool culoare_ai);
void transformareMiscare(char *start, char *finish, int *x1, int *y1, int *x2, int *y2);
void executa_mutare(int x1, int y1, int x2, int y2, GameState *gs);
int isInCheck(GameState *gs, int player);
int isCheckmate(GameState *gs);
int isSquareAttacked(GameState *gs, int x, int y, int attackerPlayer);
int validareMiscare(int x1, int y1, int x2, int y2, GameState *gs);
int validareRocada(int x1, int y1, int x2, int y2, GameState *gs);
void salveazaJocFEN(const GameState *gs, const char *filename);
void salveazaJocPGN(const GameState *gs, const char *filename);
void inchideJoc();
int reincepereJoc(GameState *gs, const char *filename, PGN *pgn, bool type);
void scrieIstoricMutari(FILE *file, const GameState *gs, int i);
void afiseazaHeaderPGN(PGN *pgn);


#endif
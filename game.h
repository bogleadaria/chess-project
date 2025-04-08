#ifndef GAME_H
#define GAME_H

typedef struct {
    char tabla[8][8];
    int currentPlayer;
    int canWhiteCastleKingside;
    int canWhiteCastleQueenside;
    int canBlackCastleKingside;
    int canBlackCastleQueenside;
    int enPassantTarget[2];
    int halfmoveClock;
    int fullmoveNumber;
} GameState;

GameState initializeGame();
void printTabla(char tabla[8][8]);
void transformareMiscare(char *start, char *finish, int *x1, int *y1, int *x2, int *y2);
void executa_mutare(int x1, int y1, int x2, int y2, GameState *gs);
int isInCheck(GameState *gs, int player);
int isCheckmate(GameState *gs);
int isSquareAttacked(GameState *gs, int x, int y, int attackerPlayer);
int validareMiscare(int x1, int y1, int x2, int y2, GameState *gs);
int validareRocada(int x1, int y1, int x2, int y2, GameState *gs);
void salveazaJoc(const GameState *gs, const char *filename);
void inchideJoc();
int reincepereJoc(GameState *gs, const char *filename);

#endif // GAME_H

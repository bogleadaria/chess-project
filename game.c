#include "game.h"
#include <stdio.h>

GameState initializeGame() {
    GameState gs = {
        .tabla = {
            {'t', 'c', 'n', 'd', 'r', 'n', 'c', 't'},
            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
            {'T', 'C', 'N', 'D', 'R', 'N', 'C', 'T'}
        },
        .currentPlayer = 0,
        .canWhiteCastleKingside = 1,
        .canWhiteCastleQueenside = 1,
        .canBlackCastleKingside = 1,
        .canBlackCastleQueenside = 1,
        .enPassantTarget = {-1, -1},
        .halfmoveClock = 0,
        .fullmoveNumber = 1
    };
    return gs;
}

void printTabla(char tabla[8][8]) {

    short color_codes[] = {7, 40};

    printf("  a b c d e f g h\n");
    printf(" -----------------\n");
    for (int i = 0; i < 8; i++) {
        printf("%d|", 8 - i);
        for (int j = 0; j < 8; j++) {
            printf("\033[1;%dm %c \033[0m", color_codes[(i+j)%2], tabla[i][j]); 
        }
        printf("|%d\n", 8 - i);
    }
    printf(" -----------------\n");
    printf("  a b c d e f g h\n");
}

int isInCheck(GameState *gs, int player) {
    int king_x, king_y;
    char king = (player == 0) ? 'R' : 'r';
    
    // Găsește regele
    for(int i=0; i<8; i++)
        for(int j=0; j<8; j++)
            if(gs->tabla[i][j] == king) {
                king_x = i;
                king_y = j;
            }
    
    // Verifică atacurile
    return isSquareAttacked(gs, king_x, king_y, !player);
}

int isCheckmate(GameState *gs) {
    if(!isInCheck(gs, gs->currentPlayer)) return 0;
    
    // Generează toate mutările posibile
    for(int x1=0; x1<8; x1++)
        for(int y1=0; y1<8; y1++)
            if((gs->currentPlayer == 0 && isupper(gs->tabla[x1][y1])) ||
               (gs->currentPlayer == 1 && islower(gs->tabla[x1][y1])))
                for(int x2=0; x2<8; x2++)
                    for(int y2=0; y2<8; y2++)
                        if(validareMiscare(x1,y1,x2,y2,gs))
                            return 0;
    return 1;
}
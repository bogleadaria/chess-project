#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "game.h"
#include "move_validation.h"

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
    short color_codes[] = {47, 40}; // 47 = alb, 40 = negru (fundal)
    printf("\n   a  b  c  d  e  f  g  h\n");
    for (int i = 0; i < 8; i++) {
        printf("%d ", 8 - i); // Numerotarea liniilor
        for (int j = 0; j < 8; j++) {
            printf("\033[1;%dm %c \033[0m", color_codes[(i + j) % 2], tabla[i][j]);
        }
        printf(" %d\n", 8 - i); // Numerotarea liniilor
    }
    printf("   a  b  c  d  e  f  g  h\n\n");
}

int isSquareAttacked(GameState *gs, int x, int y, int attackerPlayer) {
    // Verifică atacurile pionilor
    int pawnDir = (attackerPlayer == 0) ? -1 : 1;
    if ((x - pawnDir >= 0 && x - pawnDir < 8 && y - 1 >= 0 && gs->tabla[x - pawnDir][y - 1] == (attackerPlayer ? 'p' : 'P')) ||
        (x - pawnDir >= 0 && x - pawnDir < 8 && y + 1 < 8 && gs->tabla[x - pawnDir][y + 1] == (attackerPlayer ? 'p' : 'P'))) {
        return 1;
    }

    // Verifică atacurile calului
    char knight = attackerPlayer ? 'c' : 'C';
    int knightMoves[8][2] = {{2,1}, {1,2}, {-1,2}, {-2,1}, {-2,-1}, {-1,-2}, {1,-2}, {2,-1}};
    for (int i = 0; i < 8; i++) {
        int nx = x + knightMoves[i][0];
        int ny = y + knightMoves[i][1];
        if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && gs->tabla[nx][ny] == knight) {
            return 1;
        }
    }

    // Verifică atacurile nebunului și atacurile diagonale ale damei
    char bishop = attackerPlayer ? 'n' : 'N';
    char queen = attackerPlayer ? 'd' : 'D';
    int bishopDirs[4][2] = {{1,1}, {1,-1}, {-1,1}, {-1,-1}};
    for (int i = 0; i < 4; i++) {
        int dx = bishopDirs[i][0];
        int dy = bishopDirs[i][1];
        int cx = x + dx;
        int cy = y + dy;
        
        while (cx >= 0 && cx < 8 && cy >= 0 && cy < 8) {
            char piece = gs->tabla[cx][cy];
            if (piece != ' ') {
                if (piece == bishop || piece == queen) {
                    return 1;
                }
                break; // Oprim căutarea în această direcție
            }
            cx += dx;
            cy += dy;
        }
    }

    // Verifică atacurile turnului și atacurile orizontale/verticale ale damei
    char rook = attackerPlayer ? 't' : 'T';
    int rookDirs[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
    for (int i = 0; i < 4; i++) {
        int dx = rookDirs[i][0];
        int dy = rookDirs[i][1];
        int cx = x + dx;
        int cy = y + dy;
        
        while (cx >= 0 && cx < 8 && cy >= 0 && cy < 8) {
            char piece = gs->tabla[cx][cy];
            if (piece != ' ') {
                if (piece == rook || piece == queen) {
                    return 1;
                }
                break; // Oprim căutarea în această direcție
            }
            cx += dx;
            cy += dy;
        }
    }

    // Verifică atacurile regelui
    char king = attackerPlayer ? 'r' : 'R';
    int kingMoves[8][2] = {{1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}};
    for (int i = 0; i < 8; i++) {
        int kx = x + kingMoves[i][0];
        int ky = y + kingMoves[i][1];
        if (kx >= 0 && kx < 8 && ky >= 0 && ky < 8 && gs->tabla[kx][ky] == king) {
            return 1;
        }
    }

    return 0;
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

void salveazaJoc(const GameState *gs, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Eroare la deschiderea fișierului pentru salvare!\n");
        return;
    }

    // Scrie tabla de șah
    fprintf(file, "Tabla de șah:\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            fprintf(file, "%c ", gs->tabla[i][j]);
        }
        fprintf(file, "\n");
    }

    // Scrie informații despre joc
    fprintf(file, "\nJucător curent: %s\n", gs->currentPlayer ? "Negru" : "Alb");
    // fprintf(file, "Drepturi rocadă: Alb - %d%d, Negru - %d%d\n",
    //         gs->canWhiteCastleKingside, gs->canWhiteCastleQueenside,
    //         gs->canBlackCastleKingside, gs->canBlackCastleQueenside);
    // fprintf(file, "Ținta en passant: %d %d\n", gs->enPassantTarget[0], gs->enPassantTarget[1]);
    // fprintf(file, "Mutări până la remiză: %d\n", 50 - gs->halfmoveClock);
    // fprintf(file, "Numărul mutării: %d\n", gs->fullmoveNumber);

    fclose(file);
    printf("Jocul a fost salvat în fișierul '%s'.\n", filename);
}

void inchideJoc() {
    printf("\n");
    printf("Mulțumim pentru joc!\n");
    printf("Programul se va încheia\n");
    exit(0); // Termină programul cu codul de succes 0
}

int reincepereJoc(GameState *gs, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Eroare: Fișierul '%s' nu există!\n", filename);
        return 0;
    }

    char buffer[256];

    // Ignoră linia "Tabla de șah:"
    fgets(buffer, sizeof(buffer), file);

    // Citește tabla linie cu linie
    for (int i = 0; i < 8; i++) {
        fgets(buffer, sizeof(buffer), file);
        for (int j = 0; j < 8; j++) {
            gs->tabla[i][j] = buffer[j * 2]; // Fiecare caracter este separat de un spațiu
        }
    }

    // Citește metadatele jocului
    fgets(buffer, sizeof(buffer), file); // Ignoră linia goală
    fscanf(file, "Jucător curent: %s\n", buffer);
    gs->currentPlayer = (strcmp(buffer, "Negru") == 0) ? 1 : 0;

    fclose(file);
    printf("Joc încărcat cu succes din '%s'!\n", filename);
    return 1;
}
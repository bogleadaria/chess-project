#include <stdlib.h>

#include "game.h"
#include "mutari.h"



void initMoveList(MoveList *lista) {
    lista->count = 0;
}

void genereazaMutariTurn(int x, int y, GameState *gs, MoveList *lista) {
    char piece = gs->tabla[x][y];
    if (piece != 'T' && piece != 't')
        return;

    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};
    int isAlb = (piece == 'T');

    for (int dir = 0; dir < 4; dir++) {
        int nx = x + dx[dir], ny = y + dy[dir];
        while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
            char dest = gs->tabla[nx][ny];
            if (dest == ' ') {
                lista->mutari[lista->count++] = (Move){x, y, nx, ny};
            } else {
                // Captură doar dacă e piesă adversă
                if ((isAlb && dest >= 'a' && dest <= 'z') ||
                    (!isAlb && dest >= 'A' && dest <= 'Z')) {
                    lista->mutari[lista->count++] = (Move){x, y, nx, ny};
                }
                break; // Blocaj la orice piesă
            }
            nx += dx[dir];
            ny += dy[dir];
        }
    }
}

void genereazaMutariRege(int x, int y, GameState *gs, MoveList *lista) {
    static const int king_offsets[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
    char piece = gs->tabla[x][y];
    if (piece != 'R' && piece != 'r') return;

    int isAlb = (piece == 'R');
    for (int i = 0; i < 8; i++) {
        int nx = x + king_offsets[i][0];
        int ny = y + king_offsets[i][1];
        if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
            char dest = gs->tabla[nx][ny];
            if ((isAlb && (dest == ' ' || (dest >= 'a' && dest <= 'z'))) ||
                (!isAlb && (dest == ' ' || (dest >= 'A' && dest <= 'Z')))) {
                lista->mutari[lista->count++] = (Move){x, y, nx, ny};
            }
        }
    }
}

void genereazaMutariPion(int x, int y, GameState *gs, MoveList *lista) {
    char piece = gs->tabla[x][y];
    int isAlb = (piece == 'P');
    int dir = isAlb ? -1 : 1; // alb merge în sus (x scade), negru în jos (x crește)

    int nx = x + dir;
    if (nx >= 0 && nx < 8) {
        // Mutare simplă înainte
        if (gs->tabla[nx][y] == ' ') {
            lista->mutari[lista->count++] = (Move){x, y, nx, y};

            // Mutare dublă la prima mutare
            if ((isAlb && x == 6) || (!isAlb && x == 1)) {
                int nnx = x + 2 * dir;
                if (gs->tabla[nnx][y] == ' ')
                    lista->mutari[lista->count++] = (Move){x, y, nnx, y};
            }
        }

        // Captură stânga
        if (y > 0 && ((isAlb && gs->tabla[nx][y - 1] >= 'a' && gs->tabla[nx][y - 1] <= 'z') ||
                      (!isAlb && gs->tabla[nx][y - 1] >= 'A' && gs->tabla[nx][y - 1] <= 'Z'))) {
            lista->mutari[lista->count++] = (Move){x, y, nx, y - 1};
        }

        // Captură dreapta
        if (y < 7 && ((isAlb && gs->tabla[nx][y + 1] >= 'a' && gs->tabla[nx][y + 1] <= 'z') ||
                      (!isAlb && gs->tabla[nx][y + 1] >= 'A' && gs->tabla[nx][y + 1] <= 'Z'))) {
            lista->mutari[lista->count++] = (Move){x, y, nx, y + 1};
        }

        // En passant
        if (gs->enPassantTarget[0] == nx) {
            if (gs->enPassantTarget[1] == y - 1 && y > 0)
                lista->mutari[lista->count++] = (Move){x, y, nx, y - 1};
            if (gs->enPassantTarget[1] == y + 1 && y < 7)
                lista->mutari[lista->count++] = (Move){x, y, nx, y + 1};
        }
    }
}

void genereazaToateMutarilePion(GameState *gs, MoveList *lista, int player) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
            if ((player == 0 && gs->tabla[x][y] == 'P') ||
                (player == 1 && gs->tabla[x][y] == 'p')) {
                genereazaMutariPion(x, y, gs, lista);
            }
}

void genereazaMutariNebun(int x, int y, GameState *gs, MoveList *lista) {
    char piece = gs->tabla[x][y];
    if (piece != 'N' && piece != 'n')
        return;

    int isAlb = (piece == 'N');
    // Direcțiile pentru nebun: (dx, dy)
    int dx[] = {1, 1, -1, -1};
    int dy[] = {1, -1, 1, -1};

    for (int dir = 0; dir < 4; dir++) {
        int nx = x + dx[dir], ny = y + dy[dir];
        while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
            char dest = gs->tabla[nx][ny];
            if (dest == ' ') {
                lista->mutari[lista->count++] = (Move){x, y, nx, ny};
            } else {
                // Captură doar dacă e piesă adversă
                if ((isAlb && dest >= 'a' && dest <= 'z') ||
                    (!isAlb && dest >= 'A' && dest <= 'Z')) {
                    lista->mutari[lista->count++] = (Move){x, y, nx, ny};
                }
                break; // Blocaj la orice piesă
            }
            nx += dx[dir];
            ny += dy[dir];
        }
    }
}

void genereazaToateMutarileNebun(GameState *gs, MoveList *lista, int player) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if ((player == 0 && gs->tabla[x][y] == 'N') ||
                (player == 1 && gs->tabla[x][y] == 'n')) {
                genereazaMutariNebun(x, y, gs, lista);
            }
        }
    }
}

void genereazaMutariDama(int x, int y, GameState *gs, MoveList *lista) {
    char piece = gs->tabla[x][y];
    if (piece != 'D' && piece != 'd')
        return;

    int isAlb = (piece == 'D');
    // Direcțiile pentru damă: 8 direcții (turn + nebun)
    int dx[] = {1, -1, 0, 0, 1, 1, -1, -1};
    int dy[] = {0, 0, 1, -1, 1, -1, 1, -1};

    for (int dir = 0; dir < 8; dir++) {
        int nx = x + dx[dir], ny = y + dy[dir];
        while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
            char dest = gs->tabla[nx][ny];
            if (dest == ' ') {
                lista->mutari[lista->count++] = (Move){x, y, nx, ny};
            } else {
                // Captură doar dacă e piesă adversă
                if ((isAlb && dest >= 'a' && dest <= 'z') ||
                    (!isAlb && dest >= 'A' && dest <= 'Z')) {
                    lista->mutari[lista->count++] = (Move){x, y, nx, ny};
                }
                break; // Blocaj la orice piesă
            }
            nx += dx[dir];
            ny += dy[dir];
        }
    }
}

void genereazaToateMutarileDama(GameState *gs, MoveList *lista, int player) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
            if ((player == 0 && gs->tabla[x][y] == 'D') ||
                (player == 1 && gs->tabla[x][y] == 'd')) {
                genereazaMutariDama(x, y, gs, lista);
            }
}

void genereazaMutariCal(int x, int y, GameState *gs, MoveList *lista) {
    char piece = gs->tabla[x][y];
    if (piece != 'C' && piece != 'c') return;

    int isAlb = (piece == 'C');
    static const int knight_offsets[8][2] = {
        {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
        {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
    };

    for (int i = 0; i < 8; i++) {
        int nx = x + knight_offsets[i][0];
        int ny = y + knight_offsets[i][1];
        if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
            char dest = gs->tabla[nx][ny];
            if ((isAlb && (dest == ' ' || (dest >= 'a' && dest <= 'z'))) ||
                (!isAlb && (dest == ' ' || (dest >= 'A' && dest <= 'Z')))) {
                lista->mutari[lista->count++] = (Move){x, y, nx, ny};
            }
        }
    }
}

void genereazaToateMutarileCal(GameState *gs, MoveList *lista, int player) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
            if ((player == 0 && gs->tabla[x][y] == 'C') ||
                (player == 1 && gs->tabla[x][y] == 'c')) {
                genereazaMutariCal(x, y, gs, lista);
            }
}
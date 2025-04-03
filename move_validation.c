#include "move_validation.h"
#include "pieces.h"

void transformareMiscare(char *start, char *finish, int *x1, int *y1, int *x2, int *y2) {
    *y1 = tolower(start[0]) - 'a';
    *x1 = 8 - (start[1] - '0');
    *y2 = tolower(finish[0] - 'a');
    *x2 = 8 - (finish[1] - '0');
}

int validareMiscare(int x1, int y1, int x2, int y2, GameState *gs) {
    char piece = gs->tabla[x1][y1];
    switch (toupper(piece)) {
        case 'P': return Pion(x1, y1, x2, y2, gs);
        case 'C': return Cal(x1, y1, x2, y2, gs);
        case 'N': return Nebun(x1, y1, x2, y2, gs);
        case 'T': return Turn(x1, y1, x2, y2, gs);
        case 'D': return Dama(x1, y1, x2, y2, gs);
        case 'R': return Rege(x1, y1, x2, y2, gs);
        default: return 0;
    }
}

int validareRocada(int x1, int y1, int x2, int y2, GameState *gs) {
    int kingside = (y2 > y1);
    int rook_y = kingside ? 7 : 0;
    
    // Verifică drepturile de rocadă
    if ((gs->currentPlayer == 0 && !(kingside ? gs->canWhiteCastleKingside : gs->canWhiteCastleQueenside)) ||
        (gs->currentPlayer == 1 && !(kingside ? gs->canBlackCastleKingside : gs->canBlackCastleQueenside))) 
        return 0;

    // Verifică spațiile libere și atacate
    for(int y = y1 + (kingside ? 1 : -1); y != rook_y; y += (kingside ? 1 : -1)) {
        if(gs->tabla[x1][y] != ' ' || isSquareAttacked(gs, x1, y, !gs->currentPlayer)) 
            return 0;
    }
    
    // Actualizează pozițiile
    gs->tabla[x1][kingside ? y1+2 : y1-2] = gs->tabla[x1][y1];
    gs->tabla[x1][kingside ? y1+1 : y1-1] = gs->tabla[x1][rook_y];
    gs->tabla[x1][y1] = ' ';
    gs->tabla[x1][rook_y] = ' ';
    
    return 1;
}
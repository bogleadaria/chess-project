#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "move_validation.h"
#include "pieces.h"

void transformareMiscare(char *start, char *finish, int *x1, int *y1, int *x2, int *y2)
{
    *y1 = tolower(start[0]) - 'a';
    *x1 = 8 - (start[1] - '0');
    *y2 = tolower(finish[0]) - 'a';
    *x2 = 8 - (finish[1] - '0');
}

int validareMiscare(int x1, int y1, int x2, int y2, GameState *gs)
{
    char piece = gs->tabla[x1][y1];

    // Verifică dacă piesa aparține jucătorului curent
    if (piece == ' ' ||
        (gs->currentPlayer == 0 && islower(piece)) ||
        (gs->currentPlayer == 1 && isupper(piece))) {
        //printf("Jucător gresit!\n");
        return 0;
    }    

    switch (toupper(piece))
    {
    case 'P':
        return Pion(x1, y1, x2, y2, gs);
    case 'C':
        return Cal(x1, y1, x2, y2, gs);
    case 'N':
        return Nebun(x1, y1, x2, y2, gs);
    case 'T':
        return Turn(x1, y1, x2, y2, gs);
    case 'D':
        return Dama(x1, y1, x2, y2, gs);
    case 'R':
        return Rege(x1, y1, x2, y2, gs);
    default:
        return 0;
    }
}

int validareRocada(int x1, int y1, int x2, int y2, GameState *gs)
{
    int kingside = (y2 > y1);
    int rook_y = kingside ? 7 : 0;

    // Verifică drepturile de rocadă
    if ((gs->currentPlayer == 0 && !(kingside ? gs->canWhiteCastleKingside : gs->canWhiteCastleQueenside)) ||
        (gs->currentPlayer == 1 && !(kingside ? gs->canBlackCastleKingside : gs->canBlackCastleQueenside)))
        return 0;

    // Verifică spațiile libere și atacate
    for (int y = y1 + (kingside ? 1 : -1); y != rook_y; y += (kingside ? 1 : -1))
    {
        if (gs->tabla[x1][y] != ' ' || isSquareAttacked(gs, x1, y, !gs->currentPlayer))
            return 0;
    }

    // Actualizează pozițiile
    gs->tabla[x1][kingside ? y1 + 2 : y1 - 2] = gs->tabla[x1][y1];
    gs->tabla[x1][kingside ? y1 + 1 : y1 - 1] = gs->tabla[x1][rook_y];
    gs->tabla[x1][y1] = ' ';
    gs->tabla[x1][rook_y] = ' ';

    return 1;
}

void executa_mutare(int x1, int y1, int x2, int y2, GameState *gs)
{
    // Memorare informații pentru en passant și rocadă
    char piece = gs->tabla[x1][y1];
    int isPawnMove = (toupper(piece) == 'P');
    int isKingMove = (toupper(piece) == 'R');
    int isRookMove = (toupper(piece) == 'T');
    int isCapture = (gs->tabla[x2][y2] != ' ');

    // Actualizează en passant target
    if (isPawnMove && abs(x2 - x1) == 2)
    {
        gs->enPassantTarget[0] = (x1 + x2) / 2;
        gs->enPassantTarget[1] = y1;
    }
    else
    {
        gs->enPassantTarget[0] = -1;
        gs->enPassantTarget[1] = -1;
    }

    // Gestionează captura en passant
    if (isPawnMove && y2 != y1 && gs->tabla[x2][y2] == ' ')
    {
        gs->tabla[x1][y2] = ' '; // Șterge pionul capturat
    }

    // Actualizează drepturile de rocadă
    if (isKingMove)
    {
        if (piece == 'R')
        {
            gs->canWhiteCastleKingside = 0;
            gs->canWhiteCastleQueenside = 0;
        }
        else
        {
            gs->canBlackCastleKingside = 0;
            gs->canBlackCastleQueenside = 0;
        }
    }
    else if (isRookMove)
    {
        if (piece == 'T')
        {
            if (y1 == 0)
                gs->canWhiteCastleQueenside = 0;
            if (y1 == 7)
                gs->canWhiteCastleKingside = 0;
        }
        else
        {
            if (y1 == 0)
                gs->canBlackCastleQueenside = 0;
            if (y1 == 7)
                gs->canBlackCastleKingside = 0;
        }
    }

    // Gestionează rocada
    if (isKingMove && abs(y2 - y1) == 2)
    {
        int rook_y = (y2 > y1) ? 7 : 0;
        int new_rook_y = (y2 > y1) ? y2 - 1 : y2 + 1;
        gs->tabla[x1][new_rook_y] = gs->tabla[x1][rook_y];
        gs->tabla[x1][rook_y] = ' ';
    }

    // Actualizează contoarele de mutări
    if (isPawnMove || isCapture)
    {
        gs->halfmoveClock = 0;
    }
    else
    {
        gs->halfmoveClock++;
    }

    if (gs->currentPlayer == 1)
    {
        gs->fullmoveNumber++;
    }

    // Mută piesa și șterge poziția inițială
    gs->tabla[x2][y2] = piece;
    gs->tabla[x1][y1] = ' ';

    // Gestionează promovarea pionului
    if (isPawnMove && ((x2 == 0 && piece == 'P') || (x2 == 7 && piece == 'p')))
    {
        char promo;
        printf("Promovare pion! Alege (D, C, T, N): ");
        scanf(" %c", &promo);
        gs->tabla[x2][y2] = (piece == 'P') ? toupper(promo) : tolower(promo);
    }
}
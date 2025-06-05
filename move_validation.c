#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "move_validation.h"
#include "pieces.h"
#include "game.h"

int endsWith(const char *fisier, const char *sufix)
{
    if (!fisier || !sufix)
        return 0;
    size_t lenfisier = strlen(fisier);
    size_t lensufix = strlen(sufix);
    if (lensufix > lenfisier)
        return 0;
    return strcmp(fisier + lenfisier - lensufix, sufix) == 0;
}

void transformareMiscare(char *start, char *finish, int *x1, int *y1, int *x2, int *y2, bool culoare_ai)
{
    if (culoare_ai == 1)
    {
        *y1 = tolower(start[0]) - 'a';
        *x1 = 8 - (start[1] - '0');
        *y2 = tolower(finish[0]) - 'a';
        *x2 = 8 - (finish[1] - '0');
    }
    else
    {
        *y1 = tolower(start[0]) - 'a';
        *x1 = 8 - (start[1] - '0');
        *y2 = tolower(finish[0]) - 'a';
        *x2 = 8 - (finish[1] - '0');
    }
}

void transformareInversa(int *x1, int *y1, int *x2, int *y2, int *x1_t, int *y1_t, int *x2_t, int *y2_t, bool culoare_ai)
{
    if (culoare_ai == 1)
    {
        *y1_t = tolower(*y1 + 'a');
        *x1_t = 8 - *x1;
        *y2_t = tolower(*y2 - 'a');
        *x2 = 8 - *x2;
    }
    else
    {
        *y1_t = tolower(*y1 + 'a');
        *x1_t = *x1 + 1;
        *y2_t = tolower(*y2 + 'a');
        *x2_t = *x2 + 1;
    }
}

int validareMiscare(int x1, int y1, int x2, int y2, GameState *gs)
{
    char piece = gs->tabla[x1][y1];
    char dest = gs->tabla[x2][y2];

    // Prevent capturing the king
    if (dest == 'R' || dest == 'r')
        return 0;

    // Verifică dacă piesa aparține jucătorului curent
    if (piece == ' ' ||
        (gs->currentPlayer == 0 && islower(piece)) ||
        (gs->currentPlayer == 1 && isupper(piece)))
    {
        return -1;
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
    int regeside = (y2 > y1);
    int turn_y = regeside ? 7 : 0;

    // Verifică drepturile de rocadă
    if ((gs->currentPlayer == 0 && !(regeside ? gs->canWhiteCastleKingside : gs->canWhiteCastleQueenside)) ||
        (gs->currentPlayer == 1 && !(regeside ? gs->canBlackCastleKingside : gs->canBlackCastleQueenside)))
        return 0;

    // Verifică spațiile libere și atacate
    for (int y = y1 + (regeside ? 1 : -1); y != turn_y; y += (regeside ? 1 : -1))
    {
        if (gs->tabla[x1][y] != ' ' || isSquareAttacked(gs, x1, y, !gs->currentPlayer))
            return 0;
    }

    return 1;
}

// Verifică dacă există vreo mutare legală pentru jucătorul curent (pentru pat/remiză)
int existaMutareLegala(GameState *gs)
{
    for (int x1 = 0; x1 < 8; x1++)
        for (int y1 = 0; y1 < 8; y1++)
            if ((gs->currentPlayer == 0 && isupper(gs->tabla[x1][y1])) ||
                (gs->currentPlayer == 1 && islower(gs->tabla[x1][y1])))
            {
                for (int x2 = 0; x2 < 8; x2++)
                    for (int y2 = 0; y2 < 8; y2++)
                    {
                        if (validareMiscare(x1, y1, x2, y2, gs) == 1)
                            return 1;
                    }
            }
    return 0;
}

void executa_mutare(int x1, int y1, int x2, int y2, GameState *gs)
{
    // Memorare informații pentru en passant și rocadă
    char piece = gs->tabla[x1][y1];
    int isPionMove = (toupper(piece) == 'P');
    int isRegeMove = (toupper(piece) == 'R');
    int isTurnMove = (toupper(piece) == 'T');
    int isCapture = (gs->tabla[x2][y2] != ' ');

    if (isCapture)
    {
        gs->Captura = 1; // Setează captura
    }
    else
    {
        gs->Captura = 0; // Nu este captura
    }

    // Actualizează en passant target
    if (isPionMove && abs(x2 - x1) == 2)
    {
        gs->enPassantTarget[0] = (x1 + x2) / 2;
        gs->enPassantTarget[1] = y2;
    }
    else
    {
        gs->enPassantTarget[0] = -1;
        gs->enPassantTarget[1] = -1;
    }

    // Gestionează captura en passant
    if (isPionMove && y2 != y1 && gs->tabla[x2][y2] == ' ')
    {
        gs->tabla[x1][y2] = ' '; // Șterge pionul capturat
    }

    // Actualizează drepturile de rocadă
    if (isRegeMove)
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
    else if (isTurnMove)
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
    if (isRegeMove && abs(y2 - y1) == 2)
    {
        int turn_y = (y2 > y1) ? 7 : 0;
        if (y2 > y1)
        {
            turn_y = 7; // Rocada mică (dreapta)
            gs->Rocada = 1;
        }
        else
        {
            turn_y = 0; // Rocada mare (stânga)
            gs->Rocada = 2;
        }
        int new_turn_y = (y2 > y1) ? y2 - 1 : y2 + 1;

        gs->tabla[x1][y2] = piece; // Mută regele
        gs->tabla[x1][y1] = ' ';   // Șterge poziția inițială a regelui

        gs->tabla[x1][new_turn_y] = gs->tabla[x1][turn_y];
        gs->tabla[x1][turn_y] = ' ';
    }
    else
    {
        gs->Rocada = 0;
    }

    // Actualizează contoarele de mutări
    if (isPionMove || isCapture)
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
    if (isPionMove && (x2 == 0 && piece == 'P'))
    {
        if (gs->currentPlayer == gs->culoare_ai)
        {
            gs->tabla[x2][y2] = 'D'; // AI promovează automat la damă
        }
        else
        {
            char promo;
            printf("Promovare pion! Alege (D, C, T, N): ");
            scanf(" %c", &promo);
            gs->tabla[x2][y2] = toupper(promo);
        }
    }
    else if (isPionMove && (x2 == 7 && piece == 'p'))
    {
        if (gs->currentPlayer == gs->culoare_ai)
        {
            gs->tabla[x2][y2] = 'd'; // AI promovează automat la damă
        }
        else
        {
            char promo;
            printf("Promovare pion! Alege (d, c, t, n): ");
            scanf(" %c", &promo);
            gs->tabla[x2][y2] = tolower(promo);
        }
    }
}

void salveazaIstoricMutari(GameState *gs, int x1, int y1, int x2, int y2)
{
    int x1_t, y1_t, x2_t, y2_t;
    transformareInversa(&x1, &y1, &x2, &y2, &x1_t, &y1_t, &x2_t, &y2_t, gs->culoare_ai);
    switch (toupper(gs->tabla[x1][y1]))
    {
    case 'P':
        gs->pgn.history[gs->pgn.historyCount][0] = '\0';
        break;
    case 'C':
        gs->pgn.history[gs->pgn.historyCount][0] = 'N';
        break;
    case 'N':
        gs->pgn.history[gs->pgn.historyCount][0] = 'B';
        break;
    case 'T':
        gs->pgn.history[gs->pgn.historyCount][0] = 'R';
        break;
    case 'D':
        gs->pgn.history[gs->pgn.historyCount][0] = 'Q';
        break;
    case 'R':
        gs->pgn.history[gs->pgn.historyCount][0] = 'K';
        break;

    default:
        break;
    }
    gs->pgn.history[gs->pgn.historyCount][1] = y1_t;
    gs->pgn.history[gs->pgn.historyCount][2] = x1_t;
    gs->pgn.history[gs->pgn.historyCount][3] = y2_t;
    gs->pgn.history[gs->pgn.historyCount][4] = x2_t;
    gs->pgn.history[gs->pgn.historyCount][5] = '\0'; // Asigură terminarea șirului
    gs->pgn.historyCount++;
}

int mutareIeseDinSah(int x1, int y1, int x2, int y2, GameState *gs)
{
    // Salvează starea inițială a tablei
    char initialPiece = gs->tabla[x1][y1];
    char targetPiece = gs->tabla[x2][y2];

    // Execută mutarea
    gs->tabla[x2][y2] = initialPiece;
    gs->tabla[x1][y1] = ' ';

    // Verifică dacă regele este în șah după mutare
    int isCheck = isInCheck(gs, gs->currentPlayer);

    // Revin la starea inițială
    gs->tabla[x1][y1] = initialPiece;
    gs->tabla[x2][y2] = targetPiece;

    return isCheck;
}

int esteRemiza(GameState *gs)
{
    if (!isInCheck(gs, gs->currentPlayer) && !existaMutareLegala(gs))
        return 1;

    if (gs->halfmoveClock >= 50)
        return 1;

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "game.h"
#include "move_validation.h"
#include "ai.h"

GameState initializeGame()
{
    GameState gs = {
        .tabla = {
            {'t', 'c', 'n', 'd', 'r', 'n', 'c', 't'},
            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
            {'T', 'C', 'N', 'D', 'R', 'N', 'C', 'T'}},
        .currentPlayer = 0,
        .canWhiteCastleKingside = 1,
        .canWhiteCastleQueenside = 1,
        .canBlackCastleKingside = 1,
        .canBlackCastleQueenside = 1,
        .enPassantTarget = {-1, -1},
        .halfmoveClock = 0,
        .fullmoveNumber = 1,
        .isPGN = 0,
        .pgn = initializarePGN()};
    return gs;
}

PGN initializarePGN()
{
    PGN pgn = {
        .event = "",
        .site = "",
        .date = "",
        .round = 0,
        .white = "",
        .black = "",
        .result = "*",
        .historyCount = 0}; 
    return pgn;
}

void golesteFisierMiscari()
{
    FILE *f = fopen("mutari.txt", "w");
    if (f != NULL)
    {
        fclose(f); // Fișierul este acum gol
    }
    else
    {
        printf("Eroare: nu am putut deschide miscari.txt pentru golire.\n");
    }
}

void printTabla(char tabla[8][8], bool culoare_ai)
{
    if (culoare_ai == 0)
    {
        short color_codes[] = {47, 40}; // 47 = alb, 40 = negru (fundal)
        printf("\n   a  b  c  d  e  f  g  h\n");
        for (int i = 0; i < 8; i++)
        {
            printf("%d ", i + 1); // Numerotarea liniilor
            for (int j = 0; j < 8; j++)
            {
                printf("\033[1;%dm %c \033[0m", color_codes[(i + j) % 2], tabla[7 - i][j]);
            }
            printf(" %d\n", i + 1); // Numerotarea liniilor
        }
        printf("   a  b  c  d  e  f  g  h\n\n");
    }
    else
    {
        short color_codes[] = {47, 40}; // 47 = alb, 40 = negru (fundal)
        printf("\n   a  b  c  d  e  f  g  h\n");
        for (int i = 0; i < 8; i++)
        {
            printf("%d ", 8 - i); // Numerotarea liniilor
            for (int j = 0; j < 8; j++)
            {
                printf("\033[1;%dm %c \033[0m", color_codes[(i + j) % 2], tabla[i][j]);
            }
            printf(" %d\n", 8 - i); // Numerotarea liniilor
        }
        printf("   a  b  c  d  e  f  g  h\n\n");
    }
}

int isSquareAttacked(GameState *gs, int x, int y, int attackerPlayer)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
    {
        return 0;
    }
    int pionDir = (attackerPlayer == 0) ? -1 : 1; // Alb: -1 (sus), Negru: 1 (jos)
    if ((x + pionDir >= 0 && x + pionDir < 8 && y - 1 >= 0 && gs->tabla[x + pionDir][y - 1] == (attackerPlayer ? 'p' : 'P')) ||
        (x + pionDir >= 0 && x + pionDir < 8 && y + 1 < 8 && gs->tabla[x + pionDir][y + 1] == (attackerPlayer ? 'p' : 'P')))
    {
        return 1;
    }

    // Verifică atacurile calului
    char cal = attackerPlayer ? 'c' : 'C';
    int calMoves[8][2] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
    for (int i = 0; i < 8; i++)
    {
        int cx = x + calMoves[i][0];
        int cy = y + calMoves[i][1];
        if (cx >= 0 && cx < 8 && cy >= 0 && cy < 8 && gs->tabla[cx][cy] == cal)
        {
            return 1;
        }
    }

    // Verifică atacurile nebunului și atacurile diagonale ale damei
    char nebun = attackerPlayer ? 'n' : 'N';
    char dama = attackerPlayer ? 'd' : 'D';
    int nebunDirs[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (int i = 0; i < 4; i++)
    {
        int dx = nebunDirs[i][0];
        int dy = nebunDirs[i][1];
        int cx = x + dx;
        int cy = y + dy;

        while (cx >= 0 && cx < 8 && cy >= 0 && cy < 8)
        {
            char piece = gs->tabla[cx][cy];
            if (piece != ' ')
            {
                if (piece == nebun || piece == dama)
                {
                    return 1;
                }
                break; // Oprim căutarea în această direcție
            }
            cx += dx;
            cy += dy;
        }
    }

    // Verifică atacurile turnului și atacurile orizontale/verticale ale damei
    char turn = attackerPlayer ? 't' : 'T';
    int turnDirs[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    for (int i = 0; i < 4; i++)
    {
        int dx = turnDirs[i][0];
        int dy = turnDirs[i][1];
        int cx = x + dx;
        int cy = y + dy;

        while (cx >= 0 && cx < 8 && cy >= 0 && cy < 8)
        {
            char piece = gs->tabla[cx][cy];
            if (piece != ' ')
            {
                if (piece == turn || piece == dama)
                {
                    return 1;
                }
                break; // Oprim căutarea în această direcție
            }
            cx += dx;
            cy += dy;
        }
    }

    // Verifică atacurile regelui
    char rege = attackerPlayer ? 'r' : 'R';
    int regeMoves[8][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
    for (int i = 0; i < 8; i++)
    {
        int rx = x + regeMoves[i][0];
        int ry = y + regeMoves[i][1];
        if (rx >= 0 && rx < 8 && ry >= 0 && ry < 8 && gs->tabla[rx][ry] == rege)
        {
            return 1;
        }
    }

    return 0;
}

int isInCheck(GameState *gs, int player)
{
    int rege_x = -1, rege_y = -1;
    char rege = (player == 0) ? 'R' : 'r';
    // Caută regele pe tablă
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (gs->tabla[i][j] == rege)
            {
                rege_x = i;
                rege_y = j;
                break;
            }
        }
        if (rege_x != -1)
            break;
    }

    return isSquareAttacked(gs, rege_x, rege_y, !player);
}

int isCheckmate(GameState *gs)
{
    if (!isInCheck(gs, gs->currentPlayer))
        return 0;

    // Generează toate mutările posibile
    for (int x1 = 0; x1 < 8; x1++)
        for (int y1 = 0; y1 < 8; y1++)
            if ((gs->currentPlayer == 0 && isupper(gs->tabla[x1][y1])) ||
                (gs->currentPlayer == 1 && islower(gs->tabla[x1][y1])))
                for (int x2 = 0; x2 < 8; x2++)
                    for (int y2 = 0; y2 < 8; y2++)
                        if (validareMiscare(x1, y1, x2, y2, gs))
                            return 0;
    return 1;
}

void salveazaJocFEN(const GameState *gs, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Eroare la deschiderea fișierului pentru salvare!\n");
        return;
    }
    // Scrie tabla de șah
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int spaceCount = 0;
            while (gs->tabla[i][j] == ' ' && j < 8)
            {
                spaceCount++;
                j++;
            }
            if (j == 8)
            {
                fprintf(file, "%d", spaceCount);
            }
            else if (spaceCount > 0 && j < 8)
            {
                fprintf(file, "%d", spaceCount);
            }
            if (gs->tabla[i][j] != ' ')
            {
                fprintf(file, "%c", gs->tabla[i][j]);
            }
        }
        if (i < 7)
        {
            fprintf(file, "/");
        }
    }

    // cine e la rand
    fprintf(file, " %s ", gs->currentPlayer ? "N" : "A");

    // drepturi de rocada
    if (gs->canWhiteCastleKingside)
    {
        fprintf(file, "R");
    }
    if (gs->canWhiteCastleQueenside)
    {
        fprintf(file, "D");
    }
    if (gs->canBlackCastleKingside)
    {
        fprintf(file, "r");
    }
    if (gs->canBlackCastleQueenside)
    {
        fprintf(file, "d");
    }
    if (!(gs->canWhiteCastleKingside || gs->canWhiteCastleQueenside ||
          gs->canBlackCastleKingside || gs->canBlackCastleQueenside))
    {
        fprintf(file, "-");
    }

    // en passant
    if (gs->enPassantTarget[0] != -1 && gs->enPassantTarget[1] != -1)
    {
        switch (gs->enPassantTarget[1])
        {
        case 0:
            fprintf(file, " a");
            break;
        case 1:
            fprintf(file, " b");
            break;
        case 2:
            fprintf(file, " c");
            break;
        case 3:
            fprintf(file, " d");
            break;
        case 4:
            fprintf(file, " e");
            break;
        case 5:
            fprintf(file, " f");
            break;
        case 6:
            fprintf(file, " g");
            break;
        case 7:
            fprintf(file, " h");
            break;

        default:
            break;
        }
        fprintf(file, "%d ", gs->enPassantTarget[0] + 1);
    }
    else
    {
        fprintf(file, " - ");
    }

    // half move clock
    fprintf(file, "%d ", 50 - gs->halfmoveClock);

    // full move number
    fprintf(file, "%d\n", gs->fullmoveNumber);

    fclose(file);
    printf("Jocul a fost salvat în fișierul '%s'.fen\n", filename);
}

void salveazaJocPGN(const GameState *gs, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Eroare la deschiderea fișierului pentru salvare!\n");
        return;
    }

    // SCRIE HEADER-UL PGN

    fprintf(file, "[Event \"%s\"]\n", gs->pgn.event);
    fprintf(file, "[Site \"%s\"]\n", gs->pgn.site);
    fprintf(file, "[Date \"%s\"]\n", gs->pgn.date);
    fprintf(file, "[Round \"%d\"]\n", gs->pgn.round);
    fprintf(file, "[White \"%s\"]\n", gs->pgn.white);
    fprintf(file, "[Black \"%s\"]\n", gs->pgn.black);
    fprintf(file, "[Result \"%s\"]\n", gs->pgn.result);
    fprintf(file, "\n");

    fclose(file);
    printf("Jocul a fost salvat în fișierul '%s'.pgn\n", filename);
    // SCRIE ISTORICUL MUTARILOR
}

void afiseazaHeaderPGN(PGN *pgn)
{
    printf("[Event \"%s\"]\n", pgn->event);
    printf("[Site \"%s\"]\n", pgn->site);
    printf("[Date \"%s\"]\n", pgn->date);
    printf("[Round \"%d\"]\n", pgn->round);
    printf("[White \"%s\"]\n", pgn->white);
    printf("[Black \"%s\"]\n", pgn->black);
    printf("[Result \"%s\"]\n", pgn->result);
    printf("\n"); // Linie goală după header
}

// char moves[1000][10];
// int move_count = 0;
// void mutari_pgn(const char *filename)
// {
//     FILE *f = fopen(filename, "r");
//     if (!f)
//     {
//         perror("Eroare deschidere fișier");
//         return;
//     }

//     char word[20];
//     while (fscanf(f, "%s", word) == 1)
//     {
//         // Ignoră numerotări și taguri
//         if (strchr(word, '.') || word[0] == '[' || word[0] == '{')
//             continue;
//         if (move_count < 1000)
//         {
//             strncpy(moves[move_count++], word, 9);
//         }
//     }

//     fclose(f);
// }

void inchideJoc()
{
    printf("\n");
    printf("Mulțumim pentru joc!\n");
    printf("Programul se va încheia\n");
    exit(0); // Termină programul cu codul de succes 0
}

int reincepereJoc(GameState *gs, const char *filename, PGN *pgn, bool type)
{
    if (type == 0)
    {
        FILE *file = fopen(filename, "r");
        if (!file)
        {
            printf("Eroare: Fișierul '%s' nu există!\n", filename);
            return 0;
        }
        char buffer[256];

        // citeste tabla
        fgets(buffer, sizeof(buffer), file);
        int rand = 0, col = 0, isSpace = 0;

        for (int j = 0; j < strlen(buffer); j++)
        {
            if (isdigit(buffer[j]) && isSpace == 0)
            {
                int spaceCount = buffer[j] - '0';
                for (int cc = 0; cc < spaceCount; cc++)
                {
                    gs->tabla[rand][col++] = ' ';
                }
            }
            else if (isalpha(buffer[j]) && isSpace == 0)
            {
                gs->tabla[rand][col++] = buffer[j];
            }
            else if (buffer[j] == '/')
            {
                rand++;
                col = 0;
            }
            else if (buffer[j] == ' ' && isalpha(buffer[j + 1]))
            {
                isSpace++;
                if (isSpace == 1)
                {
                    switch (buffer[j + 1])
                    {
                    case 'A':
                        // Alb este jucătorul curent
                        gs->currentPlayer = 0;
                        break;
                    case 'N':
                        // Negru este jucătorul curent
                        gs->currentPlayer = 1;
                        break;
                    default:
                        break;
                    }
                }
                else if (isSpace == 2)
                {
                    switch (buffer[j + 1])
                    {
                    case 'R':
                        gs->canWhiteCastleKingside = 1;
                        break;
                    case 'D':
                        gs->canWhiteCastleQueenside = 1;
                        break;
                    case 'r':
                        gs->canBlackCastleKingside = 1;
                        break;
                    case 'd':
                        gs->canBlackCastleQueenside = 1;
                        break;
                    case '-':
                        gs->canWhiteCastleKingside = 0;
                        gs->canWhiteCastleQueenside = 0;
                        gs->canBlackCastleKingside = 0;
                        gs->canBlackCastleQueenside = 0;
                        break;
                    default:
                        break;
                    }
                }
                else if (isSpace == 3)
                {
                    switch (buffer[j + 1])
                    {
                    case 'a':
                        gs->enPassantTarget[1] = 0;
                        break;
                    case 'b':
                        gs->enPassantTarget[1] = 1;
                        break;
                    case 'c':
                        gs->enPassantTarget[1] = 2;
                        break;
                    case 'd':
                        gs->enPassantTarget[1] = 3;
                        break;
                    case 'e':
                        gs->enPassantTarget[1] = 4;
                        break;
                    case 'f':
                        gs->enPassantTarget[1] = 5;
                        break;
                    case 'g':
                        gs->enPassantTarget[1] = 6;
                        break;
                    case 'h':
                        gs->enPassantTarget[1] = 7;
                        break;
                    case '-':
                        gs->enPassantTarget[1] = -1;
                        gs->enPassantTarget[0] = -1;
                        break;
                    default:
                        break;
                    }
                    switch (buffer[j + 2])
                    {
                    case '1':
                        gs->enPassantTarget[0] = 0;
                        break;
                    case '2':
                        gs->enPassantTarget[0] = 1;
                        break;
                    case '3':
                        gs->enPassantTarget[0] = 2;
                        break;
                    case '4':
                        gs->enPassantTarget[0] = 3;
                        break;
                    case '5':
                        gs->enPassantTarget[0] = 4;
                        break;
                    case '6':
                        gs->enPassantTarget[0] = 5;
                        break;
                    case '7':
                        gs->enPassantTarget[0] = 6;
                        break;
                    case '8':
                        gs->enPassantTarget[0] = 7;
                        break;
                    default:
                        break;
                    }
                }
                else if (isSpace == 4)
                {
                    gs->halfmoveClock = atoi(buffer + j + 1);
                    gs->fullmoveNumber = atoi(buffer + j + 2);
                }
                continue;
            }
        }
        fclose(file);
        printf("Joc încărcat cu succes din '%s'!\n", filename);
        return 1;
    }
    else if (type == 1)
    {
        FILE *file = fopen(filename, "r");
        if (!file)
        {
            printf("Eroare: Fișierul '%s' nu există!\n", filename);
            return 0;
        }

        afiseazaHeaderPGN(pgn);

        fclose(file);
        printf("Joc încărcat cu succes din '%s'!\n", filename);
        return 1;
    }
    return 0;
}
// #include <stdio.h>
// #include <stdlib.h>
// #include <ctype.h>
// #include <string.h>

// // P / p = Pion alb / negru
// // C / c = Cal alb / negru
// // N / n = Nebun alb / negru
// // T / t = Turn alb / negru
// // D / d = Regină (damă) albă / neagră
// // R / r= Rege alb / negru
// // ' ' = Celulă goală

// typedef struct
// {
//     char tabla[8][8];
//     int currentPlayer; // 0 = alb, 1 = negru
//     int canWhiteCastleKingside;
//     int canWhiteCastleQueenside;
//     int canBlackCastleKingside;
//     int canBlackCastleQueenside;
//     int enPassantTarget[2]; // [x,y]
//     int halfmoveClock;
//     int fullmoveNumber;
// } GameState;

// void printTabla(char tabla[8][8])
// {
//     printf("   a b c d e f g h\n");
//     printf("  ----------------\n");
//     for (int i = 0; i < 8; i++)
//     {
//         printf("%d|", 8 - i); //
//         for (int j = 0; j < 8; j++)
//         {
//             printf(" %c", tabla[i][j]);
//         }
//         printf(" |%d\n", 8 - i);
//     }
//     printf("  ----------------\n");
//     printf("   a b c d e f g h\n");
// }

// int Pion(int x1, int y1, int x2, int y2, char tabla[8][8])
// {
//     if (tabla[x1][y1] == 'P' && x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0)
//     {
//         if (y1 == y2 && x2 == x1 - 1 && tabla[x2][y2] == ' ')
//             return 1;
//         if (y1 == y2 && x1 == 6 && x2 == 4 && tabla[5][y1] == ' ' && tabla[4][y1] == ' ')
//             return 1;
//         if ((abs(y1 - y2) == 1) && (x2 == x1 - 1) && (tabla[x2][y2] >= 'a' && tabla[x2][y2] <= 'z'))
//             return 1;
//     }
//     else if (tabla[x1][y1] == 'p')
//     {
//         if (y1 == y2 && x2 == x1 + 1 && tabla[x2][y2] == ' ')
//             return 1;
//         if (y1 == y2 && x1 == 1 && x2 == 3 && tabla[2][y1] == ' ' && tabla[3][y1] == ' ')
//             return 1;
//         if ((abs(y1 - y2) == 1) && (x2 == x1 + 1) && tabla[x2][y2] >= 'A' && tabla[x2][y2] <= 'Z')
//             return 1;
//     }
//     return 0;
// }

// int Cal(int x1, int y1, int x2, int y2, char tabla[8][8])
// {
//     if (tabla[x1][y1] == 'C' && x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0)
//     {
//         if ((abs(x1 - x2) == 2 && abs(y1 - y2) == 1) || (abs(x1 - x2) == 1 && abs(y1 - y2) == 2))
//         {
//             if (tabla[x2][y2] == ' ' || (tabla[x2][y2] >= 'a' && tabla[x2][y2] <= 'z'))
//             {
//                 return 1;
//             }
//         }
//     }
//     return 0;
// }

// int Nebun(int x1, int y1, int x2, int y2, char tabla[8][8])
// {
//     if (tabla[x1][y1] == 'N' && x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0 && abs(x1 - x2) == abs(y1 - y2))
//     {
//         int dx = (x2 > x1) ? 1 : -1;
//         int dy = (y2 > y1) ? 1 : -1;
//         int i = x1 + dx, j = y1 + dy;
//         while (i != x2 && j != y2)
//         {
//             if (tabla[i][j] != ' ')
//                 return 0;
//             i += dx;
//             j += dy;
//         }
//         return (tabla[x2][y2] == ' ' || (tabla[x2][y2] >= 'a' && tabla[x2][y2] <= 'z'));
//     }
//     return 0;
// }

// int Turn(int x1, int y1, int x2, int y2, char tabla[8][8])
// {
//     if (tabla[x1][y1] == 'T')
//     {
//         if (x1 == x2)
//         {
//             int dy = (y2 > y1) ? 1 : -1;
//             for (int j = y1 + dy; j != y2; j += dy)
//                 if (tabla[x1][j] != ' ')
//                     return 0;
//         }
//         else if (y1 == y2)
//         {
//             int dx = (x2 > x1) ? 1 : -1;
//             for (int i = x1 + dx; i != x2; i += dx)
//                 if (tabla[i][y1] != ' ')
//                     return 0;
//         }
//         else
//             return 0;
//         return (tabla[x2][y2] == ' ' || (tabla[x2][y2] >= 'a' && tabla[x2][y2] <= 'z'));
//     }
//     return 0;
// }

// int Dama(int x1, int y1, int x2, int y2, char tabla[8][8])
// {

//     int dx = (x2 > x1) ? 1 : -1;
//     int dy = (y2 > y1) ? 1 : -1;
//     int i, j;

//     if (tabla[x1][y1] == 'D' && x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0)
//     {
//         if (x1 == x2)
//         {
//             for (j = y1 + dy; j != y2; j += dy)
//                 if (tabla[x1][j] != ' ')
//                     return 0;
//         }
//         else if (y1 == y2)
//         {
//             for (i = x1 + dx; i != x2; i += dx)
//                 if (tabla[i][y1] != ' ')
//                     return 0;
//         }
//         else
//             return 0;
//         return (tabla[x2][y2] == ' ' || (tabla[x2][y2] >= 'a' && tabla[x2][y2] <= 'z'));

//         if (abs(x1 - x2) == abs(y1 - y2))
//         {
//             i = x1 + dx;
//             j = y1 + dy;
//             while (i != x2 && j != y2)
//             {
//                 if (tabla[i][j] != ' ')
//                     return 0;
//                 i += dx;
//                 j += dy;
//             }
//             return (tabla[x2][y2] == ' ' || (tabla[x2][y2] >= 'a' && tabla[x2][y2] <= 'z'));
//         }
//     }
//     return 0;
// }

// int Rege(int x1, int y1, int x2, int y2, char tabla[8][8])
// {
//     if (tabla[x1][y1] == 'R' && x2 < 8 && x2 >= 0 && y2 < 8 && y2 >= 0)
//     {
//         if ((x1 == x2) && (y2 == y1 + 1))
//             return 1;
//         if ((x1 == x2) && (y2 == y1 - 1))
//             return 1;
//         if ((y1 == y2) && (x2 == x1 + 1))
//             return 1;
//         if ((y1 == y2) && (x2 == x1 - 1))
//             return 1;
//         if ((x2 == x1 + 1) && (y2 == y1 + 1))
//             return 1;
//         if ((x2 == x1 + 1) && (y2 == y1 - 1))
//             return 1;
//         if ((x2 == x1 - 1) && (y2 == y1 + 1))
//             return 1;
//         if ((x2 == x1 - 1) && (y2 == y1 - 1))
//             return 1;
//     }
//     return 0;
// }

// void transformareMiscare(char *start, char *finish, int *x1, int *y1, int *x2, int *y2)
// {
//     *y1 = tolower(start[0]) - 'a';
//     *x1 = 8 - (start[1] - '0');
//     *y2 = tolower(finish[0] - 'a');
//     *x2 = 8 - (finish[1] - '0');
// }

// int validareMiscare(int x1, int y1, int x2, int y2, GameState *gs)
// {
//     // Verifică dacă piesa aparține jucătorului curent
//     char piece = gs->tabla[x1][y1];
//     if (gs->currentPlayer == 0 && islower(piece))
//         return 0;
//     if (gs->currentPlayer == 1 && isupper(piece))
//         return 0;

//     // Verifică rocada
//     if ((piece == 'R' || piece == 'r') && abs(y2 - y1) == 2)
//         return validareRocada(x1, y1, x2, y2, gs);

//     // Apelul original la funcțiile de validare
//     int valid = 0;
//     switch (toupper(piece))
//     {
//     case 'P':
//         valid = Pion(x1, y1, x2, y2, gs);
//         break;
//     case 'C':
//         valid = Cal(x1, y1, x2, y2, gs);
//         break;
//     case 'N':
//         valid = Nebun(x1, y1, x2, y2, gs);
//         break;
//     case 'T':
//         valid = Turn(x1, y1, x2, y2, gs);
//         break;
//     case 'D':
//         valid = Dama(x1, y1, x2, y2, gs);
//         break;
//     case 'R':
//         valid = Rege(x1, y1, x2, y2, gs);
//         break;
//     }

//     return valid && !wouldCheckOccur(gs, x1, y1, x2, y2);
// }

// int main()
// {
//     GameState gs = {
//         .tabla = {
//             {'t', 'c', 'n', 'd', 'r', 'n', 'c', 't'},
//             {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
//             {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
//             {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
//             {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
//             {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
//             {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
//             {'T', 'C', 'N', 'D', 'R', 'N', 'C', 'T'}},
//         .currentPlayer = 0,
//         .canWhiteCastleKingside = 1,
//         .canWhiteCastleQueenside = 1,
//         .canBlackCastleKingside = 1,
//         .canBlackCastleQueenside = 1,
//         .enPassantTarget = {-1, -1},
//         .halfmoveClock = 0,
//         .fullmoveNumber = 1};

//     while (1)
//     {
//         printTabla(gs.tabla);
//         printf("Jucător %s la mutare\n", gs.currentPlayer ? "negru" : "alb");

//         // ... restul codului
//         if (validareMiscare(x1, y1, x2, y2, &gs))
//         {
//             executaMutare(x1, y1, x2, y2, &gs);
//             if (isCheckmate(&gs))
//             {
//                 printf("Șah mat! %s câștigă!\n", gs.currentPlayer ? "Alb" : "Negru");
//                 break;
//             }
//             gs.currentPlayer = !gs.currentPlayer;
//         }
//     }
//     return 0;
// }

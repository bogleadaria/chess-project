#include "menu.h"
int main() {
    show_menu_and_start_game();
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <ctype.h>
// #include <stdbool.h>

// #include "game.h"
// #include "move_validation.h"
// #include "ai.h"

// int main()
// {

//     GameState gs = initializeGame();
//     initializeaza_tabele_negre();

//     char start[50], finish[3], fisier[50];
//     int x1, y1, x2, y2;
//     visual();

//     printf("\n\n\nAlegeti o culore (alb/negru): ");
//     char culoare[10];
//     scanf("%s", culoare);
//     if (strcmp(culoare, "negru") == 0)
//     {
//         printf("Veti juca piesele negre.\n");
//         gs.culoare_ai = 0; // Alb
//         gs.currentPlayer = 0;

//         while (1)
//         {
//             printTabla(gs.tabla, gs.culoare_ai);
//             printf("Jucător curent: %s\n", gs.currentPlayer ? "Negru (TU)" : "Alb (ENGINE)");

//             // Rândul AI-ului (alb)
//             if (gs.currentPlayer == 0)
//             {
//                 printf("Engine-ul gândește...\n");
//                 Move best = findBestMove(&gs, gs.culoare_ai);
//                 if (best.x1 == -1)
//                 {
//                     printf("Nicio mutare validă! Remiză sau mat.\n");
//                     break;
//                 }
//                 printf("Engine alege: %c%d-%c%d\n",
//                        'a' + best.y1, 8 - best.x1, 'a' + best.y2, 8 - best.x2);
//                 executa_mutare(best.x1, best.y1, best.x2, best.y2, &gs);

//                 // Verifică șah mat
//                 if (isCheckmate(&gs))
//                 {
//                     printTabla(gs.tabla, gs.culoare_ai);
//                     printf("Șah mat! Alb (ENGINE) câștigă!\n");
//                     break;
//                 }

//                 // Verifică pat (remiză)
//                 if (!existaMutareLegala(&gs))
//                 {
//                     if (isInCheck(&gs, gs.currentPlayer))
//                     {
//                         printf("Eroare logică: ar fi trebuit să fie șah mat!\n");
//                     }
//                     else
//                     {
//                         printTabla(gs.tabla, gs.culoare_ai);
//                         printf("Remiză prin pat! Nicio mutare legală și regele NU este în șah.\n");
//                         break;
//                     }
//                 }

//                 // Verifică dacă adversarul este în șah după mutare
//                 if (isInCheck(&gs, gs.currentPlayer))
//                 {
//                     printf("\n\nAtenție: %s este în șah!\n", gs.currentPlayer ? "Negru" : "Alb");
//                 }

//                 gs.currentPlayer = !gs.currentPlayer;
//                 continue;
//             }

//             if (isInCheck(&gs, gs.currentPlayer))
//             {
//                 int nr_regi = 0;
//                 for (int m = 0; m < 8; m++)
//                     for (int n = 0; n < 8; n++)
//                     {
//                         if (gs.tabla[m][n] == 'R')
//                             nr_regi++;
//                         if (gs.tabla[m][n] == 'r')
//                             nr_regi++;
//                     }
//                 if (nr_regi < 2)
//                 {
//                     return 0; // jocul s-a terminat, nu mai sunt regi
//                 }
//                 else
//                 {
//                     printf("\n\nAtenție: %s este în șah!\n", gs.currentPlayer ? "Negru  (TU)" : "Alb (ENGINE)");
//                     // printf("\n\nNu poti muta alta piesa pana nu iesi din șah.");
//                     //TODO: verifica daca mutarea viitoare te scoate din șah
//                 }
//             }

//             // Rândul omului (negru)
//             printf("Introduceți 'save nume_fisier' pentru a salva,\n");
//             printf("'load_state nume_fisier' pentru a încărca un joc dintr-un fisier FEN\n");
//             printf("'load_game nume_fisier' pentru a incarca un joc dintr-un fisier PGN\n");
//             printf("'exit' pentru a ieși\n");
//             printf("sau mutarea (ex: e2 e4): ");

//             scanf("%s", start);

//             if (strcmp(start, "save") == 0)
//             {
//                 scanf("%s", fisier);
//                 salveazaJoc(&gs, fisier);
//                 continue;
//             }
//             else if (strcmp(start, "load_state") == 0)
//             {
//                 scanf("%s", fisier);
//                 reincepereJoc(&gs, fisier, 0); // FEN
//                 continue;
//             }
//             else if (strcmp(start, "load_game") == 0)
//             {
//                 scanf("%s", fisier);
//                 reincepereJoc(&gs, fisier, 1); // PGN
//                 continue;
//             }
//             else if (strcmp(start, "exit") == 0)
//             {
//                 inchideJoc();
//             }

//             scanf("%s", finish);
//             transformareMiscare(start, finish, &x1, &y1, &x2, &y2, gs.culoare_ai);

//             int valid = validareMiscare(x1, y1, x2, y2, &gs);

//             if (valid == 1 && !isInCheck(&gs, gs.currentPlayer))
//             {
//                 executa_mutare(x1, y1, x2, y2, &gs);

//                 // Verifică șah mat
//                 if (isCheckmate(&gs))
//                 {
//                     printTabla(gs.tabla, gs.culoare_ai);
//                     printf("Șah mat! Negru (TU) câștigă!\n");
//                     break;
//                 }

//                 // Verifică pat (remiză)
//                 if (!existaMutareLegala(&gs))
//                 {
//                     if (isInCheck(&gs, gs.currentPlayer))
//                     {
//                         printf("Eroare logică: ar fi trebuit să fie șah mat!\n");
//                     }
//                     else
//                     {
//                         printTabla(gs.tabla, gs.culoare_ai);
//                         printf("Remiză prin pat! Nicio mutare legală și regele NU este în șah.\n");
//                         break;
//                     }
//                 }

//                 gs.currentPlayer = !gs.currentPlayer;
//             }
//             else if (valid == 0)
//             {
//                 char piece = gs.tabla[x1][y1];
//                 if (piece == ' ' ||
//                     (gs.currentPlayer == 0 && islower(piece)) ||
//                     (gs.currentPlayer == 1 && isupper(piece)))
//                 {
//                     printf("Jucător greșit! Nu poți muta piesa adversarului.\n");
//                 }
//                 else if(piece == 'r' || piece == 'R')
//                 {
//                     //TODO: aici de fapt vreau rocada daca regele vrea 2 laterala
//                     if(validareRocada(x1, y1, x2, y2, &gs) == 1)
//                     {
//                         executa_mutare(x1, y1, x2, y2, &gs);
//                         gs.currentPlayer = !gs.currentPlayer;
//                     }
//                     else
//                     {
//                         printf("Mutare invalidă! Nu poți lăsa regele în șah.\n");
//                     }
//                 }
//                 else
//                 {
//                     printf("Mutare invalidă!\n");
//                 }
//             }
//             else if (valid == -1)
//             {
//                 printf("Mutare ilegală! Nu poți lăsa regele în șah.\n");

//                 // Verifică dacă adversarul este în șah după mutare
//             }
//             else
//             {
//                 if (mutareIeseDinSah(x1, y1, x2, y2, &gs))
//                 {
//                     printf("Mutarea nu te scoate din șah.\n");
//                 }
//                 else if (validareMiscare(x1, y1, x2, y2, &gs))
//                 {
//                     executa_mutare(x1, y1, x2, y2, &gs);
//                 }
//                 else
//                 {
//                     printf("Mutare invalidă! Nu poți lăsa regele în șah.\n");
//                 }
//             }
//         }
//     }
//     else
//     {
//         printf("Veti juca piesele albe.\n");
//         gs.culoare_ai = 1; // Negru
//         gs.currentPlayer = 0; // Alb

//         while (1)
//         {
//             printTabla(gs.tabla, gs.culoare_ai);
//             printf("Jucător curent: %s\n", gs.currentPlayer ? "Negru (ENGINE)" : "Alb (TU)");

//             // Rândul AI-ului (negru)
//             if (gs.currentPlayer == 1)
//             {
//                 printf("Engine-ul gândește...\n");
//                 Move best = findBestMove(&gs, gs.culoare_ai);
//                 if (best.x1 == -1)
//                 {
//                     printf("Nicio mutare validă! Remiză sau mat.\n");
//                     break;
//                 }
//                 printf("Engine alege: %c%d-%c%d\n",
//                        'a' + best.y1, 8 - best.x1, 'a' + best.y2, 8 - best.x2);
//                 executa_mutare(best.x1, best.y1, best.x2, best.y2, &gs);

//                 // Verifică șah mat
//                 if (isCheckmate(&gs))
//                 {
//                     printTabla(gs.tabla, gs.culoare_ai);
//                     printf("Șah mat! Negru (ENGINE) câștigă!\n");
//                     break;
//                 }

//                 // Verifică pat (remiză)
//                 if (!existaMutareLegala(&gs))
//                 {
//                     if (isInCheck(&gs, gs.currentPlayer))
//                     {
//                         printf("Eroare logică: ar fi trebuit să fie șah mat!\n");
//                     }
//                     else
//                     {
//                         printTabla(gs.tabla, gs.culoare_ai);
//                         printf("Remiză prin pat! Nicio mutare legală și regele NU este în șah.\n");
//                         break;
//                     }
//                 }

//                 // Verifică dacă adversarul este în șah după mutare
//                 if (isInCheck(&gs, gs.currentPlayer))
//                 {
//                     printf("\n\nAtenție: %s este în șah!\n", gs.currentPlayer ? "Negru" : "Alb");
//                 }

//                 gs.currentPlayer = !gs.currentPlayer;
//                 continue;
//             }

//             if (isInCheck(&gs, gs.currentPlayer))
//             {
//                 int nr_regi = 0;
//                 for (int m = 0; m < 8; m++)
//                     for (int n = 0; n < 8; n++)
//                     {
//                         if (gs.tabla[m][n] == 'R')
//                             nr_regi++;
//                         if (gs.tabla[m][n] == 'r')
//                             nr_regi++;
//                     }
//                 if (nr_regi < 2)
//                 {
//                     return 0; // jocul s-a terminat, nu mai sunt regi
//                 }
//                 else
//                 {
//                     printf("\n\nAtenție: %s este în șah!\n\n\n", gs.currentPlayer ? "Negru (ENGINE)" : "Alb (TU)");
//                     // printf("\n\nNu poti muta alta piesa pana nu iesi din șah.");
//                     //TODO: verifica daca mutarea viitoare te scoate din șah
//                 }
//             }

//             // Rândul omului (alb)
//             printf("Introduceți 'save nume_fisier' pentru a salva,\n");
//             printf("'load_state nume_fisier' pentru a încărca un joc dintr-un fisier FEN\n");
//             printf("'load_game nume_fisier' pentru a incarca un joc dintr-un fisier PGN\n");
//             printf("'exit' pentru a ieși\n");
//             printf("sau mutarea (ex: e2 e4): ");

//             scanf("%s", start);

//             if (strcmp(start, "save") == 0)
//             {
//                 scanf("%s", fisier);
//                 salveazaJoc(&gs, fisier);
//                 continue;
//             }
//             else if (strcmp(start, "load_state") == 0)
//             {
//                 scanf("%s", fisier);
//                 reincepereJoc(&gs, fisier, 0); // FEN
//                 continue;
//             }
//             else if (strcmp(start, "load_game") == 0)
//             {
//                 scanf("%s", fisier);
//                 reincepereJoc(&gs, fisier, 1); // PGN
//                 continue;
//             }
//             else if (strcmp(start, "exit") == 0)
//             {
//                 inchideJoc();
//             }

//             scanf("%s", finish);
//             transformareMiscare(start, finish, &x1, &y1, &x2, &y2, gs.culoare_ai);

//             int valid = validareMiscare(x1, y1, x2, y2, &gs);

//             if (valid == 1 && !isInCheck(&gs, gs.currentPlayer))
//             {
//                 executa_mutare(x1, y1, x2, y2, &gs);

//                 // Verifică șah mat
//                 if (isCheckmate(&gs))
//                 {
//                     printTabla(gs.tabla, gs.culoare_ai);
//                     printf("Șah mat! Alb (TU) câștigă!\n");
//                     break;
//                 }

//                 // Verifică pat (remiză)
//                 if (!existaMutareLegala(&gs))
//                 {
//                     if (isInCheck(&gs, gs.currentPlayer))
//                     {
//                         printf("Eroare logică: ar fi trebuit să fie șah mat!\n");
//                     }
//                     else
//                     {
//                         printTabla(gs.tabla, gs.culoare_ai);
//                         printf("Remiză prin pat! Nicio mutare legală și regele NU este în șah.\n");
//                         break;
//                     }
//                 }

//                 gs.currentPlayer = !gs.currentPlayer;
//             }
//             else if (valid == 0)
//             {
//                 char piece = gs.tabla[x1][y1];
//                 if (piece == ' ' ||
//                     (gs.currentPlayer == 0 && islower(piece)) ||
//                     (gs.currentPlayer == 1 && isupper(piece)))
//                 {
//                     printf("Jucător greșit! Nu poți muta piesa adversarului.\n");
//                 }
//                 else if(piece == 'r' || piece == 'R')
//                 {
//                     //TODO: aici de fapt vreau rocada daca regele vrea 2 laterala
//                     if(validareRocada(x1, y1, x2, y2, &gs) == 1)
//                     {
//                         printf("ceva\n");
//                         printf("%d", gs.tabla[x1][y1]);
//                         executa_mutare(x1, y1, x2, y2, &gs);
//                         gs.currentPlayer = !gs.currentPlayer;
//                     }
//                     else
//                     {
//                         printf("Mutare invalidă! Nu poți lăsa regele în șah.\n");
//                     }
//                 }
//                 else
//                 {
//                     printf("Mutare invalidă!\n");
//                 }
//             }
//             else if (valid == -1)
//             {
//                 printf("Mutare ilegală! Nu poți lăsa regele în șah.\n");

//                 // Verifică dacă adversarul este în șah după mutare
//             }
//             else
//             {
//                 if (mutareIeseDinSah(x1, y1, x2, y2, &gs))
//                 {
//                     printf("Mutarea nu te scoate din șah.\n");
//                 }
//                 else if (validareMiscare(x1, y1, x2, y2, &gs))
//                 {
//                     executa_mutare(x1, y1, x2, y2, &gs);
//                 }
//                 else
//                 {
//                     printf("Mutare invalidă! Nu poți lăsa regele în șah.\n");
//                 }
//             }
//         }
//     }

//     return 0;
// }
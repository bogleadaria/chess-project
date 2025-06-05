
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ctype.h>
#include <string.h>

#include "game.h"
#include "move_validation.h"
#include "ai.h"
#include "mutari.h"

Move NIMIC()
{
    Move avoid = {
        .x1 = -1,
        .y1 = -1,
        .x2 = -1,
        .y2 = -1,
        .scor = 0.0};
    return avoid;
}

int main()
{
    Move nimic = NIMIC();
    GameState gs;
    // PGN pgn;

    gs = initializeGame();
    initializeaza_tabele_negre();

    char event[250];
    printf("Event: ");
    fgets(event, sizeof(event), stdin);
    event[strcspn(event, "\n")] = '\0'; // Elimină newline-ul
    strncpy(gs.pgn.event, event, sizeof(gs.pgn.event) - 1);
    gs.pgn.event[sizeof(gs.pgn.event) - 1] = '\0';

    char site[250];
    printf("Site: ");
    fgets(site, sizeof(site), stdin);
    site[strcspn(site, "\n")] = '\0'; // Elimină newline-ul
    strncpy(gs.pgn.site, site, sizeof(gs.pgn.site) - 1);
    gs.pgn.site[sizeof(gs.pgn.site) - 1] = '\0';

    char date[50];
    printf("Date: ");
    fgets(date, sizeof(date), stdin);
    date[strcspn(date, "\n")] = '\0'; // Elimină newline-ul
    strncpy(gs.pgn.date, date, sizeof(gs.pgn.date) - 1);
    gs.pgn.date[sizeof(gs.pgn.date) - 1] = '\0';

    int round = 0;

    char(white[250]);
    printf("White (nume jucător): ");
    fgets(white, sizeof(white), stdin);
    white[strcspn(white, "\n")] = '\0'; // Elimină newline-ul
    strncpy(gs.pgn.white, white, sizeof(gs.pgn.white) - 1);
    gs.pgn.white[sizeof(gs.pgn.white) - 1] = '\0';

    char black[250];
    printf("Black (nume jucător): ");
    fgets(black, sizeof(black), stdin);
    black[strcspn(black, "\n")] = '\0'; // Elimină newline-ul
    strncpy(gs.pgn.black, black, sizeof(gs.pgn.black) - 1);
    gs.pgn.black[sizeof(gs.pgn.black) - 1] = '\0';

    char result[10] = "*";

    printf("\n\n\nAlegeti o culoare (alb/negru): ");
    char culoare[10];
    scanf("%s", culoare);
    if (strcmp(culoare, "negru") == 0)
    {
        printf("Veti juca piesele negre.\n");
        gs.culoare_ai = 0; // Alb
    }
    else if (strcmp(culoare, "alb") == 0)
    {
        printf("Veti juca piesele albe.\n");
        gs.culoare_ai = 1; // Negru
    }
    else
    {
        printf("Culoare invalidă! Programul se va încheia.\n");
        return 1;
    }

    printTabla(gs.tabla, gs.culoare_ai);

    char action[20];
    while (1)
    {
        if (gs.isPGN == 0)
        {
            if (gs.culoare_ai == gs.currentPlayer)
            {
                printf("\nEngine-ul gândește...\n");

                Move best = findBestMove(&gs, gs.culoare_ai, nimic);

                int valid_ai = validareMiscare(best.x1, best.y1, best.x2, best.y2, &gs);

                if (valid_ai)
                {
                    // AI IN SAH?
                    if (isInCheck(&gs, gs.currentPlayer))
                    {
                        if (mutareIeseDinSah(best.x1, best.y1, best.x2, best.y2, &gs))
                        {
                            // MUTAREA AI-ULUI NU IL SCOATE DIN SAH
                            best = findBestMove(&gs, gs.culoare_ai, best); // cauta alta mutare
                            continue;
                        }
                        else
                        {
                            printf("Engine alege: %c%d-%c%d\n",
                                   'a' + best.y1, 8 - best.x1, 'a' + best.y2, 8 - best.x2);
                            executa_mutare(best.x1, best.y1, best.x2, best.y2, &gs);
                            printTabla(gs.tabla, gs.culoare_ai);
                        }
                    }
                    else
                    {
                        if (mutareIeseDinSah(best.x1, best.y1, best.x2, best.y2, &gs))
                        {
                            // MUTAREA AI-ULUI IL BAGA IN SAH
                            best = findBestMove(&gs, gs.culoare_ai, best); // cauta alta mutare
                            continue;
                        }
                        else
                        {
                            printf("Engine alege: %c%d-%c%d\n",
                                   'a' + best.y1, 8 - best.x1, 'a' + best.y2, 8 - best.x2);
                            executa_mutare(best.x1, best.y1, best.x2, best.y2, &gs);
                            printTabla(gs.tabla, gs.culoare_ai);
                        }
                    }
                    if (isCheckmate(&gs))
                    {
                        printf("\nSah mat! Jucătorul %s a câștigat!\n", gs.currentPlayer == 0 ? "alb" : "negru");
                        break;
                    }
                    else
                    {
                        if (esteRemiza(&gs))
                        {
                            printf("\nRemiză!\n");
                            break;
                        }
                        else if (gs.isPGN)
                        {
                            gs.currentPlayer = !gs.currentPlayer;
                            gs.culoare_ai = gs.currentPlayer;
                        }
                        else
                        {
                            gs.currentPlayer = !gs.currentPlayer;
                        }
                    }
                }
                else if (valid_ai == -1)
                {
                    // PIESA NU APARTINE AI-ULUI
                    continue;
                }
                else
                {
                    // MUTARE INVALIDA
                    continue;
                }
            }
            else
            {
                printf("\nIntroduceți 'save nume_fisier.fen/pgn' pentru a salva,\n");
                printf("\n'load_state nume_fisier' pentru a încărca un joc dintr-un fisier FEN\n");
                printf("\n'load_game nume_fisier' pentru a incarca un joc dintr-un fisier PGN\n");
                printf("\n'exit' pentru a ieși\n");
                printf("\nsau mutarea (ex: e2 e4): ");

                scanf("%s", action);
                // SAVE
                if (strcmp(action, "save") == 0)
                {
                    char filename[100];
                    scanf("%s", filename);

                    // CE TIP DE FISIER?
                    if (endsWith(filename, ".fen"))
                    {
                        salveazaJocFEN(&gs, filename);
                        continue;
                    }
                    else if (endsWith(filename, ".pgn"))
                    {
                        gs.pgn.round = (gs.pgn.historyCount + 1) / 2;
                        strcpy(gs.pgn.result, result);
                        salveazaJocPGN(&gs, filename);
                        continue;
                    }
                    else
                    {
                        printf("\nExtensie invalidă! Trebuie să fie .fen sau .pgn\n");
                        continue;
                    }
                }
                // LOAD STATE
                else if (strcmp(action, "load_state") == 0)
                {
                    char filename[100];
                    scanf("%s", filename);

                    // REGANDIT IF
                    if (endsWith(filename, ".fen"))
                    {
                        gs.isPGN = 0; // FEN
                        reincepereJoc(&gs, filename, &gs.pgn, gs.isPGN); // FEN
                        continue;
                    }
                    else
                    {
                        printf("\nExtensie invalidă! Trebuie să fie .fen\n");
                        continue;
                    }
                }
                // LOAD GAME
                else if (strcmp(action, "load_game") == 0)
                {
                    char filename[100];
                    scanf("%s", filename);
                    if (endsWith(filename, ".pgn"))
                    {
                        gs.isPGN = 1; // PGN
                        reincepereJoc(&gs, filename, &gs.pgn, gs.isPGN); // PGN
                        continue;
                    }
                    else
                    {
                        printf("\nExtensie invalidă! Trebuie să fie .pgn\n");
                        continue;
                    }
                }
                // EXIT
                else if (strcmp(action, "exit") == 0)
                {
                    inchideJoc();
                }
                // JOC - MUTARI
                else
                {
                    char start[3], finish[3];
                    strcpy(start, action);
                    scanf("%s", finish);
#include "mutari.h"

Move NIMIC()
{
    Move avoid = {
        .x1 = -1,
        .y1 = -1,
        .x2 = -1,
        .y2 = -1,
        .scor = 0.0};
    return avoid;
}

int main()
{
    Move nimic = NIMIC();
    GameState gs;
    // PGN pgn;

    gs = initializeGame();
    initializeaza_tabele_negre();

    char event[250];
    printf("Event: ");
    fgets(event, sizeof(event), stdin);
    event[strcspn(event, "\n")] = '\0'; // Elimină newline-ul
    strncpy(gs.pgn.event, event, sizeof(gs.pgn.event) - 1);
    gs.pgn.event[sizeof(gs.pgn.event) - 1] = '\0';

    char site[250];
    printf("Site: ");
    fgets(site, sizeof(site), stdin);
    site[strcspn(site, "\n")] = '\0'; // Elimină newline-ul
    strncpy(gs.pgn.site, site, sizeof(gs.pgn.site) - 1);
    gs.pgn.site[sizeof(gs.pgn.site) - 1] = '\0';

    char date[50];
    printf("Date: ");
    fgets(date, sizeof(date), stdin);
    date[strcspn(date, "\n")] = '\0'; // Elimină newline-ul
    strncpy(gs.pgn.date, date, sizeof(gs.pgn.date) - 1);
    gs.pgn.date[sizeof(gs.pgn.date) - 1] = '\0';

    int round = 0;

    char(white[250]);
    printf("White (nume jucător): ");
    fgets(white, sizeof(white), stdin);
    white[strcspn(white, "\n")] = '\0'; // Elimină newline-ul
    strncpy(gs.pgn.white, white, sizeof(gs.pgn.white) - 1);
    gs.pgn.white[sizeof(gs.pgn.white) - 1] = '\0';

    char black[250];
    printf("Black (nume jucător): ");
    fgets(black, sizeof(black), stdin);
    black[strcspn(black, "\n")] = '\0'; // Elimină newline-ul
    strncpy(gs.pgn.black, black, sizeof(gs.pgn.black) - 1);
    gs.pgn.black[sizeof(gs.pgn.black) - 1] = '\0';

    char result[10] = "*";

    printf("\n\n\nAlegeti o culoare (alb/negru): ");
    char culoare[10];
    scanf("%s", culoare);
    if (strcmp(culoare, "negru") == 0)
    {
        printf("Veti juca piesele negre.\n");
        gs.culoare_ai = 0; // Alb
    }
    else if (strcmp(culoare, "alb") == 0)
    {
        printf("Veti juca piesele albe.\n");
        gs.culoare_ai = 1; // Negru
    }
    else
    {
        printf("Culoare invalidă! Programul se va încheia.\n");
        return 1;
    }

    printTabla(gs.tabla, gs.culoare_ai);

    char action[20];
    while (1)
    {
        if (gs.isPGN == 0)
        {
            if (gs.culoare_ai == gs.currentPlayer)
            {
                printf("\nEngine-ul gândește...\n");

                Move best = findBestMove(&gs, gs.culoare_ai, nimic);

                int valid_ai = validareMiscare(best.x1, best.y1, best.x2, best.y2, &gs);

                if (valid_ai)
                {
                    // AI IN SAH?
                    if (isInCheck(&gs, gs.currentPlayer))
                    {
                        if (mutareIeseDinSah(best.x1, best.y1, best.x2, best.y2, &gs))
                        {
                            // MUTAREA AI-ULUI NU IL SCOATE DIN SAH
                            best = findBestMove(&gs, gs.culoare_ai, best); // cauta alta mutare
                            continue;
                        }
                        else
                        {
                            printf("Engine alege: %c%d-%c%d\n",
                                   'a' + best.y1, 8 - best.x1, 'a' + best.y2, 8 - best.x2);
                            executa_mutare(best.x1, best.y1, best.x2, best.y2, &gs);
                            printTabla(gs.tabla, gs.culoare_ai);
                        }
                    }
                    else
                    {
                        if (mutareIeseDinSah(best.x1, best.y1, best.x2, best.y2, &gs))
                        {
                            // MUTAREA AI-ULUI IL BAGA IN SAH
                            best = findBestMove(&gs, gs.culoare_ai, best); // cauta alta mutare
                            continue;
                        }
                        else
                        {
                            printf("Engine alege: %c%d-%c%d\n",
                                   'a' + best.y1, 8 - best.x1, 'a' + best.y2, 8 - best.x2);
                            executa_mutare(best.x1, best.y1, best.x2, best.y2, &gs);
                            printTabla(gs.tabla, gs.culoare_ai);
                        }
                    }
                    if (isCheckmate(&gs))
                    {
                        printf("\nSah mat! Jucătorul %s a câștigat!\n", gs.currentPlayer == 0 ? "alb" : "negru");
                        break;
                    }
                    else
                    {
                        if (esteRemiza(&gs))
                        {
                            printf("\nRemiză!\n");
                            break;
                        }
                        else if (gs.isPGN)
                        {
                            gs.currentPlayer = !gs.currentPlayer;
                            gs.culoare_ai = gs.currentPlayer;
                        }
                        else
                        {
                            gs.currentPlayer = !gs.currentPlayer;
                        }
                    }
                }
                else if (valid_ai == -1)
                {
                    // PIESA NU APARTINE AI-ULUI
                    continue;
                }
                else
                {
                    // MUTARE INVALIDA
                    continue;
                }
            }
            else
            {
                printf("\nIntroduceți 'save nume_fisier.fen/pgn' pentru a salva,\n");
                printf("\n'load_state nume_fisier' pentru a încărca un joc dintr-un fisier FEN\n");
                printf("\n'load_game nume_fisier' pentru a incarca un joc dintr-un fisier PGN\n");
                printf("\n'exit' pentru a ieși\n");
                printf("\nsau mutarea (ex: e2 e4): ");

                scanf("%s", action);
                // SAVE
                if (strcmp(action, "save") == 0)
                {
                    char filename[100];
                    scanf("%s", filename);

                    // CE TIP DE FISIER?
                    if (endsWith(filename, ".fen"))
                    {
                        salveazaJocFEN(&gs, filename);
                        continue;
                    }
                    else if (endsWith(filename, ".pgn"))
                    {
                        gs.pgn.round = (gs.pgn.historyCount + 1) / 2;
                        strcpy(gs.pgn.result, result);
                        salveazaJocPGN(&gs, filename);
                        continue;
                    }
                    else
                    {
                        printf("\nExtensie invalidă! Trebuie să fie .fen sau .pgn\n");
                        continue;
                    }
                }
                // LOAD STATE
                else if (strcmp(action, "load_state") == 0)
                {
                    char filename[100];
                    scanf("%s", filename);

                    // REGANDIT IF
                    if (endsWith(filename, ".fen"))
                    {
                        gs.isPGN = 0; // FEN
                        reincepereJoc(&gs, filename, &gs.pgn, gs.isPGN); // FEN
                        continue;
                    }
                    else
                    {
                        printf("\nExtensie invalidă! Trebuie să fie .fen\n");
                        continue;
                    }
                }
                // LOAD GAME
                else if (strcmp(action, "load_game") == 0)
                {
                    char filename[100];
                    scanf("%s", filename);
                    if (endsWith(filename, ".pgn"))
                    {
                        gs.isPGN = 1; // PGN
                        reincepereJoc(&gs, filename, &gs.pgn, gs.isPGN); // PGN
                        continue;
                    }
                    else
                    {
                        printf("\nExtensie invalidă! Trebuie să fie .pgn\n");
                        continue;
                    }
                }
                // EXIT
                else if (strcmp(action, "exit") == 0)
                {
                    inchideJoc();
                }
                // JOC - MUTARI
                else
                {
                    char start[3], finish[3];
                    strcpy(start, action);
                    scanf("%s", finish);

                    int x1, y1, x2, y2;
                    transformareMiscare(start, finish, &x1, &y1, &x2, &y2, gs.culoare_ai);
                    int x1, y1, x2, y2;
                    transformareMiscare(start, finish, &x1, &y1, &x2, &y2, gs.culoare_ai);

                    int valid = validareMiscare(x1, y1, x2, y2, &gs);
                    int valid = validareMiscare(x1, y1, x2, y2, &gs);

                    if (valid == 1)
                    {
                        // SUNT IN SAH?
                        if (isInCheck(&gs, gs.currentPlayer))
                        {
                            if (!mutareIeseDinSah(x1, y1, x2, y2, &gs))
                            {
                                executa_mutare(x1, y1, x2, y2, &gs);
                                printTabla(gs.tabla, gs.culoare_ai);
                            }
                            else
                            {
                                printf("\nMutarea nu te scoate din șah!\n");
                                continue;
                            }
                        }
                        else
                        {
                            if (mutareIeseDinSah(x1, y1, x2, y2, &gs))
                            {
                                printf("\nMutarea te bagă în șah!\n");
                                continue;
                            }
                            else
                            {
                                executa_mutare(x1, y1, x2, y2, &gs);
                                printTabla(gs.tabla, gs.culoare_ai);
                            }
                        }
                        // ESTE SAH MAT?
                        if (isCheckmate(&gs))
                        {
                            printf("\n Sah mat! Jucătorul %s a câștigat!\n", gs.currentPlayer == 0 ? "alb" : "negru");
                            strcpy(result, gs.currentPlayer == 0 ? "1-0" : "0-1");
                            break;
                        }
                        else
                        {
                            if (esteRemiza(&gs))
                            {
                                printf("\nRemiză!\n");
                                strcpy(result, "1/2-1/2");
                                break;
                            }
                            else if (gs.isPGN)
                            {
                                gs.currentPlayer = !gs.currentPlayer;
                                gs.culoare_ai = gs.currentPlayer;
                            }
                            else
                            {
                                gs.currentPlayer = !gs.currentPlayer;
                            }
                        }
                    }
                    else if (valid == -1)
                    {
                        printf("Mutare invalida! Piesa nu apartine jucatorului curent.\n");
                        continue;
                    }
                    else
                    {
                        printf("Mutare invalida!\n");
                        continue;
                    }
                }
            }
        }
        else
        {
            gs.culoare_ai = gs.currentPlayer;

            // executa mutarile din fisierul PGN
                    if (valid == 1)
                    {
                        // SUNT IN SAH?
                        if (isInCheck(&gs, gs.currentPlayer))
                        {
                            if (!mutareIeseDinSah(x1, y1, x2, y2, &gs))
                            {
                                executa_mutare(x1, y1, x2, y2, &gs);
                                printTabla(gs.tabla, gs.culoare_ai);
                            }
                            else
                            {
                                printf("\nMutarea nu te scoate din șah!\n");
                                continue;
                            }
                        }
                        else
                        {
                            if (mutareIeseDinSah(x1, y1, x2, y2, &gs))
                            {
                                printf("\nMutarea te bagă în șah!\n");
                                continue;
                            }
                            else
                            {
                                executa_mutare(x1, y1, x2, y2, &gs);
                                printTabla(gs.tabla, gs.culoare_ai);
                            }
                        }
                        // ESTE SAH MAT?
                        if (isCheckmate(&gs))
                        {
                            printf("\n Sah mat! Jucătorul %s a câștigat!\n", gs.currentPlayer == 0 ? "alb" : "negru");
                            strcpy(result, gs.currentPlayer == 0 ? "1-0" : "0-1");
                            break;
                        }
                        else
                        {
                            if (esteRemiza(&gs))
                            {
                                printf("\nRemiză!\n");
                                strcpy(result, "1/2-1/2");
                                break;
                            }
                            else if (gs.isPGN)
                            {
                                gs.currentPlayer = !gs.currentPlayer;
                                gs.culoare_ai = gs.currentPlayer;
                            }
                            else
                            {
                                gs.currentPlayer = !gs.currentPlayer;
                            }
                        }
                    }
                    else if (valid == -1)
                    {
                        printf("Mutare invalida! Piesa nu apartine jucatorului curent.\n");
                        continue;
                    }
                    else
                    {
                        printf("Mutare invalida!\n");
                        continue;
                    }
                }
            }
        }
        else
        {
            gs.culoare_ai = gs.currentPlayer;

            // executa mutarile din fisierul PGN

            gs.currentPlayer = !gs.currentPlayer;
        }
    }
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
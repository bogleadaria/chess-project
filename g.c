#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"
#include "move_validation.h"
#include "ai.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define SQUARE_SIZE (SCREEN_WIDTH / 8)
#define MENU_WIDTH 400
#define MENU_HEIGHT 400
#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 50
#define BUTTON_MARGIN 20

int piece_char_to_index(char piece) {
    switch (piece) {
        case 'R': return 0; case 'D': return 1; case 'T': return 2;
        case 'C': return 3; case 'N': return 4; case 'P': return 5;
        case 'r': return 6; case 'd': return 7; case 't': return 8;
        case 'c': return 9; case 'n': return 10; case 'p': return 11;
        default: return -1;
    }
}

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Error loading image %s: %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void drawPiece(SDL_Renderer* renderer, int x, int y, char piece, SDL_Texture* textures[12]) {
    int idx = piece_char_to_index(piece);
    if (idx < 0 || !textures[idx]) return;
    SDL_Rect dst = { y * SQUARE_SIZE, x * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE };
    SDL_RenderCopy(renderer, textures[idx], NULL, &dst);
}

void drawAllPieces(SDL_Renderer* renderer, char tabla[8][8], SDL_Texture* textures[12], int drag_x, int drag_y, int dragging, int flip) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            int screen_row = flip ? 7 - i : i;
            int screen_col = flip ? 7 - j : j;
            if (!(dragging && i == drag_x && j == drag_y))
                drawPiece(renderer, screen_row, screen_col, tabla[i][j], textures);
        }
}

void drawChessboard(SDL_Renderer* renderer, int flip) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int screen_row = flip ? 7 - row : row;
            int screen_col = flip ? 7 - col : col;
            if ((screen_row + screen_col) % 2 == 0)
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
            else
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
            SDL_Rect square = {screen_col * SQUARE_SIZE, screen_row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            SDL_RenderFillRect(renderer, &square);
        }
    }
}

void drawButton(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, int w, int h, int hovered) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, hovered ? 180 : 120, hovered ? 180 : 120, hovered ? 255 : 200, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);

    SDL_Color color = {0, 0, 0};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int tw, th;
    SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
    SDL_Rect dst = {x + (w-tw)/2, y + (h-th)/2, tw, th};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int mouseInRect(int mx, int my, int x, int y, int w, int h) {
    return mx >= x && mx <= x+w && my >= y && my <= y+h;
}
// Draws a button and returns 1 if clicked, 0 otherwise
int drawButtonAndCheck(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, int w, int h, int mx, int my, int mouseDown) {
    int hovered = mx >= x && mx <= x+w && my >= y && my <= y+h;
    drawButton(renderer, font, text, x, y, w, h, hovered);
    return hovered && mouseDown;
}

// --- Graphical menu ---
void show_menu_and_start_game() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Chess Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MENU_WIDTH, MENU_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);

    int running = 1, menu_stage = 0, selected_mode = 0, selected_color = 0;
    int mx = -1, my = -1;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_MOUSEMOTION) {
                mx = event.motion.x;
                my = event.motion.y;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                mx = event.button.x;
                my = event.button.y;
                if (menu_stage == 0) {
                    for (int i = 0; i < 4; i++) {
                        int bx = (MENU_WIDTH-BUTTON_WIDTH)/2;
                        int by = 50 + i*(BUTTON_HEIGHT+BUTTON_MARGIN);
                        if (mouseInRect(mx, my, bx, by, BUTTON_WIDTH, BUTTON_HEIGHT)) {
                            selected_mode = i+1;
                            if (selected_mode == 1) menu_stage = 1;
                            else if (selected_mode == 2 || selected_mode == 3) running = 0;
                            else if (selected_mode == 4) { running = 0; selected_mode = 0; }
                        }
                    }
                } else if (menu_stage == 1) {
                    int bx = (MENU_WIDTH-BUTTON_WIDTH)/2;
                    int by1 = 120, by2 = 190;
                    if (mouseInRect(mx, my, bx, by1, BUTTON_WIDTH, BUTTON_HEIGHT)) {
                        selected_color = 0; // alb
                        running = 0;
                    }
                    if (mouseInRect(mx, my, bx, by2, BUTTON_WIDTH, BUTTON_HEIGHT)) {
                        selected_color = 1; // negru
                        running = 0;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        SDL_RenderClear(renderer);

        if (menu_stage == 0) {
            const char* labels[4] = {"Om vs AI", "Om vs Om", "AI vs AI", "Ieșire"};
            for (int i = 0; i < 4; i++) {
                int bx = (MENU_WIDTH-BUTTON_WIDTH)/2;
                int by = 50 + i*(BUTTON_HEIGHT+BUTTON_MARGIN);
                int hovered = (mx != -1 && my != -1 && mouseInRect(mx, my, bx, by, BUTTON_WIDTH, BUTTON_HEIGHT));
                drawButton(renderer, font, labels[i], bx, by, BUTTON_WIDTH, BUTTON_HEIGHT, hovered);
            }
        } else if (menu_stage == 1) {
            drawButton(renderer, font, "Alege culoarea: Alb", (MENU_WIDTH-BUTTON_WIDTH)/2, 120, BUTTON_WIDTH, BUTTON_HEIGHT, mx!=-1 && my!=-1 && mouseInRect(mx, my, (MENU_WIDTH-BUTTON_WIDTH)/2, 120, BUTTON_WIDTH, BUTTON_HEIGHT));
            drawButton(renderer, font, "Alege culoarea: Negru", (MENU_WIDTH-BUTTON_WIDTH)/2, 190, BUTTON_WIDTH, BUTTON_HEIGHT, mx!=-1 && my!=-1 && mouseInRect(mx, my, (MENU_WIDTH-BUTTON_WIDTH)/2, 190, BUTTON_WIDTH, BUTTON_HEIGHT));
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    if (selected_mode == 1) {
        visual(1, selected_color);
    } else if (selected_mode == 2) {
        visual(2, 0);
    } else if (selected_mode == 3) {
        visual(3, 0);
    }
}

// --- Visual chess game with board flipping and AI ---
void visual(int mode, int color) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Chess",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* piece_textures[12] = {NULL};
    const char* piece_files[12] = {
        "assets/albele/regele.png",   // R
        "assets/albele/regina.png",   // D
        "assets/albele/tura.png",     // T
        "assets/albele/cal.png",      // C
        "assets/albele/nebunul.png",  // N
        "assets/albele/pion.png",     // P
        "assets/negrele/regele.png",  // r
        "assets/negrele/regina.png",  // d
        "assets/negrele/tura.png",    // t
        "assets/negrele/cal.png",     // c
        "assets/negrele/nebunul.png", // n
        "assets/negrele/pion.png"     // p
    };
    for (int i = 0; i < 12; i++) {
        piece_textures[i] = loadTexture(piece_files[i], renderer);
    }

    GameState gs = initializeGame();
    gs.culoare_ai = (mode == 1) ? (color == 0 ? 1 : 0) : 0;
    gs.currentPlayer = 0;

    int dragging = 0, drag_x = -1, drag_y = -1;
    int running = 1;
    int mouse_x = 0, mouse_y = 0;
    SDL_Event event;

    int flip = (mode == 1 && color == 1); // flip if human vs AI and human is black
    int gameover = 0;
    char end_message[128] = "";

    while (running) {
        // --- AI MOVE LOGIC ---
        if (!gameover && (
            (mode == 1 && gs.currentPlayer == gs.culoare_ai) // Human vs AI, AI's turn
            || (mode == 3) // AI vs AI, always AI's turn
        )) {
            SDL_Delay(300);
            Move best = findBestMove(&gs, gs.currentPlayer);
            if (best.x1 != -1) {
                executa_mutare(best.x1, best.y1, best.x2, best.y2, &gs);
                gs.currentPlayer = !gs.currentPlayer;
            }
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;

            int human_turn = (mode == 2) || (mode == 1 && gs.currentPlayer != gs.culoare_ai);

            if (!gameover && human_turn) {
                // Start drag
                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    int row = event.button.y / SQUARE_SIZE;
                    int col = event.button.x / SQUARE_SIZE;
                    int board_row = flip ? 7 - row : row;
                    int board_col = flip ? 7 - col : col;
                    char piece = gs.tabla[board_row][board_col];
                    if (piece != ' ' &&
                        ((gs.currentPlayer == 0 && piece >= 'A' && piece <= 'Z') ||
                         (gs.currentPlayer == 1 && piece >= 'a' && piece <= 'z'))) {
                        dragging = 1;
                        drag_x = board_row;
                        drag_y = board_col;
                        mouse_x = event.button.x;
                        mouse_y = event.button.y;
                    }
                }
                // Dragging motion
                if (event.type == SDL_MOUSEMOTION && dragging) {
                    mouse_x = event.motion.x;
                    mouse_y = event.motion.y;
                }
                // Drop
                if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && dragging) {
                    int to_row = event.button.y / SQUARE_SIZE;
                    int to_col = event.button.x / SQUARE_SIZE;
                    int board_to_row = flip ? 7 - to_row : to_row;
                    int board_to_col = flip ? 7 - to_col : to_col;
                    // Only allow moves that get out of check
                    if (validareMiscare(drag_x, drag_y, board_to_row, board_to_col, &gs)) {
                        // Simulate the move
                        GameState gs_copy = gs;
                        executa_mutare(drag_x, drag_y, board_to_row, board_to_col, &gs_copy);
                        if (!isInCheck(&gs_copy, gs.currentPlayer)) {
                            executa_mutare(drag_x, drag_y, board_to_row, board_to_col, &gs);
                            gs.currentPlayer = !gs.currentPlayer;
                        }
                        // else: illegal move, do nothing
                    }
                    dragging = 0;
                    drag_x = drag_y = -1;
                }
            }
        }

        // --- ENDGAME CHECKS ---
        if (!gameover) {
            if (isCheckmate(&gs)) {
                snprintf(end_message, sizeof(end_message), "Șah mat! %s câștigă!", gs.currentPlayer ? "Alb" : "Negru");
                gameover = 1;
            } else if (!existaMutareLegala(&gs)) {
                if (isInCheck(&gs, gs.currentPlayer)) {
                    snprintf(end_message, sizeof(end_message), "Șah mat! %s câștigă!", gs.currentPlayer ? "Alb" : "Negru");
                } else {
                    snprintf(end_message, sizeof(end_message), "Remiză (pat)!");
                }
                gameover = 1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        drawChessboard(renderer, flip);
        drawAllPieces(renderer, gs.tabla, piece_textures, drag_x, drag_y, dragging, flip);

        // Draw dragged piece following mouse
        if (dragging && gs.tabla[drag_x][drag_y] != ' ') {
            int idx = piece_char_to_index(gs.tabla[drag_x][drag_y]);
            if (idx >= 0 && piece_textures[idx]) {
                int screen_row = flip ? 7 - drag_x : drag_x;
                int screen_col = flip ? 7 - drag_y : drag_y;
                int draw_x = mouse_x - SQUARE_SIZE / 2;
                int draw_y = mouse_y - SQUARE_SIZE / 2;
                SDL_Rect dst = { draw_x, draw_y, SQUARE_SIZE, SQUARE_SIZE };
                SDL_RenderCopy(renderer, piece_textures[idx], NULL, &dst);
            }
        }

        // Draw end message if game is over
        if (gameover && end_message[0]) {
    int to_menu = 0, quit = 0;
    int mx = -1, my = -1, mouseDown = 0;
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 36);

    while (!to_menu && !quit) {
        SDL_Event e;
        mouseDown = 0;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;
            if (e.type == SDL_MOUSEMOTION) {
                mx = e.motion.x;
                my = e.motion.y;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                mouseDown = 1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);
        drawChessboard(renderer, flip);
        drawAllPieces(renderer, gs.tabla, piece_textures, -1, -1, 0, flip);

        // Draw end message
        SDL_Color color = {255, 0, 0};
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, end_message, color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        int tw, th;
        SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
        SDL_Rect dst = { (SCREEN_WIDTH-tw)/2, 100, tw, th };
        SDL_RenderCopy(renderer, texture, NULL, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        // Draw buttons
        int btn_w = 200, btn_h = 60;
        int btn_x = (SCREEN_WIDTH-btn_w)/2;
        int btn_y1 = 300, btn_y2 = 400;
        if (drawButtonAndCheck(renderer, font, "Menu", btn_x, btn_y1, btn_w, btn_h, mx, my, mouseDown)) to_menu = 1;
        if (drawButtonAndCheck(renderer, font, "Exit", btn_x, btn_y2, btn_w, btn_h, mx, my, mouseDown)) quit = 1;

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    TTF_CloseFont(font);

    if (to_menu) {
    // Clean up everything before going to menu
    for (int i = 0; i < 12; i++) {
        if (piece_textures[i]) SDL_DestroyTexture(piece_textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    // Go to menu (restart program)
    show_menu_and_start_game();
    // After returning from menu, exit this game instance
    return;
}
    // If quit, just fall through and cleanup
    break;
}

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    for (int i = 0; i < 12; i++) {
        if (piece_textures[i]) SDL_DestroyTexture(piece_textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
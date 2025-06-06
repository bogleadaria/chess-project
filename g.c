#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "game.h"
#include "move_validation.h"
#include "ai.h"
#include <string.h>

#define SCREEN_WIDTH 1100
#define SCREEN_HEIGHT 800
#define BOARD_WIDTH 800
#define PANEL_WIDTH (SCREEN_WIDTH - BOARD_WIDTH)
#define SQUARE_SIZE (BOARD_WIDTH / 8)
#define MENU_WIDTH 400
#define MENU_HEIGHT 400
#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 50
#define BUTTON_MARGIN 20

// --- Utility ---
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

void format_pgn_for_panel(const char* pgn_moves, char* out, size_t outlen, int moves_per_line) {
    out[0] = 0;
    int move_count = 0;
    char moves_copy[4096];
    strncpy(moves_copy, pgn_moves, sizeof(moves_copy)-1);
    moves_copy[sizeof(moves_copy)-1] = 0;
    char *token = strtok(moves_copy, " \n\r\t");
    while (token) {
        strcat(out, token);
        strcat(out, " ");
        move_count++;
        if (move_count % moves_per_line == 0)
            strcat(out, "\n");
        token = strtok(NULL, " \n\r\t");
        if (strlen(out) > outlen - 64) break; // avoid overflow
    }
    out[outlen-1] = 0;
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

// --- PGN text loader ---
void loadPGNText(const char* filename, char* buf, size_t buflen) {
    FILE* f = fopen(filename, "r");
    if (!f) { buf[0] = 0; return; }
    buf[0] = 0;
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '[') continue; // skip header
        strncat(buf, line, buflen - strlen(buf) - 1);
    }
    fclose(f);
}

// --- Draws the right panel, always visible, with extra buttons and PGN scroll ---
void drawSidePanel(SDL_Renderer* renderer, TTF_Font* font, const char* end_message, int gameover, int mx, int my, int mouseDown,
                   int* to_menu, int* quit, int* pgn_left, int* pgn_right, int* pgn_save, int* pgn_load,
                   const char* pgn_moves, int pgn_scroll) {
    SDL_Rect panel = {BOARD_WIDTH, 0, PANEL_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    SDL_RenderFillRect(renderer, &panel);

    // End message at the top if game is over
    if (gameover && end_message && end_message[0]) {
        SDL_Color color = {255, 0, 0};
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, end_message, color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        int tw, th;
        SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
        SDL_Rect dst = {BOARD_WIDTH + (PANEL_WIDTH-tw)/2, 20, tw, th};
        SDL_RenderCopy(renderer, texture, NULL, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // --- Keyboard shortcut labels (smaller font, at the very top) ---
    TTF_Font* small_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 18);
    SDL_Color label_color = {0, 0, 80};
    const char* labels[] = {
        "f - Save game (FEN)",
        "l - Load game (FEN)",
        "p - Save game (PGN)",
        "o - Load game (PGN)"
    };
    int label_y = 20; // Start higher up
    for (int i = 0; i < 4; ++i) {
        SDL_Surface* surf = TTF_RenderUTF8_Blended(small_font, labels[i], label_color);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        int tw, th;
        SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
        SDL_Rect dst = {BOARD_WIDTH + 40, label_y, tw, th};
        SDL_RenderCopy(renderer, tex, NULL, &dst);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
        label_y += th + 4;
    }
    if (small_font) TTF_CloseFont(small_font);

    // Move everything else lower to make space for the shortcut text
    int offset = label_y + 16; // Add some extra space after shortcuts

    // Main buttons (Menu/Exit)
    int btn_w = PANEL_WIDTH - 40, btn_h = 50;
    int btn_x = BOARD_WIDTH + 20;
    int btn_y1 = offset;
    int btn_y2 = btn_y1 + btn_h + 20;
    int menu_hover = mouseInRect(mx, my, btn_x, btn_y1, btn_w, btn_h);
    int exit_hover = mouseInRect(mx, my, btn_x, btn_y2, btn_w, btn_h);

    drawButton(renderer, font, "Menu", btn_x, btn_y1, btn_w, btn_h, menu_hover);
    drawButton(renderer, font, "Exit", btn_x, btn_y2, btn_w, btn_h, exit_hover);

    if (menu_hover && mouseDown) *to_menu = 1;
    if (exit_hover && mouseDown) *quit = 1;

    // --- Only show PGN panel if pgn_moves is not empty ---
    if (pgn_moves && pgn_moves[0]) {
        // PGN/Replay area
        int pgn_area_y = btn_y2 + btn_h + 20;
        SDL_Rect pgn_area = {BOARD_WIDTH + 20, pgn_area_y, btn_w, 500};
        SDL_SetRenderDrawColor(renderer, 200, 200, 240, 255);
        SDL_RenderFillRect(renderer, &pgn_area);

        // PGN navigation buttons
        int nav_btn_size = 40;
        int nav_y = pgn_area_y + 10;
        int nav_x_left = BOARD_WIDTH + 30;
        int nav_x_right = BOARD_WIDTH + PANEL_WIDTH - 30 - nav_btn_size;

        int left_hover = mouseInRect(mx, my, nav_x_left, nav_y, nav_btn_size, nav_btn_size);
        int right_hover = mouseInRect(mx, my, nav_x_right, nav_y, nav_btn_size, nav_btn_size);

        drawButton(renderer, font, "◀", nav_x_left, nav_y, nav_btn_size, nav_btn_size, left_hover);
        drawButton(renderer, font, "▶", nav_x_right, nav_y, nav_btn_size, nav_btn_size, right_hover);

        if (left_hover && mouseDown) *pgn_left = 1;
        if (right_hover && mouseDown) *pgn_right = 1;

        // Draw PGN moves text with scrolling
 SDL_Color pgn_color = {0, 0, 80};
        SDL_Surface* pgn_surface = TTF_RenderUTF8_Blended_Wrapped(font, pgn_moves, pgn_color, btn_w - 20);
        SDL_Texture* pgn_texture = SDL_CreateTextureFromSurface(renderer, pgn_surface);
        int pgn_tw = pgn_surface->w, pgn_th = pgn_surface->h;

        int pgn_area_height = 500;
        int effective_scroll = 0;
        if (pgn_th > pgn_area_height) {
            if (pgn_scroll > pgn_th - pgn_area_height)
                pgn_scroll = pgn_th - pgn_area_height;
            if (pgn_scroll < 0)
                pgn_scroll = 0;
            effective_scroll = pgn_scroll;
        }

        SDL_Rect pgn_src = {0, effective_scroll, btn_w - 20, pgn_area_height};
        SDL_Rect pgn_dst = {BOARD_WIDTH + 30, nav_y + nav_btn_size + 20, btn_w - 20, pgn_area_height};
        SDL_RenderSetClipRect(renderer, &pgn_area);
        // Only draw as much as the text actually is tall
        if (pgn_th < pgn_area_height) {
            pgn_src.h = pgn_th;
            pgn_dst.h = pgn_th;
        }
        SDL_RenderCopy(renderer, pgn_texture, &pgn_src, &pgn_dst);
        SDL_RenderSetClipRect(renderer, NULL);
        SDL_FreeSurface(pgn_surface);
        SDL_DestroyTexture(pgn_texture);
    }
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
            const char* labels[3] = {"Om vs AI", "Om vs Om"/*, "AI vs AI"*/, "Ieșire"};
            for (int i = 0; i < 3; i++) {
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

// --- Main visual function ---
void visual(int mode, int color) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

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
    int is_check = 0;

    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 28);

    int to_menu = 0, quit = 0;

    // --- PGN navigation state ---
    char pgn_moves[4096] = "";
    loadPGNText("game.pgn", pgn_moves, sizeof(pgn_moves));
    int pgn_nav_index = -1; // -1 means not in navigation mode
    int pgn_total_moves = 0;
    int pgn_scroll = 0;
    {
        char moves_copy[4096];
        strcpy(moves_copy, pgn_moves);
        char *token = strtok(moves_copy, " \n\r\t");
        while (token) {
            if (strchr(token, '.') == NULL && strcmp(token, "1-0") && strcmp(token, "0-1") && strcmp(token, "1/2-1/2") && strcmp(token, "*")) {
                pgn_total_moves++;
            }
            token = strtok(NULL, " \n\r\t");
        }
    }

    while (running && !to_menu && !quit) {
        // --- AI MOVE LOGIC ---
        if (!gameover && (
            (mode == 1 && gs.currentPlayer == gs.culoare_ai) // Human vs AI, AI's turn
            /*|| (mode == 3)*/ // AI vs AI, always AI's turn
        ) && pgn_nav_index == -1) { // Only allow AI to move if not in PGN replay
            SDL_Delay(300);
            Move nimic = { .x1 = -1, .y1 = -1, .x2 = -1, .y2 = -1, .scor = 0.0 };
            Move best = findBestMove(&gs, gs.culoare_ai, nimic);
            if (best.x1 != -1 && !mutareIeseDinSah(best.x1, best.y1, best.x2, best.y2, &gs)) {
                executa_mutare(best.x1, best.y1, best.x2, best.y2, &gs);
                gs.currentPlayer = !gs.currentPlayer;
            }
        }

        int mx = -1, my = -1, mouseDown = 0;
        int pgn_left = 0, pgn_right = 0, pgn_save = 0, pgn_load = 0;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = 1;

            int human_turn = (mode == 2) || (mode == 1 && gs.currentPlayer != gs.culoare_ai);

            if (!gameover && human_turn && pgn_nav_index == -1) {
                // Start drag
                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    int row = event.button.y / SQUARE_SIZE;
                    int col = event.button.x / SQUARE_SIZE;
                    if (col < 8) { // Only allow drag from board, not panel
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
                    if (to_col < 8) { // Only allow drop on board
                        int board_to_row = flip ? 7 - to_row : to_row;
                        int board_to_col = flip ? 7 - to_col : to_col;
                        if (validareMiscare(drag_x, drag_y, board_to_row, board_to_col, &gs)) {
                            if (!mutareIeseDinSah(drag_x, drag_y, board_to_row, board_to_col, &gs)) {
                                executa_mutare(drag_x, drag_y, board_to_row, board_to_col, &gs);
                                gs.currentPlayer = !gs.currentPlayer;
                                pgn_nav_index = -1; // Exit navigation mode after a move
                            }
                        }
                    }
                    dragging = 0;
                    drag_x = drag_y = -1;
                }
            }

            // Always track mouse for panel buttons
            if (event.type == SDL_MOUSEMOTION) {
                mx = event.motion.x;
                my = event.motion.y;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                mx = event.button.x;
                my = event.button.y;
                mouseDown = 1;
            }

            // --- KEYBOARD SHORTCUTS FOR SAVE/LOAD ---
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_f) {
                    salveazaJocFEN(&gs, "game.fen");
                }
                if (event.key.keysym.sym == SDLK_p) {
                    salveazaJocPGN(&gs, "game.pgn");
                    loadPGNText("game.pgn", pgn_moves, sizeof(pgn_moves));
                    // Recount moves
                    char moves_copy[4096];
                    strcpy(moves_copy, pgn_moves);
                    pgn_total_moves = 0;
                    char *token = strtok(moves_copy, " \n\r\t");
                    while (token) {
                        if (strchr(token, '.') == NULL && strcmp(token, "1-0") && strcmp(token, "0-1") && strcmp(token, "1/2-1/2") && strcmp(token, "*")) {
                            pgn_total_moves++;
                        }
                        token = strtok(NULL, " \n\r\t");
                    }
                    pgn_nav_index = -1;
                }
                if (event.key.keysym.sym == SDLK_l) {
                    reincepereJoc(&gs, "game.fen", &gs.pgn, 0);
                    pgn_nav_index = -1;
                }
                if (event.key.keysym.sym == SDLK_o) {
                    reincepereJoc(&gs, "game.pgn", &gs.pgn, 1);
                    loadPGNText("game.pgn", pgn_moves, sizeof(pgn_moves));
                    // Recount moves
                    char moves_copy[4096];
                    strcpy(moves_copy, pgn_moves);
                    pgn_total_moves = 0;
                    char *token = strtok(moves_copy, " \n\r\t");
                    while (token) {
                        if (strchr(token, '.') == NULL && strcmp(token, "1-0") && strcmp(token, "0-1") && strcmp(token, "1/2-1/2") && strcmp(token, "*")) {
                            pgn_total_moves++;
                        }
                        token = strtok(NULL, " \n\r\t");
                    }
                    pgn_nav_index = -1;
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    pgn_scroll -= 30;
                    if (pgn_scroll < 0) pgn_scroll = 0;
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    pgn_scroll += 30;
                }
            }
            if (event.type == SDL_MOUSEWHEEL) {
                pgn_scroll -= event.wheel.y * 30;
                if (pgn_scroll < 0) pgn_scroll = 0;
            }
        }

        // --- PGN panel button actions ---
        if (pgn_save) {
            salveazaJocPGN(&gs, "game.pgn");
            loadPGNText("game.pgn", pgn_moves, sizeof(pgn_moves));
            // Recount moves
            char moves_copy[4096];
            strcpy(moves_copy, pgn_moves);
            pgn_total_moves = 0;
            char *token = strtok(moves_copy, " \n\r\t");
            while (token) {
                if (strchr(token, '.') == NULL && strcmp(token, "1-0") && strcmp(token, "0-1") && strcmp(token, "1/2-1/2") && strcmp(token, "*")) {
                    pgn_total_moves++;
                }
                token = strtok(NULL, " \n\r\t");
            }
            pgn_nav_index = -1;
            pgn_save = 0;
        }
        if (pgn_load) {
            reincepereJoc(&gs, "game.pgn", &gs.pgn, 1);
            loadPGNText("game.pgn", pgn_moves, sizeof(pgn_moves));
            // Recount moves
            char moves_copy[4096];
            strcpy(moves_copy, pgn_moves);
            pgn_total_moves = 0;
            char *token = strtok(moves_copy, " \n\r\t");
            while (token) {
                if (strchr(token, '.') == NULL && strcmp(token, "1-0") && strcmp(token, "0-1") && strcmp(token, "1/2-1/2") && strcmp(token, "*")) {
                    pgn_total_moves++;
                }
                token = strtok(NULL, " \n\r\t");
            }
            pgn_nav_index = -1;
            pgn_load = 0;
        }
        // PGN navigation
        if (pgn_left) {
            if (pgn_nav_index == -1) pgn_nav_index = 0; // Enter navigation mode from start
            else if (pgn_nav_index > 0) pgn_nav_index--;
            pgn_left = 0;
        }
        if (pgn_right) {
            if (pgn_nav_index == -1) pgn_nav_index = 0; // Enter navigation mode from start
            else if (pgn_nav_index < pgn_total_moves) pgn_nav_index++;
            pgn_right = 0;
        }

        // --- PGN replay logic ---
        if (pgn_nav_index != -1) {
            gs = initializeGame();
            gs.pgn = initializarePGN();
            gs.currentPlayer = 0;
            // Parse PGN and play up to pgn_nav_index moves
            char moves_copy[4096];
            strcpy(moves_copy, pgn_moves);
            char *token = strtok(moves_copy, " \n\r\t");
            int move_count = 0;
            while (token && move_count < pgn_nav_index) {
                if (strchr(token, '.') == NULL && strcmp(token, "1-0") && strcmp(token, "0-1") && strcmp(token, "1/2-1/2") && strcmp(token, "*")) {
                    // e2e4 format
                    if (strlen(token) >= 4 && isalpha(token[0]) && isdigit(token[1]) && isalpha(token[2]) && isdigit(token[3])) {
                        int y1 = token[0] - 'a';
                        int x1 = 8 - (token[1] - '0');
                        int y2 = token[2] - 'a';
                        int x2 = 8 - (token[3] - '0');
                        if (validareMiscare(x1, y1, x2, y2, &gs) &&
                            !mutareIeseDinSah(x1, y1, x2, y2, &gs)) {
                            executa_mutare(x1, y1, x2, y2, &gs);
                            gs.currentPlayer = !gs.currentPlayer;
                        }
                    }
                    move_count++;
                }
                token = strtok(NULL, " \n\r\t");
            }
            gameover = 0;
            end_message[0] = 0;
        }

        // --- CHECK/CHECKMATE/RESULT LOGIC ---
        is_check = isInCheck(&gs, gs.currentPlayer);
        if (!gameover) {
            if (isCheckmate(&gs)) {
                snprintf(end_message, sizeof(end_message), "Șah mat! %s câștigă!", gs.currentPlayer ? "Alb" : "Negru");
                strcpy(gs.pgn.result, gs.currentPlayer ? "1-0" : "0-1");
                gameover = 1;
            } else if (!existaMutareLegala(&gs)) {
                if (isInCheck(&gs, gs.currentPlayer)) {
                    snprintf(end_message, sizeof(end_message), "Șah mat! %s câștigă!", gs.currentPlayer ? "Alb" : "Negru");
                    strcpy(gs.pgn.result, gs.currentPlayer ? "1-0" : "0-1");
                } else {
                    snprintf(end_message, sizeof(end_message), "Remiză (pat)!");
                    strcpy(gs.pgn.result, "1/2-1/2");
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

        // Optionally: show "Șah!" in the panel if is_check
        if (is_check && !gameover) {
            SDL_Color color = {255, 0, 0};
            SDL_Surface* surface = TTF_RenderUTF8_Blended(font, "Șah!", color);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            int tw, th;
            SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
            SDL_Rect dst = {BOARD_WIDTH + (PANEL_WIDTH-tw)/2, 10, tw, th};
            SDL_RenderCopy(renderer, texture, NULL, &dst);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
        char formatted_pgn[2048];
        format_pgn_for_panel(pgn_moves, formatted_pgn, sizeof(formatted_pgn), 10);
        drawSidePanel(renderer, font, end_message, gameover, mx, my, mouseDown, &to_menu, &quit,
                      &pgn_left, &pgn_right, &pgn_save, &pgn_load, formatted_pgn, pgn_scroll);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);

    for (int i = 0; i < 12; i++) {
        if (piece_textures[i]) SDL_DestroyTexture(piece_textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    if (to_menu) {
        show_menu_and_start_game();
    }
}
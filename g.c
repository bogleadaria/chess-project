#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "game.h"
#include "move_validation.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define SQUARE_SIZE (SCREEN_WIDTH / 8)

// Map piece character to texture index
int piece_char_to_index(char piece) {
    switch (piece) {
        case 'R': return 0; case 'D': return 1; case 'T': return 2;
        case 'C': return 3; case 'N': return 4; case 'P': return 5;
        case 'r': return 6; case 'd': return 7; case 't': return 8;
        case 'c': return 9; case 'n': return 10; case 'p': return 11;
        default: return -1;
    }
}

// Load a PNG as a texture
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

// Draw a piece using textures
void drawPiece(SDL_Renderer* renderer, int x, int y, char piece, SDL_Texture* textures[12]) {
    int idx = piece_char_to_index(piece);
    if (idx < 0 || !textures[idx]) return;
    SDL_Rect dst = { y * SQUARE_SIZE, x * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE };
    SDL_RenderCopy(renderer, textures[idx], NULL, &dst);
}

// Draw all pieces except the one being dragged
void drawAllPieces(SDL_Renderer* renderer, char tabla[8][8], SDL_Texture* textures[12], int drag_x, int drag_y, int dragging) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (!(dragging && i == drag_x && j == drag_y))
                drawPiece(renderer, i, j, tabla[i][j], textures);
}

// Draw the chessboard
void drawChessboard(SDL_Renderer* renderer) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            SDL_Rect square = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            if ((row + col) % 2 == 0)
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
            else
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
            SDL_RenderFillRect(renderer, &square);
        }
    }
}

void visual() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Chess",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load textures for all pieces
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
    int dragging = 0, drag_x = -1, drag_y = -1;
    int running = 1;
    int mouse_x = 0, mouse_y = 0;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;

            // Start drag
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int row = event.button.y / SQUARE_SIZE;
                int col = event.button.x / SQUARE_SIZE;
                char piece = gs.tabla[row][col];
                if (piece != ' ' &&
                    ((gs.currentPlayer == 0 && piece >= 'A' && piece <= 'Z') ||
                     (gs.currentPlayer == 1 && piece >= 'a' && piece <= 'z'))) {
                    dragging = 1;
                    drag_x = row;
                    drag_y = col;
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
                if (validareMiscare(drag_x, drag_y, to_row, to_col, &gs)) {
                    executa_mutare(drag_x, drag_y, to_row, to_col, &gs);
                    gs.currentPlayer = !gs.currentPlayer;
                }
                dragging = 0;
                drag_x = drag_y = -1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        drawChessboard(renderer);
        drawAllPieces(renderer, gs.tabla, piece_textures, drag_x, drag_y, dragging);

        // Draw dragged piece following mouse
        if (dragging && gs.tabla[drag_x][drag_y] != ' ') {
            int idx = piece_char_to_index(gs.tabla[drag_x][drag_y]);
            if (idx >= 0 && piece_textures[idx]) {
                SDL_Rect dst = { mouse_x - SQUARE_SIZE / 2, mouse_y - SQUARE_SIZE / 2, SQUARE_SIZE, SQUARE_SIZE };
                SDL_RenderCopy(renderer, piece_textures[idx], NULL, &dst);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Cleanup
    for (int i = 0; i < 12; i++) {
        if (piece_textures[i]) SDL_DestroyTexture(piece_textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
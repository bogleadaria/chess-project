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
        case 'R': return 0; // White King
        case 'D': return 1; // White Queen
        case 'T': return 2; // White Rook
        case 'C': return 3; // White Knight
        case 'N': return 4; // White Bishop
        case 'P': return 5; // White Pawn
        case 'r': return 6; // Black King
        case 'd': return 7; // Black Queen
        case 't': return 8; // Black Rook
        case 'c': return 9; // Black Knight
        case 'n': return 10; // Black Bishop
        case 'p': return 11; // Black Pawn
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

// Draw all pieces
void drawAllPieces(SDL_Renderer* renderer, char tabla[8][8], SDL_Texture* textures[12]) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
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
    int selected = 0, sel_x = -1, sel_y = -1;
    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mx = event.button.y / SQUARE_SIZE;
                int my = event.button.x / SQUARE_SIZE;
                if (!selected && gs.tabla[mx][my] != ' ' &&
                    ((gs.currentPlayer == 0 && gs.tabla[mx][my] >= 'A' && gs.tabla[mx][my] <= 'Z') ||
                     (gs.currentPlayer == 1 && gs.tabla[mx][my] >= 'a' && gs.tabla[mx][my] <= 'z'))) {
                    sel_x = mx; sel_y = my; selected = 1;
                } else if (selected) {
                    if (validareMiscare(sel_x, sel_y, mx, my, &gs)) {
                        executa_mutare(sel_x, sel_y, mx, my, &gs);
                        gs.currentPlayer = !gs.currentPlayer;
                    }
                    selected = 0;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        drawChessboard(renderer);
        drawAllPieces(renderer, gs.tabla, piece_textures);

        // Highlight selected square
        if (selected) {
            SDL_Rect r = {sel_y * SQUARE_SIZE, sel_x * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
            SDL_RenderFillRect(renderer, &r);
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
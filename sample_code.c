#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define SQUARE_SIZE (SCREEN_WIDTH / 8)

// Function to load images
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Error loading image: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void drawChessboard(SDL_Renderer* renderer) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            SDL_Rect square = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};

            // Alternate colors
            if ((row + col) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
            }

            SDL_RenderFillRect(renderer, &square);
        }
    }
}

void drawPieces(SDL_Renderer* renderer, SDL_Texture* pieces[2][6]) {
    // Simple chess starting setup (just an example)
    int board[8][8] = {
        {4, 3, 2, 5, 6, 2, 3, 4}, // Black pieces
        {1, 1, 1, 1, 1, 1, 1, 1}, // Black pawns
        {0, 0, 0, 0, 0, 0, 0, 0}, // Empty
        {0, 0, 0, 0, 0, 0, 0, 0}, // Empty
        {0, 0, 0, 0, 0, 0, 0, 0}, // Empty
        {0, 0, 0, 0, 0, 0, 0, 0}, // Empty
        {1, 1, 1, 1, 1, 1, 1, 1}, // White pawns
        {4, 3, 2, 5, 6, 2, 3, 4}  // White pieces
    };

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int piece = board[row][col];
            if (piece > 0) {
                int color = (row < 2) ? 0 : 1; // Black (0) or White (1)
                SDL_Rect destRect = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
                SDL_RenderCopy(renderer, pieces[color][piece - 1], NULL, &destRect);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    //IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Chess",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load chess pieces
    SDL_Texture* pieces[2][6]; // 2 colors, 6 types
    const char* filenames[2][6] = {
        {"assets/b_pawn.png", "assets/b_knight.png", "assets/b_bishop.png",
         "assets/b_rook.png", "assets/b_queen.png", "assets/b_king.png"},
        {"assets/w_pawn.png", "assets/w_knight.png", "assets/w_bishop.png",
         "assets/w_rook.png", "assets/w_queen.png", "assets/w_king.png"}
    };

    for (int color = 0; color < 2; color++) {
        for (int type = 0; type < 6; type++) {
            pieces[color][type] = loadTexture(filenames[color][type], renderer);
        }
    }

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        drawChessboard(renderer);
        drawPieces(renderer, pieces);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    for (int color = 0; color < 2; color++) {
        for (int type = 0; type < 6; type++) {
            SDL_DestroyTexture(pieces[color][type]);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
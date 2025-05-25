#include <stdio.h>
#include <SDL2/SDL.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define SQUARE_SIZE (SCREEN_WIDTH / 8)

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

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Chessboard",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Background color (gray)
        SDL_RenderClear(renderer);
        
        drawChessboard(renderer);
        
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
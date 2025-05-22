#include <SDL3/SDL.h>
#include <stdbool.h>

#define WINDOW_SIZE 640
#define BOARD_SIZE 8
#define SQUARE_SIZE (WINDOW_SIZE / BOARD_SIZE)

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Error initializing SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Chess Project (SDL3)",
        WINDOW_SIZE, WINDOW_SIZE,
        SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        SDL_Log("Error creating window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, 0);
    if (!renderer) {
        SDL_Log("Error creating renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // Draw chessboard
        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                int is_white = ((x + y) % 2 == 0);
                SDL_SetRenderDrawColor(renderer,
                    is_white ? 240 : 80,
                    is_white ? 217 : 80,
                    is_white ? 181 : 80,
                    255
                );
                SDL_FRect rect = {
                    .x = x * SQUARE_SIZE,
                    .y = y * SQUARE_SIZE,
                    .w = SQUARE_SIZE,
                    .h = SQUARE_SIZE
                };
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
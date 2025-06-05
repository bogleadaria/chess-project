#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <stdio.h>

#define MENU_WIDTH 400
#define MENU_HEIGHT 400
#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 50
#define BUTTON_MARGIN 20

typedef enum {
    MENU_NONE,
    MENU_HUMAN_AI,
    MENU_HUMAN_HUMAN,
    MENU_AI_AI,
    MENU_EXIT
} MenuChoice;

typedef enum {
    COLOR_NONE,
    COLOR_WHITE,
    COLOR_BLACK
} ColorChoice;

// Helper to draw a button with text
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

// Returns 1 if mouse is inside rect
int mouseInRect(int mx, int my, int x, int y, int w, int h) {
    return mx >= x && mx <= x+w && my >= y && my <= y+h;
}

// Forward declaration
void visual(int mode, int color); // mode: 1=Human vs AI, 2=Human vs Human, 3=AI vs AI; color: 0=alb, 1=negru

void show_menu() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Chess Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MENU_WIDTH, MENU_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);

    int running = 1;
    MenuChoice choice = MENU_NONE;
    ColorChoice color = COLOR_NONE;

    while (running) {
        SDL_Event event;
        int mx = -1, my = -1;
        int hover[4] = {0,0,0,0};

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_MOUSEMOTION) {
                mx = event.motion.x;
                my = event.motion.y;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                mx = event.button.x;
                my = event.button.y;
                if (choice == MENU_NONE) {
                    // Main menu
                    for (int i = 0; i < 4; i++) {
                        int bx = (MENU_WIDTH-BUTTON_WIDTH)/2;
                        int by = 50 + i*(BUTTON_HEIGHT+BUTTON_MARGIN);
                        if (mouseInRect(mx, my, bx, by, BUTTON_WIDTH, BUTTON_HEIGHT)) {
                            choice = (MenuChoice)(i+1);
                            if (choice == MENU_EXIT) running = 0;
                            break;
                        }
                    }
                } else if (choice == MENU_HUMAN_AI && color == COLOR_NONE) {
                    // Color selection
                    int bx = (MENU_WIDTH-BUTTON_WIDTH)/2;
                    int by1 = 120, by2 = 190;
                    if (mouseInRect(mx, my, bx, by1, BUTTON_WIDTH, BUTTON_HEIGHT)) {
                        color = COLOR_WHITE;
                        running = 0;
                    }
                    if (mouseInRect(mx, my, bx, by2, BUTTON_WIDTH, BUTTON_HEIGHT)) {
                        color = COLOR_BLACK;
                        running = 0;
                    }
                } else {
                    running = 0;
                }
            }
        }

        // Draw
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        SDL_RenderClear(renderer);

        if (choice == MENU_NONE) {
            const char* labels[4] = {"Om vs AI", "Om vs Om", "AI vs AI", "Ieșire"};
            for (int i = 0; i < 4; i++) {
                int bx = (MENU_WIDTH-BUTTON_WIDTH)/2;
                int by = 50 + i*(BUTTON_HEIGHT+BUTTON_MARGIN);
                if (mx != -1 && my != -1 && mouseInRect(mx, my, bx, by, BUTTON_WIDTH, BUTTON_HEIGHT))
                    hover[i] = 1;
                drawButton(renderer, font, labels[i], bx, by, BUTTON_WIDTH, BUTTON_HEIGHT, hover[i]);
            }
        } else if (choice == MENU_HUMAN_AI && color == COLOR_NONE) {
            drawButton(renderer, font, "Alege culoarea: Alb", (MENU_WIDTH-BUTTON_WIDTH)/2, 120, BUTTON_WIDTH, BUTTON_HEIGHT, mx!=-1 && my!=-1 && mouseInRect(mx, my, (MENU_WIDTH-BUTTON_WIDTH)/2, 120, BUTTON_WIDTH, BUTTON_HEIGHT));
            drawButton(renderer, font, "Alege culoarea: Negru", (MENU_WIDTH-BUTTON_WIDTH)/2, 190, BUTTON_WIDTH, BUTTON_HEIGHT, mx!=-1 && my!=-1 && mouseInRect(mx, my, (MENU_WIDTH-BUTTON_WIDTH)/2, 190, BUTTON_WIDTH, BUTTON_HEIGHT));
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Launch the chosen game mode
    if (choice == MENU_HUMAN_AI && color != COLOR_NONE) {
        visual(1, color == COLOR_WHITE ? 0 : 1);
    } else if (choice == MENU_HUMAN_HUMAN) {
        visual(2, 0);
    } else if (choice == MENU_AI_AI) {
        visual(3, 0);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

all:
	gcc -Wall -I. -Ipieces main.c g.c game.c move_validation.c ai.c pieces/*.c -lm -o chess_game -lSDL2 -lSDL2_image -lSDL2_ttf && ./chess_game
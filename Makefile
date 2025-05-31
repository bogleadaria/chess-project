# 	gcc -Wall -lsdDL2 -I. -Ipieces main.c game.c move_validation.c ai.c pieces/*.c -lm -o chess_game && ./chess_game
clear:
	rm -f chess_game

all:
	gcc -Wall -lSDL2 -lSDL2_image -I. -Ipieces main.c game.c move_validation.c ai.c pieces/*.c -lm -o chess_game && ./chess_game

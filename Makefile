all:
	gcc -Wall -I. -Ipieces main.c game.c g.c move_validation.c pieces/*.c -lSDL2 -lSDL2_image -o chess_game && ./chess_game
clear:
	rm -f chess_game
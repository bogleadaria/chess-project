clear:
	rm -f chess_game
all:
	gcc -Wall -I. -Ipieces main.c game.c move_validation.c ai.c pieces/*.c -lm -o chess_game && ./chess_game	
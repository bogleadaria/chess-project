# all:
# 	gcc -Wall -lSDL2 -I. -Ipieces main.c game.c move_validation.c ai.c pieces/*.c -o chess_game && ./chess_game -lm
clear:
	rm -f chess_game

all:
	gcc -Wall -I. -Ipieces main.c game.c move_validation.c ai.c pieces/*.c -lm -o chess_game && ./chess_game

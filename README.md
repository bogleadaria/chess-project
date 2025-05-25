Dry Repo Setup (Linux/Debian):

Install compiler C/C++:

sudo apt-get install build-essential

Install SDL2:
sudo apt-get install libsdl2-image-dev
sudo apt-get install libsdl2-mixer-dev
sudo apt-get install libsdl2-ttf-dev

Install Make:

sudo apt install make

After all just run in terminal:

make


Manual terminal commands: 
gcc -Wall -lSDL2 -I. -Ipieces main.c game.c move_validation.c pieces/*.c -o chess_game

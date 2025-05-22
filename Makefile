# Makefile for SDL3 Chess Project

CC = gcc
CFLAGS = -Wall -Wextra -O2 `sdl3-config --cflags`
LDFLAGS = `sdl3-config --libs`
SRC = viz.c
TARGET = chess

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)
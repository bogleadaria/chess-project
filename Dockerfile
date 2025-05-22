# FROM ubuntu:latest
FROM alpine:latest

# Install necessary packages
############ ubuntu #############
# RUN apt-get update && apt-get install -y \
#     build-essential \
#     git \
#     cmake \
#     libgl1-mesa-dev \
#     libx11-dev \
#     libxcursor-dev \
#     libxrandr-dev \
#     libxi-dev \
#     libasound2-dev

RUN apk add --no-cache \
    build-base \
    git \
    cmake \
    libx11-dev \
    libxcursor-dev \
    libxrandr-dev \
    libxi-dev \
    alsa-lib-dev \
    mesa-dev


# Clone and build Raylib
# RUN git clone https://github.com/raysan5/raylib.git /raylib && \
#     cd /raylib/src && \
#     make PLATFORM=PLATFORM_DESKTOP

# Set up working directory
WORKDIR /app

# Copy your code into the container
COPY . .

# Compile your code
RUN gcc -Wall -I. -Ipieces main.c game.c move_validation.c pieces/*.c -o chess_game

#pt raylib : P 
#-I/raylib/src -L/raylib/src -lraylib -lm -ldl -lpthread -lGL -lX11

# Command to run your program
CMD ["./chess_game"]

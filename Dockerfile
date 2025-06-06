# FROM ubuntu:latest
FROM alpine:latest

# Install necessary packages
############ ubuntu #############
# RUN apt-get update && apt-get install -y \
#     build-essential \
#     git \
#     cmake \
#     libsdl2-mixer-dev \
#     libsdl2-image-dev \
#     libsdl2-ttf-devs \
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
    libsdl2-mixer-dev \
    libsdl2-image-dev \
    libsdl2-ttf-devs \
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
RUN make -j$(nproc)

# Command to run your program
# CMD ["./chess_game"]

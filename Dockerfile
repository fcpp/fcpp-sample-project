FROM ubuntu:22.04 AS build

RUN apt-get -qq update &&\
    apt-get -qq -y install cmake ninja-build gcc g++ &&\
    apt-get -qq clean all

RUN apt-get -qq -y install git less procps htop &&\
    apt-get -qq clean all

RUN apt-get -qq -y install xorg-dev

RUN apt-get update && apt-get install -y \
    libgl1 \
    libgl1-mesa-glx \
    libglu1-mesa \
    mesa-utils \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y \
    build-essential \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    freeglut3-dev

RUN apt-get -qq -y install libwayland-dev libxkbcommon-dev

# Set environment variable to use host's X11 display
ENV DISPLAY=:0

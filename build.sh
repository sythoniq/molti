#!/usr/bin/sh

gcc -o main ./src/main.c ./src/debug.c ./src/chunk.c \
  ./src/memory.c \
  -Wall -Wextra \
  && ./main

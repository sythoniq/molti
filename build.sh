gcc -g -o main src/main.c src/memory.c src/chunk.c src/debug.c src/value.c src/vm.c src/compiler.c src/scanner.c -Wall -Wextra -fsanitize=address -fno-omit-frame-pointer  && ./main

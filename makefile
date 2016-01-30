# CC = gcc
CC = clang
#Using -Ofast instead of -O3 might result in faster code, but is supported only by newer GCC versions
CFLAGS = -pthread -Ofast -march=native -Wall -funroll-loops -Wno-unused-result
# CFLAGS = -pthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result
LDLIBS=-lm

all: main

word2vec : main.c
	$(CC) $(CFLAGS) main.c $(LDLIBS) -o main
	
clean:
	rm -rf main
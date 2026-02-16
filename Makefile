CC=gcc

all: compile

compile: main.c chess.h chess.c
	$(CC) -Wall -Wextra -c -O3 -Oz -s chess.c -o chess.o
	$(CC) -Wall -Wextra -O3 -Oz -s main.c chess.o -o chess

clean:
	rm -f chess.o chess

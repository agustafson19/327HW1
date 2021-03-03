
all: game

game: game.c
	gcc -Wall -Werror -ggdb3 -o game vertex.c stack.c heap.c game.c

clean:
	rm -f game

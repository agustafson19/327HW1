
all: game

game: game.cpp
	g++ -Wall -Werror -ggdb3 -o game vertex.cpp stack.cpp heap.cpp dice.cpp entity.cpp game.cpp -lncurses

clean:
	rm -f game

# Simple makefile: run 'make' or 'make all' to compile run the executable with ./dungeon
all: dungeon-1.03.c
		gcc -lm -g -Wall -Werror -ggdb -o dungeon dungeon-1.03.c

clean:
		$(RM) dungeon

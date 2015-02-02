# Comments are started with hashes
# To keep things simple in the first introduction, I didn't
# add all the switches we've talked about when I showed this
# in class, but now I've added them to the build commands
# below.  I've also added a rule to delete all of the 
# generated files ("make clean"); it doesn't depend on
# anything.  This is a pretty standard rule to have in a
# make file.

fc: main.o f2c.o
	gcc main.o f2c.o -o fc

main.o: cf.h main.c
	gcc -Wall -Werror -ggdb main.c -c

f2c.o: cf.h f2c.c
	gcc -Wall -Werror -ggdb f2c.c -c

clean:
	rm -f *.o fc

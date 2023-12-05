CC=gcc
OFLAGS=-O0

all:
	$(CC) -Wall $(OFLAGS) -o dir_pstree.exe dir_pstree.c

test:
	make all
	./dir_pstree.exe .. \#include
	make clean

clean:
	rm -f *.o *.so *.a *.exe *.txt
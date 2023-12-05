CC=gcc
OFLAGS=-O0

all:
	$(CC) -Wall $(OFLAGS) -o ls.exe ls.c

test:
	make all
	./ls.exe ../
	./ls.exe ./
	make clean

clean:
	rm -f *.o *.so *.a *.exe *.txt
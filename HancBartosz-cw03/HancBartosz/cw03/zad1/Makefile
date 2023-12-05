CC=gcc
OFLAGS=-O0

all:
	$(CC) -Wall $(OFLAGS) -o creat_proc.exe creat_proc.c

test:
	make all
	./creat_proc.exe 10
	make clean

clean:
	rm -f *.o *.so *.a *.exe *.txt
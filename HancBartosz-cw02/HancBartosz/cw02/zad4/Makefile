CC=gcc
OFLAGS=-O3

all:
	$(CC) -Wall $(OFLAGS) -o count_bytes_ftw.exe count_bytes_ftw.c

test:
	make all
	@echo "-------------------------"
	./count_bytes_ftw.exe ../
	@echo "-------------------------"

clean:
	rm -f *.exe *.o *.so *.a *.txt
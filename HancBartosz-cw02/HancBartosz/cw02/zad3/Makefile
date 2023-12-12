CC=gcc
OFLAGS=-O3

all:
	make count_bytes

count_bytes:
	$(CC) -Wall $(OFLAGS) -o count_bytes.exe count_bytes.c

test:
	make all
	@echo "------------------"
	./count_bytes.exe
	@echo "------------------"
	wc --bytes *
	@echo "------------------"
	
	make clean

clean:
	rm -f *.exe *.o *.so *.a *.txt
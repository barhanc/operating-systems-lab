CC=gcc
OFLAGS=-O0

all:
	$(CC) $(OFLAGS) -Wall -o test.exe saflagtest.c

test:
	make all

	@echo "\n"
	./test.exe SA_SIGINFO

	@echo "\n"
	./test.exe SA_NODEFER

	@echo "\n"
	./test.exe SA_RESETHAND

clean:
	rm -f *.so *.o *.a *.exe
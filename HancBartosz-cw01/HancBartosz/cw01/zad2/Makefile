CC=gcc
CFLAGS=-Wall
OFLAGS=-O0

static: libwc.c repl.c
	$(CC) -Wall $(OFLAGS) -c libwc.c
	ar rcs libwc.a libwc.o
	$(CC) -Wall $(OFLAGS) -c repl.c
	$(CC) -Wall $(OFLAGS) -o repl_static.exe repl.o libwc.o

shared: libwc.c repl.c
	$(CC) -Wall $(OFLAGS) -c -fpic libwc.c
	$(CC) -Wall $(OFLAGS) -shared -o libwc.so libwc.o
	$(CC) -Wall $(OFLAGS) -L. -Wl,-rpath=. -o repl_shared.exe repl.c -lwc

clean:
	rm -f *.o *.a *.so *.exe
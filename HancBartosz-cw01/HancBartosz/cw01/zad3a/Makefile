CC=gcc
CFLAGS=-Wall -c
OFLAGS=-O0

all:
	make static
	make shared

static: libwc.c repl.c
	$(CC) -Wall $(OFLAGS) -c libwc.c
	ar rcs libwc.a libwc.o
	$(CC) -Wall $(OFLAGS) -c repl.c
	$(CC) -Wall $(OFLAGS) -o repl_static.exe repl.o libwc.o

shared: libwc.c repl.c
	$(CC) -Wall $(OFLAGS) -c -fPIC libwc.c
	$(CC) -Wall $(OFLAGS) -shared -o libwc.so libwc.o
	$(CC) -Wall $(OFLAGS) -L./ -Wl,-rpath=./ -o repl_shared.exe repl.c -lwc

test:
	echo "init 10" > test.in

	echo "count libwc.c" >> test.in
	echo "count libwc.h" >> test.in
	echo "count repl.c" >> test.in

	echo "show 0" >> test.in
	echo "show 1" >> test.in
	echo "show 2" >> test.in

	echo "delete 0" >> test.in
	echo "delete 1" >> test.in
	echo "delete 2" >> test.in

	echo "destroy" >> test.in
	echo "quit" >> test.in

	./repl_static.exe < test.in > results_static.txt
	./repl_shared.exe < test.in > results_shared.txt

	rm -f test.in

report:
	make all
	make test
	make clean

clean:
	rm -f *.o *.a *.so *.exe

CC=gcc
CFLAGS=-Wall -c

libwc_static: libwc.c
	$(CC) $(CFLAGS) libwc.c
	ar rcs libwc.a libwc.o

libwc_shared: libwc.c
	$(CC) -fPIC $(CFLAGS) libwc.c
	$(CC) -shared -o libwc.so libwc.o

clean:
	rm -f *.o *.a *.so
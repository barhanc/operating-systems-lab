CC=gcc
OFLAGS=-O3

all:
	make replace_lib
	make replace_sys

replace_lib:
	$(CC) -Wall $(OFLAGS) -o replace_lib.exe replace_lib.c

replace_sys:
	$(CC) -Wall $(OFLAGS) -o replace_sys.exe replace_sys.c

test:
	make all
	./replace_sys.exe a b replace_lib.c out_sys.txt
	./replace_lib.exe a b replace_lib.c out_lib.txt
	diff -s out_sys.txt out_lib.txt

report:
	make all

	echo "(SYS)" > ./pomiar_zad_1.txt
	./replace_sys.exe a b replace_lib.c out_sys.txt >> ./pomiar_zad_1.txt
	
	echo "" >> ./pomiar_zad_1.txt

	echo "(LIB)" >> ./pomiar_zad_1.txt
	./replace_lib.exe a b replace_lib.c out_lib.txt >> ./pomiar_zad_1.txt

	rm -f out_lib.txt out_sys.txt

	make clean

clean:
	rm -f *.o *.so *.a *.exe
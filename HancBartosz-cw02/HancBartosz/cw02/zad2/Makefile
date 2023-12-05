CC=gcc
OFLAGS=-O3

make all:
	make reverse1
	make reverse1024

reverse1:
	$(CC) -Wall $(OFLAGS) -o reverse_1.exe reverse_1.c

reverse1024:
	$(CC) -Wall $(OFLAGS) -o reverse_1024.exe reverse_1024.c

test:
	make clean
	make all

	./reverse_1.exe reverse_1.c out_1.txt
	./reverse_1024.exe reverse_1.c out_1024.txt
	
	diff -s out_1.txt out_1024.txt

report:
	make all

	echo "(1 CHAR)" > ./pomiar_zad_2.txt
	./reverse_1.exe reverse_1.c out_1.txt >> ./pomiar_zad_2.txt

	echo "" >> ./pomiar_zad_2.txt

	echo "(1024 CHAR)" >> ./pomiar_zad_2.txt
	./reverse_1024.exe reverse_1.c out_1024.txt >> ./pomiar_zad_2.txt

	rm -f out_1.txt out_1024.txt

	make clean

clean:
	rm -f *.exe *.so *.a *.o
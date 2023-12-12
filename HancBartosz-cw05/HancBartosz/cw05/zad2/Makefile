CC=gcc
OFLAGS=-O0

all:
	$(CC) $(OFLAGS) -Wall -o integral.exe integral.c

report:
	make all

	@echo "dx = 0.01 , N = 1" > report.txt
	./integral.exe 0.01 1 >> report.txt
	@echo "" >> report.txt

	@echo "dx = 0.00001 , N = 1" >> report.txt
	./integral.exe 0.00001 1 >> report.txt
	@echo "" >> report.txt

	@echo "dx = 0.000001 , N = 1" >> report.txt
	./integral.exe 0.000001 1 >> report.txt
	@echo "" >> report.txt

	@echo "dx = 0.000001 , N = 10" >> report.txt
	./integral.exe 0.000001 10 >> report.txt
	@echo "" >> report.txt

	@echo "dx = 0.000001 , N = 100" >> report.txt
	./integral.exe 0.000001 100 >> report.txt
	@echo "" >> report.txt

	@echo "dx = 0.00000001 , N = 1" >> report.txt
	./integral.exe 0.00000001 1 >> report.txt
	@echo "" >> report.txt

	@echo "dx = 0.00000001 , N = 10" >> report.txt
	./integral.exe 0.00000001 10 >> report.txt
	@echo "" >> report.txt

	@echo "dx = 0.00000001 , N = 100" >> report.txt
	./integral.exe 0.00000001 100 >> report.txt

	make clean

clean:
	rm -f *.exe
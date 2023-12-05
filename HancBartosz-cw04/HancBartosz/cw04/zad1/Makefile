CC=gcc
OFLAGS=-O0

all:
	$(CC) $(OFLAGS) -Wall -o fork_test.exe fork_test.c
	$(CC) $(OFLAGS) -Wall -o exec_test.exe exec_test.c

report:
	make all
	echo "FORK" > report.txt

	echo "\n>>> Ignore:" >> report.txt
	./fork_test.exe ignore >> report.txt

	echo "\n>>> Handle:" >> report.txt
	./fork_test.exe handler >> report.txt

	echo "\n>>> Mask:" >> report.txt
	./fork_test.exe mask >> report.txt

	echo "\n>>> Pending:" >> report.txt
	./fork_test.exe pending >> report.txt

	echo "\nEXECL" >> report.txt

	echo "\n>>> Ignore:" >> report.txt
	./exec_test.exe ignore >> report.txt

	echo "\n>>> Mask:" >> report.txt
	./exec_test.exe mask >> report.txt

	echo "\n>>> Pending:" >> report.txt
	./exec_test.exe pending >> report.txt

	make clean

clean:
	rm -f *.so *.o *.a *.exe
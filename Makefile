CC = gcc
CFLAGS = -Wall -Wextra -g

all: spchk

spchk: spchk.o dictionary.o file_reader.o
	$(CC) $(CFLAGS) -o spchk spchk.o dictionary.o file_reader.o

spchk.o: spchk.c
	$(CC) $(CFLAGS) -c spchk.c

dictionary.o: dictionary.c
	$(CC) $(CFLAGS) -c dictionary.c

file_reader.o: file_reader.c
	$(CC) $(CFLAGS) -c file_reader.c

clean:
	rm -f *.o spchk
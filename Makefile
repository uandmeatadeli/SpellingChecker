CC = gcc
CFLAGS = -Wall -fsanitize=address -std=c99 -O2 -g

all: spchk

spchk: spchk.o 
	$(CC) $(CFLAGS) -o $@ $^

spchk.o: spchk.c spchk.h
	$(CC) $(CFLAGS) -c -o $@ spchk.c

clean:
	rm -f *.o spchk
# CC = gcc
# CFLAGS = -Wall -fsanitize=address -std=c99 -O2 -g

# all: spchk

# spchk: spchk.o 
# 	$(CC) $(CFLAGS) -o $@ $^

# spchk.o: spchk.c 
# 	$(CC) $(CFLAGS) -c -o $@ $<

# clean:
# 	rm -f *.o spchk
CC=gcc
CFLAGS=-D_XOPEN_SOURCE=700 -std=c11

ahencode : ahencode.o
	$(CC) $(CFLAGS) -o ahencode ahencode.o

ahencode.o : ahencode.c
	$(CC) $(CFLAGS) -c ahencode.c

clean :
	rm ahencode ahencode.o


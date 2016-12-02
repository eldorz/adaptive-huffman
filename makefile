CC=gcc
CFLAGS=-D_XOPEN_SOURCE=700 -std=c11 -ggdb

ahencode : ahencode.o common.o
	$(CC) $(CFLAGS) -o ahencode ahencode.o common.o

ahencode.o : ahencode.c common.h
	$(CC) $(CFLAGS) -c ahencode.c

common.o : common.c common.h
	$(CC) $(CFLAGS) -c common.c

clean :
	rm ahencode *.o


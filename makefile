CC=gcc
CFLAGS=-D_XOPEN_SOURCE=700 -std=c11 -O3

all : ahencode ahdecode

debug : CFLAGS=-D_XOPEN_SOURCE=700 -std=c11 -gdwarf-3
debug : all

ahencode : ahencode.o common.o
	$(CC) $(CFLAGS) -o ahencode ahencode.o common.o

ahencode.o : ahencode.c common.h
	$(CC) $(CFLAGS) -c ahencode.c

ahdecode : ahdecode.o common.o
	$(CC) $(CFLAGS) -o ahdecode ahdecode.o common.o

ahdecode.o : ahdecode.c common.h
	$(CC) $(CFLAGS) -c ahdecode.c

common.o : common.c common.h
	$(CC) $(CFLAGS) -c common.c

clean :
	rm ahencode ahdecode *.o


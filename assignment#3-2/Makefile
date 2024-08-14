CC = gcc
RT = -lrt
CFLAGS = -o

all : filegen schedtest

filegen : filegen.c
	$(CC) $(CFLAGS) $@ $^ $(RT)

schedtest : schedtest.c
	$(CC) $(CFLAGS) $@ $^ $(RT)

clean : 
	rm -rf filegen schedtest temp
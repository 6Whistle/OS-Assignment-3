CC = gcc
RT = -lrt
CFLAGS = -o
THREAD = -pthread

all : numgen fork thread

numgen : numgen.c
	$(CC) $(CFLAGS) $@ $^

fork : fork.c
	$(CC) $(CFLAGS) $@ $^ $(RT)

thread : thread.c
	$(CC) $(THREAD) $(CFLAGS) $@ $^ $(RT)

clean : 
	rm numgen fork thread
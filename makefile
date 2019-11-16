# Makefile

CC = gcc
CFLAGS = -Wall -g
LIBS = -lcrypto -pthread

.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

all: messages.h server.c priority_list.o structs.h reversehashing.o hashtable.o
	${CC} ${CFLAGS} -o server $^ ${LIBS}

clean:
	rm -f server
	rm -f *.o

# Makefile

CC = gcc
CFLAGS = -Wall -g -O3
LIBS = -lcrypto -pthread

all:
	${CC} ${CFLAGS} -o server server.c reversehashing.c ${LIBS}

clean:
	rm -f *.o
	rm -f server

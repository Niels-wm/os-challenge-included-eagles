# Makefile

CFLAG = -Wall -g
CC = gcc
LIBS = -lcrypto -pthread


all:
	${CC} ${CFLAGS} -o server server.c reversehashing.c hashtable.c consumer.c ${LIBS}

clean:
	rm -f server

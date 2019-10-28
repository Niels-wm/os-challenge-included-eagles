# Makefile

CFLAG = -Wall -g
CC = gcc
LIBS = -lcrypto


all:
	${CC} ${CFLAGS} -o server server.c reversehashing.c priorityQueue.c ${LIBS}

clean:
	rm -f server

# Makefile

CFLAG = -Wall -g
CC = gcc
LIBS = -lcrypto


all:
	${CC} ${CFLAGS} -o server server.c ${LIBS}


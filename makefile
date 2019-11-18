# Makefile

CC = gcc
CFLAGS = -Wall -g -O3
LIBS = -lcrypto -pthread

.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

all: messages.h server.c reversehashing.c priority_list.o structs.h hashtable.c
	${CC} ${CFLAGS} -o server $^ ${LIBS}

clean:
	rm -f server
	rm -f *.o



# Makefile

# CC = gcc
# CFLAGS = -Wall -g -O3
# LIBS = -lcrypto -pthread
#
#all:
#	${CC} ${CFLAGS} -o server server.c priority_list.c reversehashing.c hashtable.c ${LIBS}
#
#clean:
#	rm -f *.o
#	rm -f server

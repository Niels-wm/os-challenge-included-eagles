#include <stdio.h>
#include <stdlib.h>
#include "messages.h"
#include "packet.h"
#include "reversehashing.h"

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "threadinfo.h"

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include "hashtable.h"

#define PORT 5003
#define THREAD_AMOUNT 5

int* threadAmount;
struct Packet packets[100];

int main(int argc, char *argv[]) {
    int sockFileDescripter, newSockFileDescripter;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int i, err;
    initHashTable();

    //pthread_t thread;

    sockFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFileDescripter < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Disable safety feature */
    // The operating system sets a timeout on TCP sockets after using them.
    // It does that to make sure that all information from the old program
    // is gone before starting a new one
    int option = 1;
    setsockopt(sockFileDescripter, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));


    /* Initialize socket structure */
    // bzero() is used to set all the socket structures with null values. It does the same thing as the following:
    // memset(&serverAddr, '\0', sizeof(serverAddr));
    bzero((char *)&serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;


    /* Bind */
    if (bind(sockFileDescripter, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
        perror("ERROR on binding");
        exit(1);
    }

    /* Listen */
    listen(sockFileDescripter, 500);
    clientAddrSize = sizeof(clientAddr);

    i = 0;
    pthread_t tid[THREAD_AMOUNT];

    // // Put the accept statement and the following code in an infinite loop
    while (1) {
            // printf("\nHere are the i: %d", i);
            /* Accept */
            newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);

            if (newSockFileDescripter < 0){
                perror("ERROR on accept");
                exit(1);
            }
            struct ThreadInfo* ti = malloc(sizeof(struct ThreadInfo));
            ti->fs = newSockFileDescripter;

            // For each client request creates a thread and assign the request to it to process
            err = pthread_create(&tid[i%THREAD_AMOUNT], NULL, reversehashing, ti);

            // pthread_join(tid[(i)%THREAD_AMOUNT], NULL);
            if (err != 0) {
                perror("ERROR creating thread");
                exit(1);
            }
            if (i>=(THREAD_AMOUNT-1)) {
              pthread_join(tid[(i+1)%THREAD_AMOUNT], NULL);
            }
            i++;
    }
}

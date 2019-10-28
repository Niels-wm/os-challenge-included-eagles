#include <stdio.h>
#include <stdlib.h>
#include "messages.h"
#include "packet.h"
#include "reversehashing.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#define PORT 5003
#define NTHREADS 50


int main(int argc, char *argv[]) {
    int sockFileDescripter, newSockFileDescripter;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int n, i, err;
    i = 0;
    pthread_t thread_id[NTHREADS];


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
    listen(sockFileDescripter, 50);
    clientAddrSize = sizeof(clientAddr);

    // Put the accept statement and the following code in an infinite loop
    while (1) {
            /* Accept */
            newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);
            
            if (newSockFileDescripter < 0){
                perror("ERROR on accept");
                exit(1);
            }

            printf("New Client Request with ID: %d\n", i);


            // For each client request creates a thread and assign the request to it to process
            struct arg_struct *args = malloc(sizeof(struct arg_struct));
            args -> fileDescripter = newSockFileDescripter;


            printf("Starting thread_id[%d]\n", ((i)%NTHREADS));
            err = pthread_create(&thread_id[i%NTHREADS] , NULL, reversehashing, args);

            if (err != 0) {
                perror("ERROR creating thread");
                exit(1);
            }

            if (i >= NTHREADS-1) {
                printf("Waiting for thread_id[%d]\n", ((i+1)%NTHREADS));
                pthread_join(thread_id[(i+1)%NTHREADS], NULL);
            }
            i++;
    }
}

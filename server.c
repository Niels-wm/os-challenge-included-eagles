#include <stdio.h>
#include <stdlib.h>

#include "messages.h"
#include "packet.h"
#include "reversehashing.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <pthread.h>
#include <sched.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#define PORT 5003
#define NTHREADS 50

// Scheduler Policy: Round-Robin(RR), FIFO(FIFO).
int policy = SCHED_FIFO;

int main(int argc, char *argv[]) {
    int sockFileDescripter, newSockFileDescripter;
    int priority_min, priority_max;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int n, err;
    int i = 0;
    pthread_t thread_id[NTHREADS];

    sockFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFileDescripter < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    int j;
    for (j = 0; j < 10000; j++)
    {
        int a = 2+2;
    }
    

    /* Disable safety feature */
    // The operating system sets a timeout on TCP sockets after using them. 
    // It does that to make sure that all information from the old program 
    // is gone before starting a new one
    int option = 1;
    setsockopt(sockFileDescripter, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    /* Initialize socket structure */
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
            struct Packet packet;
            
            pthread_attr_t tattr;
            struct sched_param param;

            /* Accept */
            newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);
            
            if (newSockFileDescripter < 0) {
                perror("ERROR on accept");
                exit(1);
            }

            /* Receive */
            bzero((char *)&packet, sizeof(packet));
            n = read(newSockFileDescripter, &packet, sizeof(packet));

            if (n < 0) {
                perror("ERROR reading from socket");
                exit(1);
            }

            // Reverse the start and end
            packet.start = be64toh(packet.start);
            packet.end = be64toh(packet.end);

            // Thread priority
            // Max thread priority (RR) is 99. 99/16 ~ 6
            param.sched_priority = (int) packet.p * 6;

            err = pthread_attr_init(&tattr);
            if (err != 0){
                perror("Error initializing thread attributes");
            }

            err = pthread_attr_setschedpolicy(&tattr, policy);
            if (err != 0) {
                perror("Error setting thread schedule policy");
            }

            err = pthread_attr_setschedparam(&tattr, &param);
            if (err != 0) {
                perror("Error setting thread priority");
            }

            // For each client request creates a thread and assign the request to it to process
            struct arg_struct *args = malloc(sizeof(struct arg_struct));
            args -> fileDescripter = newSockFileDescripter;
            args -> packet = packet;

            printf("Starting thread_id[%d]\n", ((i)%NTHREADS));

            err = pthread_create(&thread_id[i%NTHREADS], &tattr, reversehashing, args);
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

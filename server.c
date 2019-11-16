#include <stdio.h>
#include <stdlib.h>
#include "messages.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <inttypes.h>
#include <unistd.h>

#include <pthread.h>

#include "priority_list.h"
#include "reversehashing.h"

#define PORT 5003
#define NUM_THREADS 4

pthread_mutex_t* ht_lock;

static void* worker_thread(void* vp);

int main(int argc, char *argv[]) {
    int sockFileDescripter, newSockFileDescripter;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;

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
    listen(sockFileDescripter, 5);
    clientAddrSize = sizeof(clientAddr);



    ht_lock = malloc(sizeof(pthread_mutex_t));


    /* initialize the priority list */
    init_list();

    /* create the worker threads */
    int i;
    for(i = 0; i < NUM_THREADS; i++){
        pthread_t thread;
        pthread_create(&thread, NULL, &worker_thread, NULL);
    }

    // Put the accept statement and the following code in an infinite loop
    while (1) {

        /* Accept */
        newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);

        if (newSockFileDescripter < 0){
            perror("ERROR on accept");
            exit(1);
        }
        
        /* Build the request entry in the linked-list */
        struct Packet current_packet;
        bzero((char *) &current_packet, sizeof(current_packet));
        if (read(newSockFileDescripter, &current_packet, sizeof(current_packet)) < 0){
            perror("ERROR reading from socket");
            exit(1);
        }

        /* construct the request, which tracks the return socket as well as the expected value of task completion */
        struct Request request;
        request.reply_socket = newSockFileDescripter;
        request.packet = current_packet;
        request.prio = (current_packet.end - current_packet.start) / current_packet.p; // compute the difficulty/reward of the task

        push_item(request);
    }
}

/* the worker threads simply pull items from the list and process them */
static void* worker_thread(void* vp){
    while(1){
        struct Request request = pop_item();
        if(request.reply_socket == -1){
            usleep(1000); // prevent to worker threads from just spinning if they're idle - give someone else the wheel!
            continue;
        }
        reversehashing(request, ht_lock);
    }
    return NULL; // I win this time, gcc! :P
}

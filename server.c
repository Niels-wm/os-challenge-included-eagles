#include <stdio.h>
#include <stdlib.h>
#include "messages.h"
#include "packet.h"
#include "reversehashing.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "threadinfo.h"
#include "node.h"

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#define PORT 5003
#define THREAD_AMOUNT 3

int threadAmount;
pthread_mutex_t* lock; 
pthread_mutex_t* queLock;
struct Packet packets[100];
struct Node* head = NULL;
int threadActive[3];
pthread_t new;


struct ThreadInfo* getNextTi(struct Packet *p){
    struct ThreadInfo* returnVal = head->ti;
    memcpy(p,&head->packet,sizeof(struct Packet));
    struct Node* oldhead = head;
    head = head->next;
    free(oldhead);
    return returnVal;
}

void *startReverseHashing(void *arg){
    struct Packet* packet = malloc(sizeof(struct Packet));
    pthread_mutex_lock(queLock);
    struct ThreadInfo* ti = getNextTi(packet);
    pthread_mutex_unlock(queLock);
    threadAmount++;
    reversehashing(ti,packet);
    threadAmount--;
    printf("Done\n");
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
printf("innnnnnnnit\n");
    int sockFileDescripter, newSockFileDescripter;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int n, i, pid, err;
    
    lock = malloc(sizeof(pthread_mutex_t));
    queLock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(lock, NULL);
    pthread_mutex_init(queLock, NULL);
    
    printf("innit\n");
    //pthread_t thread;


    sockFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFileDescripter < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    printf("1");
    /* Disable safety feature */
    // The operating system sets a timeout on TCP sockets after using them.
    // It does that to make sure that all information from the old program
    // is gone before starting a new one
    int option = 1;
    setsockopt(sockFileDescripter, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

printf("2");
    /* Initialize socket structure */
    // bzero() is used to set all the socket structures with null values. It does the same thing as the following:
    // memset(&serverAddr, '\0', sizeof(serverAddr));
    bzero((char *)&serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

printf("3");
    /* Bind */
    if (bind(sockFileDescripter, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
        perror("ERROR on binding");
        exit(1);
    }
printf("4");
    /* Listen */
    listen(sockFileDescripter, 50);
    clientAddrSize = sizeof(clientAddr);

    // // Put the accept statement and the following code in an infinite loop /*
    printf("hi");
    i = 2;
    while (1) {
        // printf("\nHere are the i: %d", i);
        /* Accept */
        printf("accepting...");
        newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);
        printf("accepted!");        
        if (newSockFileDescripter < 0){
            perror("ERROR on accept");
            exit(1);
        }
        struct ThreadInfo* ti = malloc(sizeof(struct ThreadInfo));
        ti->fs = newSockFileDescripter;
        ti->lock = lock;

        struct Node* request = malloc(sizeof(struct Node));
        struct Packet packet;

        
        
        n = read(ti->fs, &packet, sizeof(packet));

        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        }
        request->packet = packet;
        request->ti = ti;
        
        struct Node* node = head;
        pthread_mutex_lock(queLock);
        if(node != NULL){
            while(node != NULL){
                if(node->next != NULL){
                    if(request->packet.p <= node->packet.p && request->packet.p > node->next->packet.p){
                    request->next = node->next;
                    node->next = request;
                    }
                }
                else{
                    request->next = NULL;
                    node->next = request;
                }
                
            }
        }
        else{
            head = request;
        }
        pthread_mutex_unlock(queLock);
        
        if(threadAmount < 1){
            err = pthread_create(&new, NULL, startReverseHashing, NULL);
        }
    }
}

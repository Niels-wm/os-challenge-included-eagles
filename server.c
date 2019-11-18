#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
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
pthread_mutex_t* threadsLock;
struct Packet packets[100];
struct Node* head = NULL;
int threadActive[3];
pthread_t new;


struct ThreadInfo* getNextTi(struct Packet *p){
    struct ThreadInfo* returnVal = NULL;
    if(head != NULL){
        returnVal = head->ti;
        memcpy(p,&head->packet,sizeof(struct Packet));
        struct Node* oldhead = head;
        head = head->next;
        free(oldhead);
    }
    else{
        //printf("head is null, gonna seg fault");
    }

    return returnVal;
}

void *startReverseHashing(void *arg){
    struct Packet* packet = malloc(sizeof(struct Packet));
    //printf("locking quelock\n");
    pthread_mutex_lock(queLock);
    //printf("getting nedxt ti\n");
    struct ThreadInfo* ti = getNextTi(packet);
    //printf("Got ti\n");
    pthread_mutex_unlock(queLock);
     //printf("unlocked quelock\n");
    if(ti != NULL){
        

        reversehashing(ti,packet);
    
    }
    pthread_mutex_lock(threadsLock);
    threadAmount--;
    //printf("nr of threads: %d", threadAmount);
    pthread_mutex_unlock(threadsLock);
    //printf("Done\n");
}


int main(int argc, char *argv[]) {
//printf("innnnnnnnit\n");
    int sockFileDescripter, newSockFileDescripter;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int n, i, pid, err;
    int wait = 0;
    struct pollfd fds[1];
    int timeout_msecs = 3000;
    int ret;
    threadAmount = 0;
    
    lock = malloc(sizeof(pthread_mutex_t));
    queLock = malloc(sizeof(pthread_mutex_t));
    threadsLock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(lock, NULL);
    pthread_mutex_init(queLock, NULL);
    
    //printf("innit\n");
    //pthread_t thread;


    sockFileDescripter = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFileDescripter < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    //printf("1");
    /* Disable safety feature */
    // The operating system sets a timeout on TCP sockets after using them.
    // It does that to make sure that all information from the old program
    // is gone before starting a new one
    int option = 1;
    setsockopt(sockFileDescripter, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

//printf("2");
    /* Initialize socket structure */
    // bzero() is used to set all the socket structures with null values. It does the same thing as the following:
    // memset(&serverAddr, '\0', sizeof(serverAddr));
    bzero((char *)&serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

//printf("3");
    /* Bind */
    if (bind(sockFileDescripter, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
        perror("ERROR on binding");
        exit(1);
    }
//printf("4");
    /* Listen */
    listen(sockFileDescripter, 50);
    clientAddrSize = sizeof(clientAddr);

    // // Put the accept statement and the following code in an infinite loop /*
    //printf("hi");
    fcntl(sockFileDescripter, F_SETFL, O_NONBLOCK);

    fds[0].fd = sockFileDescripter;
    fds[0].events = POLLIN;
    while (1) {
        // //printf("\nHere are the i: %d", i);
        /* Accept */
        //printf("polling...\n");
        ret = poll(fds, 2, timeout_msecs);
        //printf("poll done\n");
        if(ret > 0 && (fds[0].revents & POLLIN)){
            //printf("accepting\n");
            newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);
            //printf("accepted!\n");        
            if (newSockFileDescripter < 0 ){
                perror("ERROR on accept");
                exit(1);
            }
    
            struct ThreadInfo* ti = malloc(sizeof(struct ThreadInfo));
            ti->fs = newSockFileDescripter;
            ti->lock = lock;

            struct Node* request = malloc(sizeof(struct Node));
            struct Packet packet;

            //printf("print1");
            
            n = read(ti->fs, &packet, sizeof(packet));

            if (n < 0)
            {
                perror("ERROR reading from socket");
                exit(1);
            }
            request->packet = packet;
            request->ti = ti;
            
            struct Node* node = head;
            //printf("print2");
            //printf("locked quelock\n");
            pthread_mutex_lock(queLock);
            if(node != NULL){
                //printf("node not null\n");
            }
            if(request != NULL){
                //printf("request not null\n");
            }
            if(node != NULL){
                while(node != NULL){
                    //printf("request p: %d, node p: %d, next p: xxxx.\n",request->packet.p,node->packet.p/*,node->next->packet.p*/);
                    if(node->next != NULL){
                        //printf("next p: %d\n", node->next->packet.p);
                        if(request->packet.p <= node->packet.p && request->packet.p >= node->next->packet.p){

                            //printf("hi");
                            request->next = node->next;
                            node->next = request;
                            break;
                        }
                        else if (request->packet.p >= node->packet.p){
                            request->next = node;
                            head = request;
                            break;
                        }
                        else{
                            node = node->next;
                        }
                    }
                    else{
                        //printf("entered else\n");
                        request->next = NULL;
                        node->next = request;
                        break;
                    }
                    
                }
            }
            else{
                head = request;
            }
            
            pthread_mutex_unlock(queLock);
            //printf("unlocked quelock\n");
            
        }
        
        pthread_mutex_lock(threadsLock);
        if(threadAmount < 3){

            err = pthread_create(&new, NULL, startReverseHashing, NULL);
               
            threadAmount++;
            //printf("nr of threads: %d", threadAmount);

        }
        else if(threadAmount = 3){
            wait = 1;
            //printf("waiting...\n");
        }
        pthread_mutex_unlock(threadsLock);
        
        //printf("locking quelock main\n");
        //printf("nr of threads: %d", threadAmount);
        pthread_mutex_lock(threadsLock);
        if(threadAmount < 3){
            wait = 0;
            //printf("resuming");
            err = pthread_create(&new, NULL, startReverseHashing, NULL);
            threadAmount++;
            //printf("nr of threads: %d", threadAmount);
        }
        pthread_mutex_unlock(threadsLock);
        //printf("unlocked quelock\n");

        

    }
}

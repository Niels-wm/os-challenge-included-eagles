#include <stdio.h>
#include <stdlib.h>
#include "messages.h"
#include "packet.h"
#include "reversehashing.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "threadinfo.h"

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include "producer.h"
#include "consumer.h"
#include "sempacket.h"
#include <semaphore.h>

#define PORT 5003
#define THREAD_AMOUNT 10

int* threadAmount;
pthread_mutex_t* lock;
struct Packet packets[100];

int main(int argc, char *argv[]) {
    int sockFileDescripter, newSockFileDescripter;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;
    int n, i, pid, err;
    lock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(lock, NULL);

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

    initReverseHashing(lock);

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

    struct JobQueuePacket *jobQueuePacket = malloc(sizeof(struct JobQueuePacket));

    int jobQueueSize = 30;
    int value;
    // Initialize job queue and send it to producer
    jobQueuePacket->jobEmptyCount = malloc(sizeof(sem_t));
    jobQueuePacket->jobFillCount = malloc(sizeof(sem_t));
    jobQueuePacket->jobQueueLock = malloc(sizeof(sem_t));
    sem_init(jobQueuePacket->jobEmptyCount, 0, jobQueueSize);
    sem_init(jobQueuePacket->jobFillCount, 0, 0);
    sem_init(jobQueuePacket->jobQueueLock, 0, 1);
    jobQueuePacket->queue = malloc(sizeof(struct Packet)*jobQueueSize);
    jobQueuePacket->jobPosition = malloc(sizeof(int));
    *jobQueuePacket->jobPosition = -1;
    sem_getvalue(jobQueuePacket->jobEmptyCount, &value);
    printf("emptyCount %d\n", value);


    initProducer(jobQueuePacket, sockFileDescripter);
    initConsumer(jobQueuePacket);

    // clientAddrSize = sizeof(clientAddr);

    i = 0;
    // pthread_t producerTid[THREAD_AMOUNT];
    pthread_t tIdProducer;
    pthread_t tIdsConsumers[4];

    for (size_t i = 0; i < 4; i++) {
      err = pthread_create(&tIdsConsumers[i], NULL, consumeFromJobQueue, NULL);
      if (err != 0) {
          perror("ERROR creating thread");
          exit(1);
      }
    }

    err = pthread_create(&tIdProducer, NULL, produceToJobQueue, NULL);
    // pthread_join(tid[(i)%THREAD_AMOUNT], NULL);
    if (err != 0) {
        perror("ERROR creating thread");
        exit(1);
    }



    // err = pthread_create(&tIdConsumer5, NULL, consumeFromJobQueue, NULL);
    // // pthread_join(tid[(i)%THREAD_AMOUNT], NULL);
    // if (err != 0) {
    //     perror("ERROR creating thread");
    //     exit(1);
    // }


    pthread_join(tIdProducer, NULL);

    for (size_t i = 0; i < 4; i++) {
      pthread_join(tIdsConsumers[i], NULL);
    }
    // pthread_join(tIdConsumer5, NULL);

    // // Put the accept statement and the following code in an infinite loop
    // while (1) {
    //   // printf("\nHere are the i: %d", i);
    //   /* Accept */
    //   newSockFileDescripter = accept(sockFileDescripter, (struct sockaddr *)&clientAddr, &clientAddrSize);
    //
    //   if (newSockFileDescripter < 0){
    //       perror("ERROR on accept");
    //       exit(1);
    //   }
    //
    //   struct ThreadInfo* ti = malloc(sizeof(struct ThreadInfo));
    //   ti->fs = newSockFileDescripter;
    //   // For each client request creates a thread and assign the request to it to process
    //   err = pthread_create(&tid[i%THREAD_AMOUNT], NULL, reversehashing, ti);
    //   // pthread_join(tid[(i)%THREAD_AMOUNT], NULL);
    //   if (err != 0) {
    //       perror("ERROR creating thread");
    //       exit(1);
    //   }
    //   if (i>=(THREAD_AMOUNT-1)) {
    //     pthread_join(tid[(i+1)%THREAD_AMOUNT], NULL);
    //   }
    //   i++;
    // }
}

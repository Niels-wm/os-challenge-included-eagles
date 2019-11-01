
#include "sempacket.h"
#include "packet.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include "hashtable.h"
// #include <netinet/in.h>

// sem_t* emptyCount;
// sem_t* fillCount;
// sem_t* queueLock;
int socketFD;

// struct Packet* jobQueue;

// int* jobPos;

pthread_mutex_t hashTableLock;


void initProducer(int sockFD){
  // emptyCount = queuePacket->jobEmptyCount;
  // fillCount = queuePacket->jobFillCount;
  // queueLock = queuePacket->jobQueueLock;
  // jobQueue = queuePacket->queue;
  // jobPos = queuePacket->jobPosition;
  socketFD = sockFD;
  pthread_mutex_init(&hashTableLock, NULL);

  printf("INITIALIZED PRODUCER\n" );
}

void *produceToJobQueue(void *args){
  struct Packet packet;
  struct sockaddr_in  clientAddr;
  socklen_t clientAddrSize;
  int err, fd;
  uint64_t answer;
  uint8_t testHash[32];
  uint64_t start, end;

  bzero(testHash, 32);
  // int counter = *jobPos;

  printf("Producer started\n");

  clientAddrSize = sizeof(clientAddr);
  bzero((char *)&packet, sizeof(struct Packet));

  while(1) {

    fd = accept(socketFD, (struct sockaddr *)&clientAddr, &clientAddrSize);

    if (fd < 0){
        perror("ERROR on accept");
        exit(1);
    }

    bzero((char *)&packet, sizeof(struct Packet));
    err = read(fd, &packet, sizeof(struct Packet));
    if (err < 0){
      perror("ERROR reading from socket");
      exit(1);
    }

    pthread_mutex_lock(&hashTableLock);
    answer = find(packet.hash);
    pthread_mutex_unlock(&hashTableLock);

    SHA256((char*) &answer, 8, testHash);
    if (answer != 0 && memcmp(testHash, packet.hash, sizeof(testHash)) == 0){
      printf("\nFOUND FROM HASHTABLE %"PRIu64, answer);
      printf("\n");
      // answer = be64toh(answer);
    } else {
      end = be64toh(packet.end);
      start = be64toh(packet.start);
      // uint8_t hash[32] = packet.hash;
      printf("ITERATING\n");

      for (answer = start; answer <= end; answer++){

        SHA256((char*) &answer, 8, testHash);
        if (memcmp(testHash, packet.hash, sizeof(testHash)) == 0) {
          printf("FOUND ANSWER: %" PRIu64, answer);
          pthread_mutex_lock(&hashTableLock);
          insert(packet.hash, answer);
          pthread_mutex_unlock(&hashTableLock);
          // return htobe64(answer);
          break;
        }
      }
    }



    // answer = reversehashing(packet.start, packet.end, packet.hash);
    printf("FOUND ANSWER %" PRIu64, answer);
    answer = htobe64(answer);
    write(fd, &answer, 8);

    // sem_wait(emptyCount);
    // sem_wait(queueLock);
    // // Critical section
    // counter = *jobPos;
    // counter++;
    // *jobPos = counter;
    // jobQueue[*jobPos] = packet;
    //
    // sem_post(queueLock);
    // sem_post(fillCount);
  }
  return NULL;
}

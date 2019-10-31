#include "sempacket.h"
#include <stdio.h>
#include "packet.h"
#include <semaphore.h>
#include "reversehashing.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

sem_t emptyCount;
sem_t fillCount;
sem_t queueLock;
int socketFS;
int socketFD;

struct Packet* jobQueue;

int jobPos;
int jobQueueSize;

void initConsumer(int sockFD){
  jobQueueSize = 10;
  sem_init(&queueLock, 0, 1);
  sem_init(&emptyCount, 0, jobQueueSize);
  sem_init(&fillCount, 0, 0);
  jobQueue = malloc(sizeof(struct Packet)*jobQueueSize);
  jobPos = -1;
  socketFD = sockFD;

  printf("INITIALIZED CONSUMER\n");
}

void *consumeFromJobQueue(void *arg){
  struct Packet packet;
  int value;
  uint64_t answer;

  while(1) {

    sem_wait(&fillCount);
    sem_wait(&queueLock);
    packet = jobQueue[jobPos];
    jobPos--;
    sem_post(&queueLock);
    sem_post(&emptyCount);

    answer = reversehashing(packet.start, packet.end, packet.hash);
    write(packet.fd, &answer, 8);
  }
}

void *produceToJobQueue(void *args){
  struct Packet packet;
  struct sockaddr_in  clientAddr;
  socklen_t clientAddrSize;
  int err, fd;

  clientAddrSize = sizeof(clientAddr);
  bzero((char *)&packet, sizeof(struct Packet));

  while(1) {

    fd = accept(socketFD, (struct sockaddr *)&clientAddr, &clientAddrSize);

    if (fd < 0){
        perror("ERROR on accept");
        exit(1);
    }

    err = read(fd, &packet, sizeof(struct Packet));
    packet.fd = fd;

    if (err < 0){
      perror("ERROR reading from socket");
      exit(1);
    }

    sem_wait(&emptyCount);
    sem_wait(&queueLock);
    // Critical section
    jobPos++;
    jobQueue[jobPos] = packet;

    sem_post(&queueLock);
    sem_post(&fillCount);
  }
  return NULL;
}

#include "sempacket.h"
#include <stdio.h>
#include "packet.h"
#include <semaphore.h>
#include "reversehashing.h"

sem_t* emptyCount;
sem_t* fillCount;
sem_t* queueLock;
int socketFS;

struct Packet* jobQueue;

int* jobPos;

void initConsumer(struct JobQueuePacket *jobQueuePacket){
  emptyCount = jobQueuePacket->jobEmptyCount;
  fillCount = jobQueuePacket->jobFillCount;
  queueLock = jobQueuePacket->jobQueueLock;
  jobQueue = jobQueuePacket->queue;
  jobPos = jobQueuePacket->jobPosition;

  printf("INITIALIZED CONSUMER\n");
}
void *consumeFromJobQueue(void *arg){
  struct Packet packet;
  int counter = *jobPos;
  int n;

  while(1) {

    sem_wait(fillCount);
    sem_wait(queueLock);
    packet = jobQueue[*jobPos];
    counter = *jobPos;
    counter--;
    *jobPos = counter;
    sem_post(queueLock);
    sem_post(emptyCount);

    uint64_t answer = reversehashing2(packet.start, packet.end, packet.hash);
    n = write(packet.fd, &answer, 8);

  }
}

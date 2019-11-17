
#include "sempacket.h"
#include "packet.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include "hashtable.h"
#include <unistd.h>

// #include <netinet/in.h>

// sem_t* emptyCount;
// sem_t* fillCount;
// sem_t* queueLock;
int socketFD;

// struct Packet* jobQueue;

// int* jobPos;


void initProducer(int sockFD){
  // emptyCount = queuePacket->jobEmptyCount;
  // fillCount = queuePacket->jobFillCount;
  // queueLock = queuePacket->jobQueueLock;
  // jobQueue = queuePacket->queue;
  // jobPos = queuePacket->jobPosition;
  socketFD = sockFD;
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

    answer = find(packet.hash);
    if (answer == 0){
      end = be64toh(packet.end);
      start = be64toh(packet.start);

      for (answer = start; answer <= end; answer++){
        SHA256((const unsigned char*) &answer, 8, testHash);
        if (memcmp(testHash, packet.hash, 32 * sizeof(uint8_t)) == 0) {
          insert(packet.hash, answer);
          break;
        }

      }
    }
    answer = htobe64(answer);
    err = write(fd, &answer, 8);
    if (err < 0) {
      perror("Error writing");
      exit(1);
    }
  }
  return NULL;
}

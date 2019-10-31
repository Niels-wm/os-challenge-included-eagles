// 
// #include "sempacket.h"
// #include "packet.h"
// #include <string.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <sys/socket.h>
// #include <netdb.h>
// // #include <netinet/in.h>
//
// sem_t* emptyCount;
// sem_t* fillCount;
// sem_t* queueLock;
// int socketFD;
//
// struct Packet* jobQueue;
//
// int* jobPos;
//
// void initProducer(struct JobQueuePacket *queuePacket, int sockFD){
//   emptyCount = queuePacket->jobEmptyCount;
//   fillCount = queuePacket->jobFillCount;
//   queueLock = queuePacket->jobQueueLock;
//   jobQueue = queuePacket->queue;
//   jobPos = queuePacket->jobPosition;
//   socketFD = sockFD;
//
//   printf("INITIALIZED PRODUCER\n" );
// }
//
// void *produceToJobQueue(void *args){
//   struct Packet packet;
//   struct sockaddr_in  clientAddr;
//   socklen_t clientAddrSize;
//   int err, fd;
//   int counter = *jobPos;
//
//   clientAddrSize = sizeof(clientAddr);
//   bzero((char *)&packet, sizeof(struct Packet));
//
//   while(1) {
//
//     fd = accept(socketFD, (struct sockaddr *)&clientAddr, &clientAddrSize);
//
//     if (fd < 0){
//         perror("ERROR on accept");
//         exit(1);
//     }
//
//     err = read(fd, &packet, sizeof(struct Packet));
//     packet.fd = fd;
//
//     if (err < 0){
//       perror("ERROR reading from socket");
//       exit(1);
//     }
//
//     sem_wait(emptyCount);
//     sem_wait(queueLock);
//     // Critical section
//     counter = *jobPos;
//     counter++;
//     *jobPos = counter;
//     jobQueue[*jobPos] = packet;
//
//     sem_post(queueLock);
//     sem_post(fillCount);
//   }
//   return NULL;
// }

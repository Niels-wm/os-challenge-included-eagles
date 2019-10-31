#include <stdio.h>
#include <stdlib.h>
#include "messages.h"
#include "packet.h"
#include "reversehashing.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include "consumer.h"
#include "sempacket.h"
#include <semaphore.h>

#define PORT 5003

int main(int argc, char *argv[]){
  int sockFileDescripter;
  struct sockaddr_in serverAddr, clientAddr;
  socklen_t clientAddrSize;
  int n, pid, err;

  initReverseHashing();

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
  listen(sockFileDescripter, 50);
  // initProducer(jobQueuePacket, sockFileDescripter);
  initConsumer(sockFileDescripter);

  int consumerCount = 10;
  pthread_t tIdProducer;
  pthread_t tIdsConsumers[consumerCount];

  for (size_t i = 0; i < consumerCount; i++) {
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

  pthread_join(tIdProducer, NULL);

  for (size_t i = 0; i < consumerCount; i++) {
    pthread_join(tIdsConsumers[i], NULL);
  }
}

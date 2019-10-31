#ifndef SEMPACKET_H
#define SEMPACKET_H

#include <semaphore.h>
#include "packet.h"

struct JobQueuePacket {
  sem_t* jobEmptyCount;
  sem_t* jobFillCount;
  sem_t* jobQueueLock;
  struct Packet* queue;
  int* jobPosition;
};

#endif

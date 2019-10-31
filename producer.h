#ifndef PRODUCER_H
#define PRODUCER_H

#include "sempacket.h"

void initProducer(struct JobQueuePacket* queuePacket, int sockFS);
void *produceToJobQueue(void *args);

#endif

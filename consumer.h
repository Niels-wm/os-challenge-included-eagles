#ifndef CONSUMER_H
#define CONSUMER_H

#include "sempacket.h"

void initConsumer(struct JobQueuePacket *jobQueuePacket);
void *consumeFromJobQueue(void *arg);

#endif 

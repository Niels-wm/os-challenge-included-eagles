#ifndef CONSUMER_H
#define CONSUMER_H

#include "sempacket.h"

void initConsumer(int fd);
void *consumeFromJobQueue(void *arg);
void *produceToJobQueue(void *args);

#endif

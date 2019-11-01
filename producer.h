#ifndef PRODUCER_H
#define PRODUCER_H

#include "sempacket.h"

void initProducer(int sockFS);
void *produceToJobQueue(void *args);

#endif

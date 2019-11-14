#ifndef NODE_H
#define NODE_H

#include "threadinfo.h"
#include "packet.h"

struct Node
{
    struct Packet packet;
    struct ThreadInfo* ti;
    struct Node* next;
};

#endif
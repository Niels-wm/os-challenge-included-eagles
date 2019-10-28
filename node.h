
#ifndef NODE_H
#define NODE_H

#include "packet.h"

struct Node {
    struct Packet data;
    int priority, fd;

    struct Node *next;
};

#endif

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "packet.h"

struct Node *createNewNode(struct Packet packet, uint8_t p, int fd);

struct Node peek(struct Node **head);

void pop(struct Node **head);

void push(struct Node **head, struct Packet d, uint8_t p, int fd);

int isEmpty(struct Node **head);
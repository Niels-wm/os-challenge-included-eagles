#include <stdio.h>
#include "stdlib.h"
#include <inttypes.h>
#include "strings.h"


typedef struct Node {
    struct Node *parent;
    struct Node *right;
    struct Node *left;
    uint8_t hash;
    int value;
} Node;

struct Node *root;

void splay(Node *node) {
    if (node == root) {
        return;
    }
    Node *p = node->parent;
    if (!p) {
        return;
    }

    Node *pp = node->parent->parent;
    Node *left = node->left;
    Node *right = node->right;

    // Zig operation
    if (p == root) {
        if (p->left == node) {
            if (pp != NULL) {
                if (pp->left == pp) {
                    pp->left = node;
                } else {
                    pp->right = node;
                }
            }
            if (node->right != NULL) {
                p->left = node->right;
                p->left->parent = p;
            } else {
                p->left = NULL;
            }

            node->right = p;
            node->parent = pp;
            node->right->parent = node;

        } else {
            if (pp != NULL) {
                if (pp->right == pp) {
                    pp->right = node;
                } else {
                    pp->left = node;
                }
            }
            if (node->left != NULL) {
                p->right = node->left;
                p->right->parent = p;
            } else {
                p->right = NULL;
            }

            node->left = p;
            node->parent = pp;
            node->left->parent = node;
        }
    } else if (p->left == node) {
        // Zig zig right rotation
        if (pp->left == p) {
            Node *ppp = pp->parent;
            node->parent = ppp;
            if (ppp != NULL) {
                if (ppp->left == pp) {
                    ppp->left = node;
                } else {
                    ppp->right = node;
                }
            }
            if (p->right != NULL) {
                pp->left = p->right;
                pp->left->parent = pp;
            } else {
                pp->left = NULL;
                p->parent = NULL;
            }

            if (node->right != NULL) {
                p->left = node->right;
                p->left->parent = p;
            } else {
                p->left = NULL;
            }
            p->right = pp; // 2
            pp->parent = p;
            node->right = p;
            p->parent = node;
        } else { // Zig zag Left-right
            Node *ppp = pp->parent;
            node->parent = ppp;

            if (ppp != NULL) {
                if (ppp->left == pp) {
                    ppp->left = node;
                } else {
                    ppp->right = node;
                }
                pp->parent = node;
            }

            if (node->left != NULL) {
                pp->right = node->left;
                pp->right->parent = p;
            } else {
                pp->left = NULL;
            }

            if (node->right != NULL) {
                p->left = node->right;
                p->left->parent = p;
            } else {
                p->left = NULL;
            }

            node->right = p;

            node->left = pp;
            node->right->parent = node;
            node->left->parent = node;
            node->parent = ppp;
        }
        splay(node);
    } else if (p->right == node) {
        // Zig zig
        if (pp->right == p) {
            if (pp->parent != NULL) {
                if (pp->parent->left == pp) {
                    pp->parent->left = node;
                } else {
                    pp->parent->right = node;
                }
            }
            if (p->left != NULL) {
                pp->right = p->left;
                pp->right->parent = pp;
            } else {
                pp->right = NULL; // 1
            }

            p->left = pp; // 2

            if (node->left != NULL) {
                p->right = node->left;
                node->left->parent = p;
            } else {
                p->right = NULL;
            }

            node->left = p;

            p->parent = node;
            node->parent = pp->parent;
            pp->parent = p;
        } else { // Zig zag Right-left
            Node *ppp = pp->parent;
            node->parent = ppp;

            if (ppp != NULL) {
                if (ppp->right == pp) {
                    ppp->right = node;
                } else {
                    ppp->left = node;
                }
                pp->parent = node;
            }

            if (node->right != NULL) {
                pp->left = node->right;
                pp->left->parent = p;
            } else {
                pp->left = NULL;
            }

            if (node->left != NULL) {
                p->right = node->left;
                p->right->parent = p;
            } else {
                p->right = NULL;
            }

            node->left = p;

            node->right = pp;
            node->right->parent = node;
            node->left->parent = node;
        }
        splay(node);
    }
    root = node;
}

void insert(uint8_t newHash, int value, struct Node *node) {
//    printf("%" PRIu8 "\n", newHash);

    if (!root) {
        root = (Node *) malloc(sizeof(Node));
        root->parent = NULL;
        root->right = NULL;
        root->left = NULL;
        root->hash = newHash;
        root->value = value;
        return;
    }

    if (node->hash > newHash) {
        if (!node->left) {
            node->left = (Node *) malloc(sizeof(Node));
            node->left->parent = node;
            node->left->hash = newHash;
            node->left->value = value;
            splay(node->left);
        } else {
            insert(newHash, value, node->left);
        }
    } else {
        if (!node->right) {
            node->right = (Node *) malloc(sizeof(Node));
            node->right->parent = node;
            node->right->hash = newHash;
            node->right->value = value;
            splay(node->right);
        } else {
            insert(newHash, value, node->right);
        }
    }


}

int find(uint8_t n, struct Node *node) {
    if (node->hash > n) {
        if (!node->right) {
            splay(node);
        } else {
            find(n, node->right);
        }
    } else if (node->hash < n) {
        if (!node->left) {
            splay(node);
        } else {
            find(n, node->left);
        }
    } else {
        splay(node);
        return node->value;
    }

    return -1;
}


int main(int argc, char *argv[]) {
    printf("hej");
}
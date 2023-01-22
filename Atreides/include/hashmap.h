#ifndef _HASHMAP_H
#define _HASHMAP_H

#include "../include/utils.h"
#include "../include/bidirectionallist.h"

typedef struct node{
    int key;
    BidirectionalList val;
    struct node *next;
}node;

typedef struct hashmap{
    int size;
    node **list;
}hashmap;

hashmap *HASMAP_createTable(int size);
int HASMAP_hashCode(hashmap *t, int key);
void HASMAP_insert(hashmap *t, int key, user val);
BidirectionalList HASMAP_lookup(hashmap *t, int key);

#endif
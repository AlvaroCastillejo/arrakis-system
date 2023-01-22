#ifndef _BIDIRECTIONALLIST_H
#define _BIDIRECTIONALLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/utils.h"

#define LIST_NO_ERROR 0
#define LIST_ERROR_EMPTY 2			// Error, the list is empty.
#define LIST_ERROR_MALLOC 3			// Error, a malloc failed.
#define LIST_ERROR_END 4			// Error, the POI is at the tail.
#define LIST_ERROR_START 5			// Error, the POI is at the head.
#define LIST_ERROR_INVALID 6		// Error, the POI is on a phantom node.

#define MAX_BUFFER 100

typedef struct _Node {
	user element;
	struct _Node * next;
	struct _Node * previous;
} Node;

typedef struct {
	int error;
	Node * head;
	Node * tail;
	Node * poi;
	int size;
} BidirectionalList;

BidirectionalList BIDIRECTIONALLIST_create();

void	BIDIRECTIONALLIST_addBefore(BidirectionalList * list, user element);
void	BIDIRECTIONALLIST_addAfter (BidirectionalList * list, user element);
user	BIDIRECTIONALLIST_get(BidirectionalList * list);
void	BIDIRECTIONALLIST_remove(BidirectionalList * list);
int		BIDIRECTIONALLIST_isEmpty(BidirectionalList list);

int 	BIDIRECTIONALLIST_isValid(BidirectionalList list);

void	BIDIRECTIONALLIST_goToHead(BidirectionalList * list);
void	BIDIRECTIONALLIST_next(BidirectionalList * list);

void	BIDIRECTIONALLIST_goToTail(BidirectionalList * list);
void	BIDIRECTIONALLIST_previous(BidirectionalList * list);

void 	BIDIRECTIONALLIST_destroy(BidirectionalList * list);

int		BIDIRECTIONALLIST_getErrorCode(BidirectionalList list);


#endif
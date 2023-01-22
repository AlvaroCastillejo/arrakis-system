#ifndef _BIDIRECTIONALLIST_H_
#define _BIDIRECTIONALLIST_H_

#include <stdlib.h>

#define LIST_NO_ERROR 0
#define LIST_ERROR_EMPTY 2			// Error, the list is empty.
#define LIST_ERROR_MALLOC 3			// Error, a malloc failed.
#define LIST_ERROR_END 4			// Error, the POI is at the tail.
#define LIST_ERROR_START 5			// Error, the POI is at the head.
#define LIST_ERROR_INVALID 6		// Error, the POI is on a phantom node.

#define MAX_BUFFER 100

typedef struct{
    char** argv;
    int argc;
    char raw[MAX_BUFFER];
} Command;

typedef struct _Node {
	Command element;
	struct _Node * next;
	struct _Node * previous;
} Node;

typedef struct {
	int error;
	Node * head;
	Node * tail;
	Node * poi;
} BidirectionalList;

BidirectionalList BIDIRECTIONALLIST_create();

void	BIDIRECTIONALLIST_addBefore(BidirectionalList * list, Command element);
void	BIDIRECTIONALLIST_addAfter (BidirectionalList * list, Command element);
Command	BIDIRECTIONALLIST_get(BidirectionalList * list);
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


#ifndef _STORAGE_H
#define _STORAGE_H

#include "../include/utils.h"
#include "../include/hashmap.h"

int STORAGE_loadDatabase(char* path);
int STORAGE_getID(user u);
int STORAGE_addUser(user u);
hashmap* STORAGE_getStorage();
void STORAGE_deleteDatabase();
int STORAGE_registerUser(user u, char* path);
BidirectionalList STORAGE_lookupUsrsIN(int zipcodeToSearch);
#endif
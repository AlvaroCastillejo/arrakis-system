#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "../include/network.h"
#include "../include/utils.h"

typedef struct{
    int id;
    pthread_t tid;
    int clientFD;
} ThreadArg;

void* THREAD_userThread(void* arg);
pthread_t THREAD_create(ThreadArg arg);
void THREAD_deleteThreads(pthread_t* ids, int total);

#endif
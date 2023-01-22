#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "../include/utils.h"
#include "../include/network.h"

typedef struct{
    int id;
    pthread_t tid;
    int clientFD;
} ThreadArg;

void* THREAD_localFileCleanUp(void* arg);


#endif
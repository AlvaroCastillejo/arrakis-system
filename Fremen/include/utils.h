#ifndef _UTILS_H
#define _UTILS_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <strings.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/semaphore_v2.h"
#include "../include/bidirectionallist.h"

typedef struct {
    int countdown;
    char* ip;
    int port;
    char* programFolder;
} Configuration;

char* UTILS_read_until(int fd, char end);
char * UTILS_read_until_(char buffer[],int *i, char end, int argc);
int UTILS_countChars(char buffer[], char target);
Configuration UTILS_readConfiguration(char* argv);
void UTILS_cprint(char* buffer);
void UTILS_toLower_(char buffer[]);
char * UTILS_md5sum (char * filename);
char * UTILS_get_filename_ext(const char *filename);
int UTILS_localFileCleanUp();

#endif

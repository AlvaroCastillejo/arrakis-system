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
#include <sys/stat.h>

#include "../include/semaphore_v2.h"

#define MAX_ORIGIN_SIZE 15
#define MAX_DATA_SIZE 240

#define MAX_BUFFER 100

#define USR_REG_PATH "build/usr_reg.txt"
#define MAX_ZIPCODE 53000

typedef struct{
    char origin[MAX_ORIGIN_SIZE];
    char type;
    char data[MAX_DATA_SIZE];
}Packet;

typedef struct {
    char* ip;
    int port;
    char* programFolder;
} Configuration;

typedef struct user{
    char login[MAX_DATA_SIZE];
    int zipcode;
    int id;
    int connected;
}user;

char* UTILS_read_until(int fd, char end);
int UTILS_read_until_(char buffer[],char* string, int i, char end);
int UTILS_countChars(char buffer[], char target);
Configuration UTILS_readConfiguration(char* argv);
void cprint(char* buffer);
void UTILS_toLower_(char buffer[]);
char* UTILS_itoa(int num, char* str, int base);
char * UTILS_md5sum (char * filename);
#endif
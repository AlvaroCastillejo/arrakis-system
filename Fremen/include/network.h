#ifndef _NETWORK_H
#define _NETWORK_H

#include "../include/utils.h"

#define MAX_ORIGIN_SIZE 15
#define MAX_DATA_SIZE 240

typedef struct{
    char origin[MAX_ORIGIN_SIZE];
    char type;
    char data[MAX_DATA_SIZE];
}Packet;

int NETWORK_connectServer(Configuration config);
int NETWORK_send(Command command);
int NETWORK_establishConnection(int fd_server, Command command);
void NETWORK_disconnect ();
int NETWORK_getServerFD();
void NETWORK_sendSearch(int fd_server, char* zipcode, int usr_id);
void NETWORK_setDisconnected();
int NETWORK_isConnected();
int NETWORK_sendSend(int fd_server, char* filename);
int NETWORK_sendPhoto(int fd_server, char * photo_id);

#endif
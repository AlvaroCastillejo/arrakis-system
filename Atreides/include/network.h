#ifndef _NETWORK_H
#define _NETWORK_H

#define _GNU_SOURCE
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
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

#include "../include/utils.h"
#include "../include/login.h"

#define ERR_BIND 2
#define ERR_LISTEN 3

int clientFD;
struct sockaddr_in servidor;
int listenFD;

int NETWORK_startAtreides(Configuration config);
void NETWORK_stopAtreides();
int NETWORK_checkMessage(Packet pkt, int fd_server, user u);
void NETWORK_broadcastLogout(int *connected_fds, int total);
#endif
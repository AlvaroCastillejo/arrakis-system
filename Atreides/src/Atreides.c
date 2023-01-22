#include <stdio.h>
#include "../include/utils.h"
#include "../include/network.h"
#include "../include/thread.h"

pthread_t* ids;
int *connected_fds;
int total_threads = 0;
semaphore sem_message;

//helps to free memory space before closing 
void exitAtreides(){
    write(1,"\nExiting...\n",12);
    NETWORK_broadcastLogout(connected_fds, total_threads);
    STORAGE_deleteDatabase();
    NETWORK_stopAtreides();
    SEM_destructor(&sem_message);
    THREAD_deleteThreads(ids, total_threads);
    //Limpiar
    exit(0);
}

int main(int argc, char** argv){
    char buffer[MAX_BUFFER];
    Configuration config;

	if (argc != 2) {
        sprintf(buffer, "Error: missing arguments\n");
        write(1, buffer, strlen(buffer));
        exit(-1);
    }

    SEM_constructor_with_name(&sem_message, ftok("Atreides.c",'a'));
    SEM_init(&sem_message, 1);

    cprint("\nSERVIDOR ATREIDES\n");

    config = UTILS_readConfiguration(argv[1]);

    signal(SIGINT, exitAtreides);

    int r = NETWORK_startAtreides(config);
    if(r != 1){
        exitAtreides();
    }

    if(STORAGE_loadDatabase(USR_REG_PATH) < 0){
        exitAtreides();
    }

    ids = (pthread_t*)malloc(sizeof(pthread_t));
    connected_fds = (int*)malloc(sizeof(int));
    ThreadArg arg;
    for(int id = 0;;id++){
        cprint("Esperant connexions...\n\n");
        clientFD = accept(listenFD, (struct sockaddr*) NULL, NULL);
        arg.id = id;
        arg.clientFD = clientFD;
        //creates the dedicated server
        pthread_create(&arg.tid, NULL, THREAD_userThread, &arg);
        ids[id] = arg.tid;
        //saves the client fd 
        connected_fds[id] = clientFD;
        total_threads++;
        ids = (pthread_t*)realloc(ids, sizeof(pthread_t)*(id+1));
        connected_fds = (int*)realloc(connected_fds, sizeof(int)*(id+1));

        sprintf(buffer,"\nNou client connectat amb fd %d\n",clientFD);
        cprint(buffer);
    }
}

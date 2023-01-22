#include "../include/thread.h"
#include "../include/utils.h"
//Dedicated server for a client
void* THREAD_userThread(void* arg){
    Packet pkt;
    char buffer[MAX_DATA_SIZE];
    user u;

    semaphore sem_message;
    SEM_constructor_with_name(&sem_message, ftok("Atreides.c", 'a'));

    ThreadArg aux = *((ThreadArg *) arg);
    
    for(;;){
        //read client packet
        if(read(aux.clientFD,&pkt, sizeof(Packet)) < 0){
            sprintf(buffer, "Error: Couldn't receive packet from client %d\n",aux.id);
            cprint(buffer);
        } else {
            SEM_wait(&sem_message);
            //process the client packet
            int ret = NETWORK_checkMessage(pkt, aux.clientFD, u); 
            if (ret < 0) break;
            if(pkt.type == 'C'){
                u.id = ret;
                memset(u.login, '\0',strlen(u.login));
                int i = 0;
                for(; pkt.data[i] != '*'; i++){
                    u.login[i] = pkt.data[i];
                }
                i++;
                memset(buffer,'\0',strlen(buffer));
                int j = 0;
                for(; pkt.data[i] != '\0' && i < MAX_DATA_SIZE; i++){
                    buffer[j] = pkt.data[i];
                    j++;
                }
                
                u.zipcode = atoi(buffer);
                u.connected = 1;
            }
            SEM_signal(&sem_message);
        }

    }
    
    //for(;;) sleep(20);
    return NULL;
}

pthread_t THREAD_create(ThreadArg arg){
    char buffer[MAX_BUFFER];
    int num = arg.clientFD;
    sprintf(buffer, "thread create FD:  %d\n", num);
    cprint(buffer);
    cprint("??????\n");
    pthread_create(&arg.tid, NULL, THREAD_userThread, &arg);
    return arg.tid;
}

//Delete threads working
void THREAD_deleteThreads(pthread_t* ids, int total){
    
    int len = total;
    for(int i = 0; i < len; i++){
        int ret = pthread_cancel(ids[i]);
        if(ret){
            //sprintf(buffer, "Error: Failed to cancel thread with id %ld.\n", ids[i]);
            //cprint(buffer);
        }
    }
}
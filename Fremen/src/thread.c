#include "../include/thread.h"
semaphore sem_localFileCleanUp;
void* THREAD_localFileCleanUp(void* arg){

    int countdown = *((int *) arg);

    
	SEM_constructor_with_name(&sem_localFileCleanUp, ftok("Fremen.c", 'z'));
    char buffer[100];
    while (1) {
        sprintf(buffer, "%d\n", countdown);
        //UTILS_cprint(buffer);
        sleep(countdown);
        UTILS_cprint("");
        SEM_wait(&sem_localFileCleanUp);
        UTILS_cprint("");
        int ret = UTILS_localFileCleanUp();
        sprintf(buffer, "%d\n", ret);
        SEM_signal(&sem_localFileCleanUp);
        UTILS_cprint("");
    }    

    return NULL;
}


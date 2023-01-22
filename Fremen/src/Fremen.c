#include "../include/utils.h"
#include "../include/commands.h"
#include "../include/thread.h"

#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE    
//Network
int fd_server;
pthread_t thread_id;
int usr_id;
Configuration config;
Command command;
semaphore sem_localFileCleanUp;

void exitFremen(){
    write(1,"\nExiting...\n",12);
    COMMANDS_destroyBufferedCommands();
    NETWORK_disconnect();
    close(fd_server);
    free(config.ip);
    free(config.programFolder);
    for (int i = 0; i < (command.argc); i++) {
        free(command.argv[i]);
        //command.argv[i] = NULL;
    }
    free(command.argv);
    SEM_destructor(&sem_localFileCleanUp);
    //shutdown(fd_server,2);
    exit(0);
}


int main(int argc, char** argv){
    //utils
    char buffer[MAX_BUFFER] = "";
    
    usr_id = -1;

    SEM_constructor_with_name(&sem_localFileCleanUp, ftok("Fremen.c", 'z'));
    SEM_init(&sem_localFileCleanUp, 2);
    
    
    //Check arguments
    if (argc != 2) {
        sprintf(buffer, "Error: missing arguments\n");
        write(1, buffer, strlen(buffer));
        exit(-1);
    }

    config = UTILS_readConfiguration(argv[1]);


    signal(SIGINT, exitFremen);
    
    pthread_create(&thread_id, NULL, THREAD_localFileCleanUp, &config.countdown);
    UTILS_cprint("Benvingut a Fremen\n");
    while(1){
        SEM_wait(&sem_localFileCleanUp);
        UTILS_cprint("$ ");
    	if(read(0, buffer, MAX_BUFFER) < 0){
            write(2, "An error occurred in the read.\n", 31);
            continue;
		}
        if(strlen(buffer)==1){
            continue;
        }
        
        COMMANDS_normalizeCommand(buffer);
		UTILS_toLower_(buffer);

        command = COMMANDS_getCommand(buffer);
        sprintf(command.raw,"%s",buffer);
        memset(buffer,'\0',MAX_BUFFER);
        int ret = COMMANDS_executeCommand(command, config, usr_id);
        SEM_signal(&sem_localFileCleanUp);
        if(ret > 0){
            usr_id = ret;
            fd_server = NETWORK_getServerFD();
            
        }
	}

	return 0;
}
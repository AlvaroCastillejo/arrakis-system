#include "../include/commands.h"

Command last;
BidirectionalList command_buffer;
int fd_server;

void COMMANDS_destroyBufferedCommands(){
    BIDIRECTIONALLIST_destroy(&command_buffer);
}

void showCommandBuffer(){
    char aux[MAX_BUFFER*2];
    //Not necessary
    BIDIRECTIONALLIST_goToTail(&command_buffer);
    UTILS_cprint("Showing buffered commands\n");
    UTILS_cprint("\t1 - showbuffer\n");
    for(int i = 2; BIDIRECTIONALLIST_isValid(command_buffer); i++){
        sprintf(aux,"\t%d - %s",i,BIDIRECTIONALLIST_get(&command_buffer).raw);
        UTILS_cprint(aux);
        BIDIRECTIONALLIST_previous(&command_buffer);
    }
    
    BIDIRECTIONALLIST_goToTail(&command_buffer);
}

int COMMANDS_executeCommand(Command command, Configuration config, int usr_id){
    
    if(last.argc == 0){
        command_buffer = BIDIRECTIONALLIST_create();
    }

    if(strcmp(command.argv[0],"login") == 0){
        if(command.argc < 3){
            UTILS_cprint("Error: missing arguments for \"login\".\n\tUsage: login <username> <postcode>\n");
            if(!ALLOW_ERR_STORE) return -1;
        } else if(command.argc > 3){
            UTILS_cprint("Error: too many arguments for \"login\".\n\tUsage: login <username> <postcode>\n");
            if(!ALLOW_ERR_STORE) return -1;
        } else {
            if(NETWORK_isConnected() == 1){
                UTILS_cprint("Already connected to the server!\n");
                return 0;
            }
            fd_server = NETWORK_connectServer(config);
            if(fd_server < 0) {
                UTILS_cprint("Error. Server seems to be down.\n");
                return 0;
            }

            
            UTILS_cprint("Connectat a Atreides\n");

            int id = NETWORK_establishConnection(fd_server,command);
            if(id < 0){
                //Failed. Possible problem with sockets. Exiting Fremen.
                raise(SIGINT);
            }
            last = command;
            BIDIRECTIONALLIST_goToTail(&command_buffer);
            BIDIRECTIONALLIST_addAfter(&command_buffer, command);
            BIDIRECTIONALLIST_next(&command_buffer);
            return id;
        }
    } else if (strcmp(command.argv[0],"search") == 0){
        if(command.argc < 2){
            UTILS_cprint("Error: missing arguments for \"search\".\n\tUsage: search <postcode>\n");
            if(!ALLOW_ERR_STORE) return -1;
        } else if(command.argc > 2){
            UTILS_cprint("Error: too many arguments for \"search\".\n\tUsage: search <postcode>\n");
            if(!ALLOW_ERR_STORE) return -1;
        }  else {
            NETWORK_sendSearch(fd_server, command.argv[1], usr_id);
            //return 0;
        }
    } else if (strcmp(command.argv[0],"send") == 0){
        if(command.argc < 2){
            UTILS_cprint("Error: missing arguments for \"send\".\n\tUsage: send <file.ext>\n");
            if(!ALLOW_ERR_STORE) return -1;
        } else if(command.argc > 2){
            UTILS_cprint("Error: too many arguments for \"send\".\n\tUsage: send <file.ext>\n");
            if(!ALLOW_ERR_STORE) return -1;
        } else {
            NETWORK_sendSend(fd_server, command.argv[1]);
            
        }
    } else if (strcmp(command.argv[0],"photo") == 0){
        if(command.argc < 2){
            UTILS_cprint("Error: missing arguments for \"photo\".\n\tUsage: photo <id>\n");
            if(!ALLOW_ERR_STORE) return -1;
        } else if(command.argc > 2){
            UTILS_cprint("Error: too many arguments for \"photo\".\n\tUsage: photo <id>\n");
            if(!ALLOW_ERR_STORE) return -1;
        } else {
            NETWORK_sendPhoto(fd_server, command.argv[1]);
            
        }
    } else if (strcmp(command.argv[0],"logout") == 0){
        if(command.argc != 1){
            UTILS_cprint("Error: too many arguments for \"logout\".\n\tUsage: logout\n");
            if(!ALLOW_ERR_STORE) return -1;
        } else {
            UTILS_cprint("Desconnectat d'Atreides. Dew!\n");
            raise(SIGINT); 

        }
    } else if(strcmp(command.argv[0],"checkconfig") == 0 ){
        if(command.argc != 1){
            UTILS_cprint("Error: too many arguments for \"checkconfig\".\n\tUsage: checkconfig\n");
            if(!ALLOW_ERR_STORE) return -1;
        } else {
            char aux_[100];
            UTILS_cprint("- Config.dat -\n");
            sprintf(aux_, "\tcountdown: %d\n\tIP: %s\n\tPORT: %d\n\tprogramFolder: %s\n",config.countdown, config.ip, config.port,config.programFolder);
            UTILS_cprint(aux_);
        }
    } else if(strcmp(command.argv[0], "last") == 0){
        if(last.argc == 0) return 0;
        if(command.argc > 2){
            UTILS_cprint("Error: too many arguments for \"last\".\n\tUsage: last <index>\n");
            if(!ALLOW_ERR_STORE) return -1;
        }
        char aux[200];
        if(command.argc == 1){
            sprintf(aux, "Executing -> %s", BIDIRECTIONALLIST_get(&command_buffer).raw);
            UTILS_cprint(aux);

            COMMANDS_executeCommand(BIDIRECTIONALLIST_get(&command_buffer), config, usr_id);
        } else {
            int index = atoi(command.argv[1])-1;
            for(int i = 0; i < index; i++){
                BIDIRECTIONALLIST_previous(&command_buffer);
            }
            sprintf(aux, "Executing -> %s", BIDIRECTIONALLIST_get(&command_buffer).raw);
            UTILS_cprint(aux);

            COMMANDS_executeCommand(BIDIRECTIONALLIST_get(&command_buffer), config, usr_id);
            BIDIRECTIONALLIST_goToTail(&command_buffer);
        }
        //Cannot be stored to prevent "last" command loops.
        return 0;
    } else if((strcmp(command.argv[0], "showbuffer") == 0) || (strcmp(command.argv[0], "sb") == 0)){
        if(last.argc == 0){
            UTILS_cprint("No commands yet\n");
            return 0;
        }
        showCommandBuffer();
        //UTILS_cprint("showing command buffer...\n");
        if(!ALLOW_SBC_STORE) return 0;
    } else {
        int fd = fork();
        if(fd == -1){
            UTILS_cprint("An error occurred\n");
            return -1;
        }
        if(fd == 0){
            //child
            //execl("/bin/bash", "/bin/bash", "-c", command.raw, 0);
            execvp(command.argv[0], command.argv);
        } else {
            //daddy :$ v:
            wait(NULL);
        }
    }
    last = command;
    BIDIRECTIONALLIST_goToTail(&command_buffer);
    BIDIRECTIONALLIST_addAfter(&command_buffer, command);
    BIDIRECTIONALLIST_next(&command_buffer);
    return 0;
}

char** COMMANDS_getCommandArgv(char buffer[]){
    int argc = UTILS_countChars(buffer, ' ')+1;
    char** argv = malloc(sizeof(argv)*argc);
    int i = 0;
    char* string;
    for(int j = 0; j < argc; j++){
        //returned i: current index buffer
        string = UTILS_read_until_(buffer,&i,' ', argc);
        //skip  extra spaces
        argv[j] = string;
        if(argv[j][strlen(argv[j])-1] == '\n'){
            argv[j][strlen(argv[j])-1] = '\0';
        }
        i++;
        //string = realloc(string, sizeof(char));
    }
    //free(string);
    return argv;
}

void deleteDuplicatedSpaces(char buffer[]){
    // When string is empty, return
    if (buffer[0] == '\0')
        return;

    // if the adjacent characters are same
    if (buffer[0] == buffer[1] && buffer[0] == ' ') {

        // Shift character by one to left
        int i = 0;
        while (buffer[i] != '\0') {
            buffer[i] = buffer[i + 1];
            i++;
        }

        // Check on Updated String S
        deleteDuplicatedSpaces(buffer);
    }

    // If the adjacent characters are not same
    // Check from S+1 string address
    deleteDuplicatedSpaces(buffer + 1);
}

void COMMANDS_normalizeCommand(char buffer[]){
    deleteDuplicatedSpaces(buffer);
    if(buffer[0] == ' '){
        for(int i = 0; buffer[i] != '\0'; i++){
            buffer[i] = buffer[i+1];
        }
    }
}

Command COMMANDS_getCommand(char buffer[]){
    Command command;
    sprintf(command.raw,"%s",buffer);
    //command.raw = buffer;
    command.argv = COMMANDS_getCommandArgv(buffer);
    command.argc = UTILS_countChars(buffer, ' ')+1;
    //command.argc = getCommandArgc(buffer);
    return command;
}

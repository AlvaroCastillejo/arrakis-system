#include <stdio.h>
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

#define PRINT(x) write(1,x,strlen(x));

int getPidToAtack(){
    //char buffer[3000];
    int fd_pipe[2];
    int des_p[2];

    if (pipe(fd_pipe) < 0) {
        perror("Pipe1 failed");
        exit(1);
    }

    if(pipe(des_p) == -1) {
        perror("Pipe2 failed");
        exit(1);
    }

    if(fork() == 0){            //first fork
    
        close(STDOUT_FILENO);  //closing stdout
        dup(des_p[1]);         //replacing stdout with pipe write 
        close(des_p[0]);       //closing pipe read
        close(des_p[1]);
        close(fd_pipe[0]);
        close(fd_pipe[1]);

        char* const prog1[] = { "ps", "-u", 0};
        execvp(prog1[0], prog1);
        perror("execvp of ps failed");
        exit(1);
    }

    if(fork() == 0){            //creating 2nd child
    
        close(STDIN_FILENO);   //closing stdin
        dup(des_p[0]);         //replacing stdin with pipe read
        close(des_p[1]);       //closing pipe write
        close(des_p[0]);

        close(fd_pipe[0]);
        dup2(fd_pipe[1],STDOUT_FILENO);
        close(fd_pipe[1]);

        char* const prog2[] = { "grep", "Fremen", 0};
        execvp(prog2[0], prog2);
        perror("execvp of grep failed");
        exit(1);
    }

    close(des_p[0]);
    close(des_p[1]);
    
    wait(0);
    wait(0);
    
    close(fd_pipe[1]);
    //write(1, "buffefr: ",8);
    char c[2];
    int *pids = (int*)malloc(sizeof(int));
    char pidBuff[50];
    int i = 0;
    for(;;i++){
        memset(pidBuff,'\0',50);
        int byte = read(fd_pipe[0], &c, 1);
        if (byte == 0) break;
        for(;c[0] != ' ';){
            read(fd_pipe[0], &c, 1);
        }
        for(;c[0] == ' ';){
            read(fd_pipe[0], &c, 1);
        }
        //c contains first digit of the pid already
        pidBuff[0] = c[0];
        for(int j = 1;c[0] != ' ';j++){
            read(fd_pipe[0], &c, 1);
            pidBuff[j] = c[0];
        }
        //write(1, pidBuff, strlen(pidBuff));
        pids[i] = atoi(pidBuff);
        int * tmp = (int *)realloc(pids,sizeof(int)*(i+1));
        pids = tmp;
        int n = 0;
        for(;c[0] != '\n';){
            n = read(fd_pipe[0], &c, 1);
            if (n == 0) {
                break;
            }
        }
        if(n == 0) break;
    }
    pids[i] = -1;
    char abuff[50] = "ini";
    
    write(1,"Fremen pids found: \n",20);
    for(int j = 0;j<(i-1);j++){
        sprintf(abuff,"- pid: %d\n", pids[j]);
        write(1, abuff, strlen(abuff));
    }

    //only have the grep process pid
    if(i==1){
        close(fd_pipe[0]);
        free(pids);
        return -1;
    }

    srand( time(NULL) );
    int pidIndex = rand() % (i-1);
    int toReturn = pids[pidIndex];
    free(pids);
    //write(1, buffer, 3000);
    close(fd_pipe[0]);

    return toReturn;
}

void killPID(int pid){
    if(fork() == 0){            //fork
        char buffer[20] = "i";
        sprintf(buffer, "%d", pid);
        close(STDOUT_FILENO);  //closing stdout

        char* const prog1[] = { "kill", "-9", buffer , 0};
        execvp(prog1[0], prog1);
        perror("kill failed");
        exit(1);
    }
    wait(0);
}

int main(int argc, char *argv[]){
    //char* buffer;
    char buffer[300] = "ini";
    
    //char* const args[]={"ps", "-u", "alvaro.castillejo", "|", "grep", "top", "|", "awk", "'{ print $1 }'",0};
    //char* const args[]={"ps","-u",0};

    int fd_pipe[2];

    if (argc != 2){
        write(1, "No time specified!\n",19);
        exit(-1);   
    }

    if (pipe(fd_pipe) < 0) {
        perror("pipe");
        exit(-2);
    }

    PRINT("Starting Harkonen...\n")
    while(1){
        PRINT("Scanning pids...\n")
        int pidToAtack = 0;
        pidToAtack = getPidToAtack();
        if(pidToAtack < 0){
            write(1,"No fremen found\n",16);
            sleep(atoi(argv[1]));
            continue;
        }
        killPID(pidToAtack);
        sprintf(buffer,"killing pid %d\n",pidToAtack);
        write(1,buffer,strlen(buffer));
        sleep(atoi(argv[1]));
    }
    
    //free(buffer);
    return 0;
}
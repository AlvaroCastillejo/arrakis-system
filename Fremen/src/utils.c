#include <stdio.h>
#include <dirent.h>
#include "../include/utils.h"


char* UTILS_read_until(int fd, char end) {
    int i = 0, size;
    char c = '\0';
    char* string = (char*)malloc(sizeof(char));

    while (1) {
        size = read(fd, &c, sizeof(char));

        if (c != end && size > 0) {
            string = (char*)realloc(string, sizeof(char) * (i + 2));
            string[i++] = c;
        } else {
            i++; //Pel \0
            break;
        }
    }

    string[i - 1] = '\0';
    return string;
}

//Homemade read_until that work with a given index
char * UTILS_read_until_(char buffer[], int *i, char end, int argc){
    int j = 0;
    int size = strlen(buffer);
    char *string = (char*)malloc(sizeof(char));
    while(1){
        if(buffer[*i] != end && *i < size){
            string = (char*)realloc(string, sizeof(string) * (j + 2));
            //int j = i;
            string[j] = buffer[*i];
            j++;
            *i = *i + 1;
        } else {
            if (argc == 1) {
               string[j-1] = '\0'; 
            }else {
                string[j] = '\0';
            }
            break;
        }
    }
    return string;
}

//this function counts chars :)
int UTILS_countChars(char buffer[], char target){
    int count = 0;
    for(int i = 0; i < strlen(buffer); i++){
        if(buffer[i] == target) count++;
    }
    return count;
}
//reads the configuration file
Configuration UTILS_readConfiguration(char* argv){
    char buffer[MAX_BUFFER];
    int fd = open(argv, O_RDONLY);
    if(fd < 0){
        sprintf(buffer, "Error: file \"%s\" doesn't exist\n", argv);
        write(1, buffer, strlen(buffer));
        exit(-1);
    }

    Configuration config;
    char * countdown = UTILS_read_until(fd,'\n');
    config.countdown = atoi(countdown);
    config.ip = UTILS_read_until(fd,'\n');
    char * port = UTILS_read_until(fd,'\n');
    config.port = atoi(port);
    config.programFolder = UTILS_read_until(fd,'\n');

    free(countdown);
    free(port);
    close(fd);
    return config;
}

//homemade print
void UTILS_cprint(char* buffer){
    write(1,buffer,strlen(buffer));
    return;
}

//Transform any single letter in a quote/word in lower case 
void UTILS_toLower_(char buffer[]){
    for(int i = 0; i <= strlen(buffer) && buffer[i] != ' ';i++){
        if(buffer[i] >= 65 && buffer[i] <= 90)
            buffer[i] = buffer[i]+32;
    }
    return;
}

//Calculates md5sum of a file
char * UTILS_md5sum (char * filename) {
    char * toReturn;
    char buffer[300];
    char* const args[]={"md5sum",filename,0};
    int fd_pipe[2];

    if (pipe(fd_pipe) < 0) {
        perror("pipe");
        return NULL;
    }
    
    pid_t pid=fork();

    if(!pid) {
         close(fd_pipe[0]);
         dup2(fd_pipe[1],STDOUT_FILENO);
         execvp(args[0], args);
    } else {
        wait(NULL);
       
        close(fd_pipe[1]);
        read(fd_pipe[0], &buffer, 300);
        toReturn = malloc(sizeof(toReturn));
        for(int i = 0; buffer[i] != ' '; i++) {
            toReturn[i] = buffer[i];
            toReturn = realloc(toReturn, sizeof(toReturn)*(i+1));
        }
        toReturn[strlen(toReturn)] = '\0';
    }
    
    return toReturn;

}

//Gets extension from file
char * UTILS_get_filename_ext(const char *filename) {
     char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

//Cleans the images folder
int UTILS_localFileCleanUp() {

    struct dirent *d;
    DIR *dir;
    char buf[1000];
    char * file;
    dir = opendir("imgs");
    while((d = readdir (dir)) != NULL) {          
            file = d->d_name;
            sprintf(buf, "imgs/%s", file);
            char * ext = UTILS_get_filename_ext(buf);
            if (strcmp(ext, "jpg") == 0) {
                remove(buf);
            }
    }
    return 1;

}

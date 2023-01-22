#include <stdio.h>
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
int UTILS_read_until_(char buffer[],char* string, int i, char end){
    int j = 0;
    int size = strlen(buffer);
    while(1){
        if(buffer[i] != end && i < size){
            string = (char*)realloc(string, sizeof(char) * (j + 2));
            //int j = i;
            string[j++] = buffer[i];
            i++;
        } else {
            j++;
            break;
        }
    }
    string[j-1] = '\0';
    return i;
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
    int fd = open(argv, O_RDONLY);
    char buffer[100];
    if(fd < 0){
        sprintf(buffer, "Error: file \"%s\" doesn't exist\n", argv);
        write(1, buffer, strlen(buffer));
        exit(-1);
    }

    Configuration config;
    config.ip = UTILS_read_until(fd,'\n');
    config.port = atoi(UTILS_read_until(fd,'\n'));
    config.programFolder = UTILS_read_until(fd,'\n');
    close(fd);
    cprint("LLegit el fitxer de configuració\n");
    return config;
}
//homemade print
void cprint(char* buffer){
    write(1,buffer,strlen(buffer));
    return;
}
//Transform any single letter in a quote/word in lower case 
void UTILS_toLower_(char buffer[]){
    for(int i = 0; i <= strlen(buffer);i++){
        if(buffer[i] >= 65 && buffer[i] <= 90)
            buffer[i] = buffer[i]+32;
    }
    return;
}

void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}
//itoa
char* UTILS_itoa(int value, char* buffer, int base){
    if (base < 2 || base > 32) {
        return buffer;
    }

    // consider the absolute value of the number
    int n = abs(value);

    int i = 0;
    while (n)
    {
        int r = n % base;

        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }

        n = n / base;
    }

    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }

    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0'; // null terminate string

    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
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
        toReturn = malloc(sizeof(char));
        for(int i = 0; buffer[i] != ' '; i++) {
            toReturn[i] = buffer[i];
            toReturn = realloc(toReturn, sizeof(char)*(i+1));
        }
        toReturn[strlen(toReturn)] = '\0';
    }
    
    return toReturn;

}
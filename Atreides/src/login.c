#include "../include/login.h"

    //return id, if error return -1
int LOGIN_loginOK(Packet pkt){
    //If it was already registered it returns its id.
    user u;
    char tok[100];
    int i = 0;
    for(i = 0; pkt.data[i] != '*'; i++){
        tok[i] = pkt.data[i];
    }
    sprintf(u.login,"%s",tok);
    i++;
    int k = 0;
    memset(tok,'\0',strlen(tok));
    for(int j = i; pkt.data[j] != '\0'; j++){
        tok[k] = pkt.data[j];
        k++;
    }
    u.zipcode = atoi(tok);
    u.connected = 1;
    int id = STORAGE_getID(u);
    if(id < 0){
        //Doesn't exists, create it and return its id
        id = STORAGE_addUser(u);
        u.id = id;
        if(STORAGE_registerUser(u,USR_REG_PATH) < 0){
            char buffer[500];
            sprintf(buffer,"Error registering user \"%s\" with zipcode \"%d\" and assigned id \"%d\".\n",u.login,u.zipcode,id);
            cprint(buffer);
        }
        return id;
    }

    return id;
}
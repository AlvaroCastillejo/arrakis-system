#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/network.h"
//Start the server
int NETWORK_startAtreides(Configuration config){
    if( (listenFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        cprint("Error creant el socket\n");
    }
    bzero(&servidor, sizeof(servidor));
    servidor.sin_port = htons(config.port);
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = inet_addr(config.ip);

    if(bind(listenFD, (struct sockaddr*) &servidor, sizeof(servidor)) < 0){
        cprint("Error fent el bind\n");
        close(listenFD);
        return ERR_BIND;
    }
    if(listen(listenFD, 10) < 0){
        cprint("Error fent el listen\n");
        close(listenFD);
        return ERR_LISTEN;
    } 
    return 1;
}

//Stop the server
void NETWORK_stopAtreides(){
    close(listenFD);
}

//Broadcast for every clinet connected when the server is shuting down
void NETWORK_broadcastLogout(int *connected_fds, int total){
    //int len = (int)(sizeof(connected_fds)/sizeof(int))-2;
    int len = total;
    Packet pkt;
    sprintf(pkt.origin,"ATREIDES");
    sprintf(pkt.data,"disconnect");
    pkt.type = 'Q';
    for(int i = 0; i < len; i++){
        write(connected_fds[i],&pkt,sizeof(Packet));
    }
}

//process teh client message
int NETWORK_checkMessage (Packet pkt, int fd_server, user u) {
    //char buffer[MAX_DATA_SIZE];
    char *dbuffer;
    switch(pkt.type) {
        case 'Q': ;
            //Proceso de desconexion: cambiar posibe flag de la mem principal
            //Mostrar por pantalla que usuario se ha desconectado
            //cprint("SE DESCONECA UN FREMEN\n");
            
            // char tok[MAX_DATA_SIZE];
            char buffer[400];
            int i = 0;
            // for(i = 0; pkt.data[i] != '*'; i++){
            //     tok[i] = pkt.data[i];
            // }
            // i++;

            // char tok_[20];
            // int k = 0;
            // memset(tok_,'\0',strlen(tok_));
            // for(int j = i; pkt.data[j] != '\0'; j++){
            //     tok_[k] = pkt.data[j];
            //     k++;
            // }
            memset(buffer, '\0', strlen(buffer));
            sprintf(buffer,"user \"%s\" with zipcode \"%d\" has disconnected.\n",u.login,u.zipcode);
            cprint(buffer);
            return -1;
            break;
        case 'C': ;
            //Comprobar que la trama sea correcta (siempre estara bien?)
            //Comprobar login
            int newID = LOGIN_loginOK(pkt);
            
            if(newID>=0){
                sprintf(pkt.origin,"ATREIDES");
                pkt.type = 'O';
                dbuffer = UTILS_itoa(newID,buffer,10);
                sprintf(pkt.data,"%s",dbuffer);
                write(fd_server, &pkt, sizeof(Packet));
            } else {
                sprintf(pkt.origin,"ATREIDES");
                pkt.type = 'E';
                sprintf(pkt.data,"ERROR");
                write(fd_server, &pkt, sizeof(Packet));  
            }
            return newID;
            

            /*
            if (se connecta) {
                devolver mensaje de confirmacion
            }else {
                devolver mensaje comunicando error
            }
            */
            break;
        //search for user zipcode
        case 'S': ;
            char name[100];
            i = 0;
            
            for(; pkt.data[i] != '*'; i++){
                name[i] = pkt.data[i];
            }
            i++;
            memset(buffer,'\0', strlen(buffer));
            for(int k = 0; pkt.data[i] != '*'; i++){
                buffer[k] = pkt.data[i];
            }
            i++;
            int id = atoi(buffer);
            memset(buffer,'\0', strlen(buffer));
            for(int k = 0; i < MAX_DATA_SIZE; i++){
                buffer[k] = pkt.data[i];
                k++;
            }
            int zipcodeToSearch = atoi(buffer);
            char aux[500];
            sprintf(aux, "Rebut search %d de %s %d\n",zipcodeToSearch, name, id);
            cprint(aux);
            BidirectionalList list = STORAGE_lookupUsrsIN(zipcodeToSearch);
            cprint("Feta la cerca\n");
            sprintf(aux, "Hi ha %d persones humanes a %d\n", list.size, zipcodeToSearch);
            cprint(aux);
            memset(aux,'\0',strlen(aux));
            int usr_sizes[list.size];
            BIDIRECTIONALLIST_goToHead(&list);
            for(int k = 0; k < list.size; k++){
                char usr_info[MAX_DATA_SIZE*2];
                user r_usr = BIDIRECTIONALLIST_get(&list);
                sprintf(usr_info, "%d %s\n",r_usr.id,r_usr.login);
                cprint(usr_info);
                usr_sizes[k] = strlen(usr_info);
                memset(usr_info,'\0',strlen(usr_info));
                BIDIRECTIONALLIST_next(&list);
            }
            char new_buff[100];
            char* dbuffer = UTILS_itoa(list.size,new_buff,10);
            int total_size_needed = strlen(dbuffer)+1;
            for(int k = 0; k < list.size; k++){
                total_size_needed += usr_sizes[k];
            }
            Packet searchResult;
            sprintf(searchResult.origin, "ATREIDES");
            searchResult.type = 'L';
            sprintf(searchResult.data, "%s",dbuffer);
            searchResult.data[strlen(dbuffer)] = '*';
            //total size needed includes the last *.
            if(total_size_needed < MAX_DATA_SIZE){
                //All users fit
                //Fill the data string
                int k = strlen(dbuffer)+1;
                BIDIRECTIONALLIST_goToHead(&list);
                for(int q = 0; q < list.size; q++){
                    //For each user
                    user ad_usr = BIDIRECTIONALLIST_get(&list);
                    //add name
                    for(int p = 0; p < strlen(ad_usr.login);p++){
                        searchResult.data[k] = ad_usr.login[p];
                        k++;
                    }
                    searchResult.data[k] = '*';
                    k++;
                    //add id
                    char aux_buffer[10];
                    char * aux_dbuffer = UTILS_itoa(ad_usr.id,aux_buffer,10);
                    for(int p = 0; p < strlen(aux_buffer); p++){
                        searchResult.data[k] = aux_buffer[p];
                        k++;
                    }
                    searchResult.data[k] = '*';
                    k++;
                    memset(aux_dbuffer,'\0',strlen(aux_dbuffer));
                    BIDIRECTIONALLIST_next(&list);
                }
                //fill the rest with '-'
                for(; k < MAX_DATA_SIZE; k++){
                    searchResult.data[k] = '\0';
                }
                write(fd_server, &searchResult, 256);
                write(1,searchResult.data, strlen(searchResult.data));
            } else {
                //Not all users fit
                
                int k = strlen(dbuffer)+1;
                BIDIRECTIONALLIST_goToHead(&list);
                //calculate how many users do fit for the first packet.
                int amount = 0;
                for(int total = k; total < MAX_DATA_SIZE;){
                    total += usr_sizes[amount];
                    amount++;
                }
                //substract 1 because it overflooded
                amount--;
                for(int q = 0; q < amount; q++){
                    //For each user
                    user ad_usr = BIDIRECTIONALLIST_get(&list);
                    //add name
                    for(int p = 0; p < strlen(ad_usr.login);p++){
                        searchResult.data[k] = ad_usr.login[p];
                        k++;
                    }
                    searchResult.data[k] = '*';
                    k++;
                    //add id
                    char aux_buffer[10];
                    char * aux_dbuffer = UTILS_itoa(ad_usr.id,aux_buffer,10);
                    for(int p = 0; p < strlen(aux_buffer); p++){
                        searchResult.data[k] = aux_buffer[p];
                        k++;
                    }
                    searchResult.data[k] = '*';
                    k++;
                    memset(aux_dbuffer,'\0',strlen(aux_dbuffer));
                    BIDIRECTIONALLIST_next(&list);
                }
                //fill the rest with '-'
                for(; k < MAX_DATA_SIZE; k++){
                    searchResult.data[k] = '\0';
                }
                write(fd_server, &searchResult, sizeof(Packet));
                write(1,searchResult.data, strlen(searchResult.data));
                //The other packets change the format.
                //Calculate again how many users do fit now.
                    
                ////////BUCLE/////////
                for(;;){
                    k=0;
                    
                    int last_amount = amount;
                    for(int total = 0; total < MAX_DATA_SIZE;){
                        total += usr_sizes[amount];
                        amount++;
                        if(amount == list.size){
                            amount++;
                            break;
                        }
                    }
                    amount--;
                    int users_to_add = amount-last_amount;
                    for(int q = 0; q < users_to_add; q++){
                        //For each user
                        user ad_usr = BIDIRECTIONALLIST_get(&list);
                        //add name
                        for(int p = 0; p < strlen(ad_usr.login);p++){
                            searchResult.data[k] = ad_usr.login[p];
                            k++;
                        }
                        searchResult.data[k] = '*';
                        k++;
                        //add id
                        char aux_buffer[10];
                        char * aux_dbuffer = UTILS_itoa(ad_usr.id,aux_buffer,10);
                        for(int p = 0; p < strlen(aux_buffer); p++){
                            searchResult.data[k] = aux_buffer[p];
                            k++;
                        }
                        searchResult.data[k] = '*';
                        k++;
                        memset(aux_dbuffer,'\0',strlen(aux_dbuffer));
                        BIDIRECTIONALLIST_next(&list);
                    }
                    //fill the rest with '-'
                    for(; k < MAX_DATA_SIZE; k++){
                        searchResult.data[k] = '\0';
                    }
                    write(fd_server, &searchResult, sizeof(Packet));
                    write(1,searchResult.data, strlen(searchResult.data));
                    if(amount == list.size) break;
                }
            }
            cprint("Enviada resposta\n\n");
            memset(pkt.data,'\0',strlen(pkt.data));
            break;        
    }
    //Send received
    if(pkt.type == 'F'){
        char buffer[1000];
        char filename[30];
        char mida[30];
        char md5sum[MAX_DATA_SIZE];
        int filesize = 0;
        memset(filename, '\0', strlen(filename));
        memset(buffer, '\0', strlen(buffer));
        //dbuffer = (char*)malloc(sizeof(char));
        //cprint(pkt.data);
        int i = 0;
        for(; pkt.data[i] != '*'; i++){
            filename[i] = pkt.data[i];
        }
        i++;
        int j = 0;
        for(; pkt.data[i] != '*'; i++){
            mida[j] = pkt.data[i];
            j++;
        }
        filesize = atoi(mida);
        i++;
        j = 0;
        for(; pkt.data[i] != '\0' && i < MAX_DATA_SIZE; i++){
            md5sum[j] = pkt.data[i];
            j++;
        }
        sprintf(buffer, "Rebut send %s de %s %d\n", filename, u.login, u.id);
        //sprintf(buffer, "Rebut send %s de %s %d\n", filename, u.login, u.id);
        cprint(buffer);
        memset(buffer, '\0', strlen(buffer));

        int extensionIncluded = 0;
        char extension[10];
        int ext_ind = 0;
        //get file extension
        for(j = 0; j < strlen(filename) && filename[j] != '\0'; j++){
            if(!extensionIncluded) extensionIncluded = (filename[j] == '.') ? 1 : 0;
            if(extensionIncluded){
                extension[ext_ind] = filename[j];
                ext_ind++;
            }
        }
        sprintf(buffer,"imgs/%d%s", u.id,extension);
        //cprint(buffer);
        //cprint("<-- porro de alvaro\n");
        //Check if file exists and open it with the necessary flags
        int existed = (access(buffer, F_OK) != 0) ? 0 : 1;
        int fileToWrite_fd = -1;
        if(existed){ 
            //cprint("\nEXISTED\n");
            fileToWrite_fd = open(buffer, O_WRONLY | O_TRUNC | O_APPEND, 777);
        } else {
            cprint("\nNo hi ha foto registrada!\n");
            fileToWrite_fd = open(buffer, O_CREAT | O_WRONLY | O_APPEND, 777);
        }
        
        if(fileToWrite_fd < 0){
            sprintf(pkt.origin,"ATREIDES");
            pkt.type = 'R';
            sprintf(pkt.data, "IMAGE OK");
            write(fd_server, &pkt, sizeof(Packet)); 
            return -1;
        }
        
        int bytesCount = 0;
        int ind = 0;
        for (;;) {
            ind ++;
            read(fd_server,&pkt,sizeof(Packet));
            bytesCount += 240;
            if ((filesize - bytesCount) == 0) {
                write(fileToWrite_fd,pkt.data, 240);
                break;
            }
            if ((filesize - bytesCount) < 0) {
                bytesCount = bytesCount - 240;
                write(fileToWrite_fd,pkt.data, (filesize - bytesCount));
                break;
            }
            write(fileToWrite_fd,pkt.data, 240); 
        }
        
        close(fileToWrite_fd);
        char * md5sum_ = UTILS_md5sum(buffer);
        cprint(md5sum_);
        cprint("    <-md5sum Generated\n");
        cprint(md5sum);
        cprint("    <-md5sum from mensaje\n\n");
        if (strcmp(md5sum_, md5sum) != 0) {
            remove(buffer);
            sprintf(pkt.origin,"ATREIDES");
            pkt.type = 'R';
            sprintf(pkt.data, "IMAGE KO");
            cprint("Error al verificar la foto\n");
            write(fd_server, &pkt, sizeof(Packet));
            return -1;
        }
        sprintf(pkt.origin,"ATREIDES");
        pkt.type = 'I';
        sprintf(pkt.data, "IMAGE OK");
        write(fd_server, &pkt, sizeof(Packet));
        free(md5sum_);
        
        return 1;

    }
    //photo sended by client
    if (pkt.type == 'P') {
        char buffer[1000];
        char filename[50];
        struct stat sb;
        Packet packet;
        sprintf(buffer, "Rebut photo %s de %s %d\n", pkt.data, u.login, u.zipcode);
        cprint(buffer);

        sprintf(buffer, "imgs/%s.jpg", pkt.data);
        for (int i = 0; buffer[i] != '\0'; i++) {
            filename[i] = buffer[i];
        }
        
        //Check if file exists and open it with the necessary flags
        int existed = (access(buffer, F_OK) != 0) ? 0 : 1;
        if(!existed){ 
            //send error message 
            sprintf(packet.origin, "ATREIDES");
            packet.type = 'F';
            sprintf(packet.data, "FILE NOT FOUND");
            write(fd_server,&packet,sizeof(Packet));
            cprint("No hi ha foto registrada.\n");
            return -1;
        }   
        
        if (stat(buffer, &sb) == -1) {
            perror("stat");
            return -1;
        }
        
        char * md5sum = UTILS_md5sum(buffer);
        cprint(md5sum);
        cprint("<-md5sum generated for message\n");
        memset(buffer, '\0', strlen(buffer));
        memset(filename, '\0', strlen(filename));
        sprintf(buffer, "%s.jpg", pkt.data);
        char fname[20];
        for (int i = 0; buffer[i] != '\0'; i++) {
            fname[i] = buffer[i];
        } 

        
        //filename[strlen(filename)] = '\0';
        sprintf(packet.origin, "ATREIDES");
        packet.type = 'F';
        sprintf(packet.data, "%s*%lu*%s", fname, sb.st_size, md5sum);

        write(fd_server,&packet,sizeof(Packet));
        memset(buffer, '\0', strlen(buffer));
        sprintf(buffer, "imgs/%s.jpg", pkt.data);
        for (int i = 0; buffer[i] != '\0'; i++) {
            filename[i] = buffer[i];
        }

        int fd_file;
        fd_file = open(filename, O_RDONLY);
        //int size = 1;
        pkt.type = 'D';
        int count = 0;
        int count_ = 0;
        int ind = 0;
        //int bytes_sended = 0;
        //int size = (int) sb.st_size;
        for (;;) {
            ind ++;
            memset(pkt.data,'\0',strlen(pkt.data));
            //sprintf(buffer, "%d - %d = %d\n", (int)sb.st_size, count_, ((int)sb.st_size - count_));
            //cprint(buffer);
            if (((int) sb.st_size - count_) > 240) {
                count = read(fd_file,&pkt.data,240);
                count_ = count_ + count;
                write(fd_server,&pkt,sizeof(Packet)); 
            }else {
                //cprint("preRead\n");
                count = read(fd_file,&pkt.data, (int) sb.st_size - count_);
                write(fd_server,&pkt,sizeof(Packet));
                count_ = count_ + count;
                break;
            }
            //cprint("posttusmuertos\n");
        
        }
        close(fd_file);
        //sprintf(buffer, "\nfull data times: %d\nsize 0 times: %d", count, count_);
        //write(1, buffer, strlen(buffer));

        Packet new_pkt;
        int aux = read(fd_server,&new_pkt,sizeof(Packet));
        
        if(aux < 0){
            cprint("Error: Couldn't receive the send response. Try again...\n");
            return -1;    
        }
        // sprintf(buffer, "%c", pkt.type);
        // cprint(buffer);
        if(new_pkt.type == 'I'){
            //Everything was ok
            cprint("Foto enviada amb exit.\n\n");
            return 1;
        } else {
            sprintf(buffer, "%c chech", pkt.type);
            //cprint(buffer);
            cprint("Error al verificar la foto.\n"); 
            return -1;
        }
    
    }
    return 1;
}
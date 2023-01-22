#include "../include/network.h"

int fd_server;
Command user;
int usr_id = -1;
int connected = 0;

//Connects to socket
int NETWORK_connectServer(Configuration config){
    struct sockaddr_in s_addr;
    int socket_conn = -1;

    socket_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_conn < 0) {
        UTILS_cprint("ERROR durante la creacion del socket.\n");
    } else {
        memset(&s_addr, 0, sizeof(s_addr));
        s_addr.sin_family = AF_INET;
        s_addr.sin_port = htons(config.port);
        s_addr.sin_addr.s_addr = inet_addr(config.ip);

        if (connect(socket_conn, (void *) &s_addr, sizeof(s_addr)) < 0) {
            char buff[128];
            UTILS_cprint("ERROR de connexion con el servidor.\n");
            int bytes = sprintf(buff, "errno says: %s\n", strerror(errno)); // molt útil
            write(1, buff, bytes);
            close(socket_conn);
            socket_conn = -1;
        }
    }
    fd_server = socket_conn;
    return socket_conn;
}

//Establishes connection according to the protocol.
int NETWORK_establishConnection(int fd_server, Command command){
    Packet pkt;
    user = command;
    char* buffer;
    buffer = (char*) malloc(MAX_BUFFER * sizeof(char));
    //Send connection solicitude
    sprintf(pkt.origin,"FREMEN");
    pkt.type = 'C';
    sprintf(pkt.data,"%s*%s",command.argv[1],command.argv[2]);
    if(send(fd_server,&pkt,sizeof(pkt),0) < 0){
        UTILS_cprint("Error: Couldn't send the connection solicitude. Exiting Fremen...\n");
        return -1;
    }
    //Receive connection solicitude response
    if(read(fd_server,&pkt,sizeof(pkt)) < 0){
        UTILS_cprint("Error: Couldn't receive the connection solicitude response. Exiting Fremen...\n");
        return -1;    
    }
    
    if(strcmp(pkt.data,"ERROR") == 0){
        UTILS_cprint("Error: Couldn't login. Try again...\n");
        return 0;
    } else {
        sprintf(buffer,"Benvingut %s. Tens ID %s\nAra estàs connectat a Atreides.\n",command.argv[1],pkt.data);
        UTILS_cprint(buffer);
        free(buffer);
        connected = 1;
        return atoi(pkt.data);
    }
}

void NETWORK_sendSearch(int fd_server, char* zipcode, int usr_id){

    if(connected == 0){
        UTILS_cprint("Error. Not connected to Atreides\n");
        return;
    }

    Packet pkt;
    sprintf(pkt.origin,"FREMEN");
    pkt.type = 'S';
    sprintf(pkt.data,"%s*%d*%s",user.argv[1],usr_id,zipcode);
    int occupied = strlen(pkt.data);
    for(; occupied < MAX_DATA_SIZE; occupied++){
        pkt.data[occupied] = '\0';
    }
    
    if(send(fd_server,&pkt,sizeof(pkt),0) < 0){
        UTILS_cprint("Error: Couldn't send the search solicitude. Try again...\n");
        return;
    }
    write(1,"antes del read\n",15);
    Packet new_packet;
    //signal sigpipe i el write salta si no pot fer-ho
    int retu = read(fd_server,&new_packet,256);
    char buffer[10];
    sprintf(buffer, "%d",retu);
    UTILS_cprint(buffer);
    if(retu < 0){
        UTILS_cprint("Error: Couldn't receive the search solicitude response. Try again...\n");
        return;    
    }
    pkt = new_packet;

    if(pkt.type == 'K'){
        UTILS_cprint("Error: Couldn't search. Try again...\n");
        return;
    } else {
        //write(1,pkt.data, strlen(pkt.data));
        char buffer[500];
        int i = 0;
        int total_size_needed = 0;
        for(int k = 0; pkt.data[k] != '-'; k++){
            total_size_needed++;
        }
        
        for(; pkt.data[i] != '*'; i++){
            buffer[i] = pkt.data[i];
        }
        i++;
        //First * found, skip it. Now buffer has the total users found.
        int total_usrs = atoi(buffer);
        char auxiliar[100];
        sprintf(auxiliar,"Hi ha %d persones humanes a %s\n",total_usrs,zipcode);
        UTILS_cprint(auxiliar);
        memset(auxiliar,'\0',strlen(auxiliar));
        if(total_usrs == 0) return;
        int usrs_read = 0;
        for(; (usrs_read < total_usrs) && (i < MAX_DATA_SIZE); usrs_read++){
            //For each user, read id and login. Current 'i' pointer position at '*'.
            char f_usr_name[MAX_DATA_SIZE];
            memset(f_usr_name,'\0',strlen(f_usr_name));
            for(int k = 0; pkt.data[i] != '*'; i++){
                f_usr_name[k] = pkt.data[i];
                k++;
            }
            i++;
            char sid[MAX_DATA_SIZE];
            for(int k = 0; pkt.data[i] != '*'; i++){
                sid[k] = pkt.data[i];
                k++;
            }
            i++;
            int f_usr_id = atoi(sid);
            sprintf(buffer, "%d %s\n",f_usr_id,f_usr_name);
            UTILS_cprint(buffer);
            memset(buffer,'\0', strlen(buffer));
            usrs_read++;
            if((i > MAX_DATA_SIZE-1) && (usrs_read < total_usrs)){
                //Users fit perfectly in the data field. So when i is incremented it can produce a
                //segmentation fault
                break;
            }
            usrs_read--;
            if(pkt.data[i] == '\0'){
                //Data is longer but there are no more users as they don't fit in this packet.
                //Need to read more packets from the fd_server.
                break;
            }
        }
        usrs_read++;
        int before_usrs_read = usrs_read;
        if(usrs_read < total_usrs){
            //There are more users, need to read from fd_server.
            for(;;){
                i = 0;
                //n more packets to read.
                //printf("users read: %d\n", usrs_read);
                if((usrs_read+before_usrs_read) == total_usrs){
                    return;
                }
                if(read(fd_server,&pkt,sizeof(pkt)) < 0){
                    UTILS_cprint("Error: Couldn't receive the search solicitude response. Try again...\n");
                    return;    
                }
                //write(1,pkt.data, strlen(pkt.data));
                for(; (usrs_read < total_usrs) && (i < MAX_DATA_SIZE); usrs_read++){
                    //For each user, read id and login. Current 'i' pointer position at '*'.
                    char f_usr_name[MAX_DATA_SIZE];
                    memset(f_usr_name,'\0',strlen(f_usr_name));
                    for(int k = 0; pkt.data[i] != '*'; i++){
                        f_usr_name[k] = pkt.data[i];
                        k++;
                    }
                    i++;
                    char sid[MAX_DATA_SIZE];
                    for(int k = 0; pkt.data[i] != '*'; i++){
                        sid[k] = pkt.data[i];
                        k++;
                    }
                    i++;
                    int f_usr_id = atoi(sid);
                    sprintf(buffer, "%d %s\n",f_usr_id,f_usr_name);
                    UTILS_cprint(buffer);
                    memset(buffer,'\0', strlen(buffer));
                    
                    if(pkt.data[i] == '\0'){
                        //Data is longer but there are no more users as they don't fit in this packet.
                        //Need to read more packets from the fd_server.
                        break;
                    }
                }
                if(usrs_read+1 == total_usrs){
                    //finished
                    return;
                }
            }
        }
        return;
    }
}

void NETWORK_disconnect () {
    if(connected == 0){
        return;
    }
    Packet pkt;
    sprintf(pkt.origin,"FREMEN");
    pkt.type = 'Q';
    sprintf(pkt.data,"%s*%s",user.argv[1],user.argv[2]);
    
    write(fd_server, &pkt, sizeof(Packet));
}

int NETWORK_getServerFD(){
    return fd_server;
}

void NETWORK_setDisconnected(){
    connected = 0;
}

int NETWORK_isConnected(){
    return connected;
}

int NETWORK_sendSend(int fd_server, char* filename){
    struct stat sb;
    Packet pkt;
    int fd_file;
    char filename_[30];
    sprintf(filename_, "imgs/%s", filename);

    if(strlen(filename) > 30){
        UTILS_cprint("Error, filename too long!\n");
        return -1;
    }
    
    sprintf(pkt.origin,"FREMEN");
    pkt.type = 'F';
    
    int existed = (access(filename_, F_OK) != 0) ? 0 : 1;
    if(!existed){ 
        //send error message 
        UTILS_cprint("No hi ha foto registrada.\n");
        return -1;
    }   
    
    if (stat(filename_, &sb) == -1) {
        perror("stat");
        return -1;
    }
    
    
    char * md5sum = UTILS_md5sum(filename_);
    sprintf(pkt.data, "%s*%lu*%s",filename,sb.st_size, md5sum);
    write(fd_server,&pkt,sizeof(Packet));
    
    fd_file = open(filename_, O_RDONLY);
    pkt.type = 'D';
    int count = 0;
    int count_ = 0;
    int ind = 0;
    
    for (;;) {
        ind ++;
        memset(pkt.data,'\0',strlen(pkt.data));
        if (((int) sb.st_size - count_) > 240) {
            count = read(fd_file,&pkt.data,240);
            count_ = count_ + count;
            write(fd_server,&pkt,sizeof(Packet)); 
        }else {
            count = read(fd_file,&pkt.data, (int) sb.st_size - count_);
            write(fd_server,&pkt,sizeof(Packet));
            count_ = count_ + count;
            
            break;
        }
       
    } 
    close(fd_file);
    free(md5sum);
    Packet new_pkt;
    int aux = read(fd_server,&new_pkt,sizeof(Packet));
    if(aux < 0){
        UTILS_cprint("Error: Couldn't receive the send response. Try again...\n");
        return -1;    
    }

    if(new_pkt.type == 'I'){
        //Everything was ok
        UTILS_cprint("Foto enviada amb exit a Atreides.\n\n");
        return 1;
    } else {
        UTILS_cprint("No se ha podido mandar la foto.\n\n");
    }
    
    return (new_pkt.type == 'R') ? -1 : 1;
}


int NETWORK_sendPhoto(int fd_server, char * photo_id) {

    Packet pkt;
    char buffer[1000];
    char filename[30];
    char mida[30];
    char md5sum[MAX_DATA_SIZE];
    int filesize;

    sprintf(pkt.origin, "FREMEN");
    pkt.type = 'P';
    sprintf(pkt.data, "%s", photo_id);

    write(fd_server, &pkt, sizeof(Packet));
    memset(filename, '\0', strlen(filename));
    
    read(fd_server, &pkt, sizeof(Packet));
    if (strcmp(pkt.data, "FILE NOT FOUND") == 0) {
        sprintf(buffer, "No hi ha foto registrada.\n");
        UTILS_cprint(buffer);
        return -1;
    }
    //data parse
    int i = 0;
    for(; pkt.data[i] != '*'; i++){
        filename[i] = pkt.data[i];
    }
    char filename_[50];
    sprintf(filename_, "imgs/%s", filename);
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
    md5sum[j] = '\0';
    
    
    sprintf(buffer, "Rebut info de la photo %s de user - Mida: %s - MD5SUM: %s\n", filename, mida, md5sum);
    memset(buffer, '\0', strlen(buffer));
    int existed = (access(filename_, F_OK) != 0) ? 0 : 1;
    int fileToWrite_fd = -1;
    if(existed){ 
        //UTILS_cprint("EXISTED\n");
        fileToWrite_fd = open(filename_, O_WRONLY | O_TRUNC | O_APPEND, 777);
    } else {
        //UTILS_cprint("NO EXISTED\n");
        fileToWrite_fd = open(filename_, O_CREAT | O_WRONLY | O_APPEND, 777);
    }
    
    if(fileToWrite_fd < 0){
        sprintf(pkt.origin,"FREMEN");
        pkt.type = 'R';
        sprintf(pkt.data, "IMAGE OK");
        write(fd_server, &pkt, sizeof(Packet)); 
        return -1;
    }
    //bytes count :v
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
    //Check md5sum
    char * md5sum_ = UTILS_md5sum(filename_);
    UTILS_cprint(md5sum_);
    UTILS_cprint("<-md5sum creado\n");

    UTILS_cprint(md5sum);
    UTILS_cprint("<-md5sum MESSAGE\n");
    if (strcmp(md5sum, md5sum_) != 0) {
        UTILS_cprint("Error al verificar la foto.\n");
        remove(filename_);
        sprintf(pkt.origin,"FREMEN");
        pkt.type = 'R';
        sprintf(pkt.data, "IMAGE KO");
        write(fd_server, &pkt, sizeof(Packet));
        return -1;
    }
    //Send confirm message
    UTILS_cprint("Foto descarregada.\n\n");
    sprintf(pkt.origin,"FREMEN");
    pkt.type = 'I';
    sprintf(pkt.data, "IMAGE OK");
    write(fd_server, &pkt, sizeof(Packet));

    free(md5sum_);
    return 1;

}
#include "../include/storage.h"
#include "../include/utils.h"

struct hashmap *map;
int last_id_assigned = 0;

//loads the databsae (users registered) in the system
int STORAGE_loadDatabase(char* path){
    int fd = open(path, O_RDONLY);
    char buffer[200];
    if(fd < 0){
        cprint("Error al abrir el fichero de usuarios.\n");
        return -1;
    }
    //create the table
    map = HASMAP_createTable(MAX_ZIPCODE);
    int size;
    char c;
    while(1){
        //read first char from each line. Each iteration represents one user.
        size = read(fd, &c, sizeof(char));
        if(size == 0){
            break;
        } else {
            user u;
            for(int i = 0; c != '*';i++){
                buffer[i] = c;
                read(fd, &c, sizeof(char));
            }
            //first * found, skip it. Buffer contains id.
            u.id = atoi(buffer);
            //printf("Added user with id: %d\n", u.id);
            memset(buffer, '\0', strlen(buffer));
            read(fd, &c, sizeof(char));
            for(int i = 0; c != '*';i++){
                buffer[i] = c;
                read(fd, &c, sizeof(char));
            }
            //second * found, skip it. Buffer contains login.
            sprintf(u.login, "%s",buffer);
            memset(buffer, '\0', strlen(buffer));
            read(fd, &c, sizeof(char));
            for(int i = 0; c != '\n';i++){
                buffer[i] = c;
                read(fd, &c, sizeof(char));
                //cprint(buffer);
            }
            u.zipcode = atoi(buffer);
            HASMAP_insert(map, u.zipcode, u);
            last_id_assigned++;
            memset(buffer, '\0', strlen(buffer));
        }
    }
    close(fd);
    return 0;
}
//get an id of a specific user
int STORAGE_getID(user u){
    //search in the list for the user
    BidirectionalList list = HASMAP_lookup(map, u.zipcode);
    BIDIRECTIONALLIST_goToHead(&list);
    while(list.error != 4){
        user aux = BIDIRECTIONALLIST_get(&list);
        if(strcmp(aux.login, u.login) == 0){
            return aux.id;
        }
        BIDIRECTIONALLIST_next(&list);
    }
    return -1;
    //iteramos la lista buscamos el usuario comparando el nombre, si encuentra 
}
//add user in storage
int STORAGE_addUser(user u){
    u.id = ++last_id_assigned;
    HASMAP_insert(map, u.zipcode, u);
    return u.id;
}
//getter for database
hashmap* STORAGE_getStorage(){
    return map;
}
//deletes the database
void STORAGE_deleteDatabase(){
    free(map);
}
//regist new users
int STORAGE_registerUser(user u, char* path){
    int fd = open(path, O_RDWR | O_APPEND);
    char buffer[300];
    if(fd < 0){
        cprint("Error attempting to register user, opening the user register file failed.\n");
        return -1;
    }

    sprintf(buffer,"%d*%s*%d\n",u.id,u.login,u.zipcode);
    write(fd,buffer,strlen(buffer));
    
    close(fd);
    return 1;
}

BidirectionalList STORAGE_lookupUsrsIN(int zipcodeToSearch){
    return HASMAP_lookup(map,zipcodeToSearch);
}
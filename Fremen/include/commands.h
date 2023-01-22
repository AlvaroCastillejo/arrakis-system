#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "../include/utils.h"
#include "../include/network.h"

//Allows the commands that throw errors to be stored and used with "last" command. Set to 0 to disable it.
#define ALLOW_ERR_STORE 1
//Allows the unknown commands to be stored and used with "last" command. Set to 0 to disable it.
#define ALLOW_UNK_STORE 1
//Allows the "showbuffer" comand to be stored and used with "last" command. Set to 0 to disable it.
#define ALLOW_SBC_STORE 1

int COMMANDS_executeCommand(Command command, Configuration config, int usr_id);
char** COMMANDS_getCommandArgv(char buffer[]);
Command COMMANDS_getCommand(char buffer[]);
void COMMANDS_destroyBufferedCommands();
void COMMANDS_normalizeCommand(char buffer[]);

#endif
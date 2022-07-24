#include "server.h"

cmdType analyzeCmd(char *cmd)
{
    if(strcmp(cmd, "exit")==0) return CMD_TYPE_EXIT;
    if(strcmp(cmd, "cd")==0) return CMD_TYPE_CD;
    if(strcmp(cmd, "mv")==0) return CMD_TYPE_MV;
    if(strcmp(cmd, "cp")==0) return CMD_TYPE_CP;
    if(strcmp(cmd, "ls")==0) return CMD_TYPE_LS;
    if(strcmp(cmd, "ll")==0) return CMD_TYPE_LL;
    if(strcmp(cmd, "pwd")==0) return CMD_TYPE_PWD;
    if(strcmp(cmd, "puts")==0) return CMD_TYPE_PUTS;
    if(strcmp(cmd, "gets")==0) return CMD_TYPE_GETS;
    if(strcmp(cmd, "rm")==0) return CMD_TYPE_RM;
    if(strcmp(cmd, "mkdir")==0) return CMD_TYPE_MKDIR;

    return CMD_TYPE_OTHERS;
}


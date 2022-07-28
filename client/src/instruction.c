#include "../include/instruction.h"

//如果返回NULL，则说明出错
cmd_hdl_t *analyzeCmd(char *cmd, size_t cmdSize)
{
    char instruction[10] = {0};     //暂存指令
    cmd_hdl_t *cmdBuff = (cmd_hdl_t *)calloc(1, sizeof(cmd_hdl_t));

    //获取指令
    size_t position = 0;
    for( ; cmd[position] != ' ' && position < cmdSize && cmd[position] != '\n'; ++position){
        instruction[position] = cmd[position];
    }
    if(!strcmp(instruction, "exit")) cmdBuff->cmd = CMD_TYPE_EXIT;
    else if(!strcmp(instruction, "cd")) cmdBuff->cmd =CMD_TYPE_CD;
    else if(!strcmp(instruction, "mv")) cmdBuff->cmd =CMD_TYPE_MV;
    else if(!strcmp(instruction, "cp")) cmdBuff->cmd =CMD_TYPE_CP;
    else if(!strcmp(instruction, "ls")) cmdBuff->cmd =CMD_TYPE_LS;
    else if(!strcmp(instruction, "ll")) cmdBuff->cmd =CMD_TYPE_LL;
    else if(!strcmp(instruction, "pwd")) cmdBuff->cmd =CMD_TYPE_PWD;
    else if(!strcmp(instruction, "puts")) cmdBuff->cmd =CMD_TYPE_PUTS;
    else if(!strcmp(instruction, "gets")) cmdBuff->cmd =CMD_TYPE_GETS;
    else if(!strcmp(instruction, "rm")) cmdBuff->cmd =CMD_TYPE_RM;
    else if(!strcmp(instruction, "mkdir")) cmdBuff->cmd = CMD_TYPE_MKDIR;
    //输入错误，返回空
    else { printf("Error arguments.\n"); free(cmdBuff); return NULL; }
    
    //获取参数
    //忽略空格和结尾
    while(cmd[position] == ' ' && position < cmdSize){
        position++;
    }
    //查看是否有参数
    if(position >= cmdSize){
        cmdBuff->argSize = 0;
        return cmdBuff;
    }else{
        char argv_1[124] = {0};
        char argv_2[124] = {0};
        sscanf(cmd + position, "%s %s", argv_1, argv_2);
        //查看拆分后的数据里面是否有效，统计参数个数
        if(strlen(argv_1) > 0){
            cmdBuff->argSize++;
            cmdBuff->args[0].argLen = strlen(argv_1);
            strcpy(cmdBuff->args[0].arg, argv_1);
        }
        if(strlen(argv_2) > 0){
            cmdBuff->argSize++;
            cmdBuff->args[1].argLen = strlen(argv_2);
            strcpy(cmdBuff->args[1].arg, argv_2);
        }
    }

    return cmdBuff;

}

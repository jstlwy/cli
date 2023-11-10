#include "commands.h"
#include <stdio.h>
#include <string.h>

const char cmdShowCmdsStr[] = "commands";
const char cmdSayHelloStr[] = "hello";
const char cmdExitStr[]     = "exit";

const char* const commands[NUM_CMDS] = {
    [CMD_SHOW_CMDS] = cmdShowCmdsStr,
    [CMD_SAY_HELLO] = cmdSayHelloStr,
    [CMD_EXIT]      = cmdExitStr
};

const int commandLens[NUM_CMDS] = {
    [CMD_SHOW_CMDS] = sizeof(cmdShowCmdsStr)-1,
    [CMD_SAY_HELLO] = sizeof(cmdSayHelloStr)-1,
    [CMD_EXIT]      = sizeof(cmdExitStr)-1
};

int findCommand(const char* const str)
{
    for (int i = 0; i < NUM_CMDS; i++) {
        if (strcmp(str, commands[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void sayHello(void)
{
    printf("Hello, world!\n");
}


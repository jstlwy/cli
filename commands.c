#include "commands.h"
#include <string.h>
#include <stdio.h>

#define STRLEN(str) (sizeof(str) - 1)

static const char cmdHelpStr[]      = "help";
static const char cmdShowCmdsStr[]  = "commands";
static const char cmdSayHelloStr[]  = "hello";
static const char cmdHellaStr[]     = "hella";
static const char cmdPrintTrieStr[] = "trie";
static const char cmdExitStr[]      = "exit";

const char* const commands[NUM_CMDS] = {
    [CMD_HELP]       = cmdHelpStr,
    [CMD_SHOW_CMDS]  = cmdShowCmdsStr,
    [CMD_SAY_HELLO]  = cmdSayHelloStr,
    [CMD_HELLA]      = cmdHellaStr,
    [CMD_PRINT_TRIE] = cmdPrintTrieStr,
    [CMD_EXIT]       = cmdExitStr
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

void printHelp(void)
{
    printf("Type \"%s\" for a list of commands.\n", cmdShowCmdsStr);
    printf("Type \"%s\" to quit.\n", cmdExitStr);
}

void printCommands(void)
{
    puts("Available commands:");
    for (int i = 0; i < NUM_CMDS; i++) {
        puts(commands[i]);
    }
}

void sayHello(void)
{
    puts("Hello, world!");
}

void reactToHella(void)
{
    puts("Are you from California?");
}

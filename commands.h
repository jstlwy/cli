#ifndef COMMANDS_H
#define COMMANDS_H

enum commandType {
    CMD_HELP,
    CMD_SHOW_CMDS,
    CMD_SAY_HELLO,
    CMD_HELLA,
    CMD_PRINT_TRIE,
    CMD_EXIT,
    NUM_CMDS
};

extern const char* const commands[NUM_CMDS];

int findCommand(const char* const str);

void printHelp(void);
void printCommands(void);
void sayHello(void);
void reactToHella(void);

#endif

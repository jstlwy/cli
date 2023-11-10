#ifndef COMMANDS_H
#define COMMANDS_H

enum commandType {
    CMD_SHOW_CMDS,
    CMD_SAY_HELLO,
    CMD_EXIT,
    NUM_CMDS
};

extern const char cmdShowCmdsStr[];
extern const char cmdSayHelloStr[];
extern const char cmdExitStr[];

extern const char* const commands[NUM_CMDS];
extern const int commandLens[NUM_CMDS];

int findCommand(const char* const str);
void sayHello(void);

#endif


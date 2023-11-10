#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "commands.h"
#include "keycodes.h"

#define INPUT_BUFFER_LEN 64

int autocomplete(char usrInBuf[const static INPUT_BUFFER_LEN], const int inLen);

int main(void)
{
    // Save current terminal attributes so that they can be restored later
    struct termios old_attr;
    tcgetattr(STDIN_FILENO, &old_attr);
    struct termios new_attr = old_attr;
    // Disable canonical mode and echoing to terminal
    new_attr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);
    // Disable stdout buffering so that each print statement displays immediately
    setvbuf(stdout, NULL, _IONBF, 0);

    char usrInBuf[INPUT_BUFFER_LEN];
    memset(usrInBuf, 0, INPUT_BUFFER_LEN);
    char history[INPUT_BUFFER_LEN];
    memset(history, 0, INPUT_BUFFER_LEN);
    int inLen = 0;
    int histLen = 0;

    printf("Type \"%s\" for a list of commands.\n", cmdShowCmdsStr);
    printf("Type \"%s\" to quit.\n", cmdExitStr);
    bool shouldContinue = true;
    while (shouldContinue) {
        printf("%s--> ", ansiEraseLineSeq);
        if (inLen > 0) {
            printf("%s", usrInBuf);
        }

        // Track whether to attempt to call a function
        // and which function to call
        bool shouldExecute = false;
        int selection = -1;

        // First, handle user input
        char readBuf[4];
        const ssize_t numRead = read(STDIN_FILENO, readBuf, sizeof(readBuf));
        switch (readBuf[0]) {
        case KEY_BACKSPACE:
        case KEY_DELETE:
            if (inLen > 0) {
                inLen--;
                usrInBuf[inLen] = 0;
            }
            break;
        case KEY_TAB:
            inLen = autocomplete(usrInBuf, inLen);
            break;
        case KEY_ENTER:
            shouldExecute = true;
            break;
        case KEY_ESCAPE:
            if (numRead == 1) {
                memset(usrInBuf, 0, INPUT_BUFFER_LEN);
                inLen = 0;
            } else if ((numRead == 3) && (memcmp(readBuf, ansiUpSeq, numRead) == 0)) {
                memcpy(usrInBuf, history, INPUT_BUFFER_LEN);
                inLen = histLen > 0 ? histLen - 1 : 0;
            }
            break;
        case KEY_SPACE:
            // Prevent the user from entering wasteful leading whitespace
            if ((inLen > 0) && (inLen < INPUT_BUFFER_LEN)) {
                usrInBuf[inLen++] = readBuf[0];
            }
            break;
        default:
            if ((isVisibleAsciiChar(readBuf[0])) && (inLen < INPUT_BUFFER_LEN)) {
                usrInBuf[inLen++] = readBuf[0];
            }
            break;
        }

        if (!shouldExecute) {
            continue;
        }

        printf("\n");
        if (inLen < INPUT_BUFFER_LEN) {
            usrInBuf[inLen++] = '\0';
        } else {
            usrInBuf[INPUT_BUFFER_LEN - 1] = '\0';
        }
        selection = findCommand(usrInBuf);

        // If the user entered a valid command, call the appropriate function
        switch (selection) {
        case CMD_SHOW_CMDS:
            printf("Available commands:\n");
            for (int i = 0; i < NUM_CMDS; i++) {
                printf("%s\n", commands[i]);
            }
            break;
        case CMD_SAY_HELLO:
            sayHello();
            break;
        case CMD_EXIT:
            shouldContinue = false;
            break;
        default:
            printf("Invalid command.\n");
            break;
        }

        memcpy(history, usrInBuf, INPUT_BUFFER_LEN);
        histLen = inLen;
        memset(usrInBuf, 0, INPUT_BUFFER_LEN);
        inLen = 0;
    }

    printf("%s", ansiEraseLineSeq);
    // Restore the old terminal attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
    return 0;
}

int autocomplete(char usrInBuf[const static INPUT_BUFFER_LEN], const int inLen)
{
    const size_t cmpSize = (size_t)inLen;
    for (int i = 0; i < NUM_CMDS; i++) {
        if (strncmp(usrInBuf, commands[i], cmpSize) == 0) {
            memcpy(usrInBuf, commands[i], (size_t)commandLens[i]);
            return commandLens[i];
        }
    }
    return inLen;
}


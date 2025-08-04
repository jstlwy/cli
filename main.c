#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include "commands.h"
#include "keycodes.h"
#include "trie.h"

#define INPUT_BUFFER_LEN 64

int main(void)
{
    TRIE trie = trieGetNew();
    for (int i = 0; i < NUM_CMDS; i++) {
        trieAddString(&trie, i, commands[i]);
    }

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
    memset(usrInBuf, '\0', INPUT_BUFFER_LEN);
    size_t inLen = 0;

    char history[INPUT_BUFFER_LEN];
    memset(history, '\0', INPUT_BUFFER_LEN);
    size_t histLen = 0;

    printHelp();

    bool shouldContinue = true;
    while (shouldContinue) {
        printf("%s--> %s", ansiEraseLineSeq, usrInBuf);

        // Track whether to attempt to call a function
        // and which function to call
        bool shouldExecute = false;
        size_t newSize = 0;

        // First, handle user input
        char readBuf[4];
        const ssize_t numRead = read(STDIN_FILENO, readBuf, sizeof(readBuf));

        switch (readBuf[0]) {
        case KEY_BACKSPACE:
        case KEY_DELETE:
            if (inLen > 0) {
                inLen--;
                usrInBuf[inLen] = '\0';
            }
            break;
        case KEY_TAB:
            newSize = trieAutocomplete(&trie, INPUT_BUFFER_LEN, usrInBuf);
            if (newSize > 0) {
                inLen = newSize;
            }
            break;
        case KEY_ENTER:
            shouldExecute = true;
            break;
        case KEY_ESCAPE:
            if (numRead == 1) {
                memset(usrInBuf, '\0', INPUT_BUFFER_LEN);
                inLen = 0;
            } else if ((numRead == 3) && (memcmp(readBuf, ansiUpSeq, numRead) == 0)) {
                memcpy(usrInBuf, history, INPUT_BUFFER_LEN);
                inLen = histLen;
            }
            break;
        case KEY_SPACE:
            // Prevent the user from entering wasteful leading whitespace
            if ((inLen > 0) && (inLen < INPUT_BUFFER_LEN)) {
                usrInBuf[inLen++] = readBuf[0];
            }
            break;
        default:
            if ((isprint(readBuf[0])) && (inLen < (INPUT_BUFFER_LEN - 1))) {
                usrInBuf[inLen++] = readBuf[0];
                usrInBuf[inLen] = '\0';
            }
        }

        if (!shouldExecute) {
            continue;
        }

        putchar('\n');
        const ssize_t selection = trieGetCmdIndex(&trie, INPUT_BUFFER_LEN, usrInBuf);

        // If the user entered a valid command, call the appropriate function
        switch (selection) {
        case CMD_HELP:
            printHelp();
            break;
        case CMD_SHOW_CMDS:
            printCommands();
            break;
        case CMD_SAY_HELLO:
            sayHello();
            break;
        case CMD_HELLA:
            reactToHella();
            break;
        case CMD_PRINT_TRIE:
            triePrint(&trie);
            break;
        case CMD_EXIT:
            shouldContinue = false;
            break;
        default:
            puts("Command not found.");
            break;
        }

        memcpy(history, usrInBuf, INPUT_BUFFER_LEN);
        histLen = inLen;
        memset(usrInBuf, 0, INPUT_BUFFER_LEN);
        inLen = 0;
    }

    puts(ansiEraseLineSeq);

    // Restore the old terminal attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);

    trieDelete(&trie);

    return EXIT_SUCCESS;
}

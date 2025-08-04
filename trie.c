#include "trie.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // malloc(), free(), exit(), EXIT_FAILURE
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <assert.h>

#define ARENA_INIT_CAPACITY    16
#define MAX_WORD_LEN           256
#define MAX_WORD_LEN_SANS_NULL (MAX_WORD_LEN - 1)

TRIE trieGetNew(void)
{
    TRIE trie;
    trie.capacity = ARENA_INIT_CAPACITY;
    trie.nodeArena = malloc(trie.capacity * sizeof(TRIE_NODE));
    if (trie.nodeArena == NULL) {
        fprintf(stderr, "%s: ERROR: malloc failed.\n", __func__);
        exit(EXIT_FAILURE);
    }
    trie.size = 0;
    return trie;
}

#if 0
static void trieNodePrint(const TRIE_NODE* const tNode)
{
    printf("Address    : %p\n", (void*)tNode);
    printf("Char       : %c\n", tNode->ch);
    printf("iCmd       : %d\n", tNode->iCmd);
    printf("nextSibling: %d\n", tNode->nextSibling);
    printf("firstChild : %d\n", tNode->firstChild);
}
#endif

void triePrint(const TRIE* const trie)
{
    if (trie == NULL) {
        fprintf(stderr, "%s: ERROR: Null trie.\n", __func__);
        return;
    }

    const size_t size = trie->size;
    if (size == 0) {
        printf("%s: The trie is empty.\n", __func__);
        return;
    }

    printf("%s: Capacity: %zu\n", __func__, trie->capacity);
    printf("%s: Size    : %zu\n", __func__, size);
    printf("%s: Arena   : %p\n", __func__, (void*)trie->nodeArena);

    puts("idx        | ch | iCmd       | nextSibling | firstChild");
    for (size_t i = 0; i < size; i++) {
        const TRIE_NODE* const tNode = trie->nodeArena + i;
        printf("%10zu | %2c | %10d | %11d | %10d\n", i, tNode->ch, tNode->iCmd, tNode->nextSibling, tNode->firstChild);
    }
}

static int trieAddNode(TRIE* const trie, const char ch)
{
    assert(trie != NULL);
    assert(isprint(ch));

    const size_t current_size = trie->size;
    const size_t current_capacity = trie->capacity;

    if (current_size >= current_capacity) {
        const size_t new_capacity = 2 * current_capacity;
        TRIE_NODE* const newArena = realloc(trie->nodeArena, new_capacity * sizeof(TRIE_NODE));
        if (newArena == NULL) {
            fprintf(stderr, "%s: ERROR: realloc failed.\n", __func__);
            free(trie->nodeArena);
            exit(EXIT_FAILURE);
        }
        trie->nodeArena = newArena;
        trie->capacity = new_capacity;
    }

    trie->nodeArena[current_size] = (TRIE_NODE){
        .ch = ch,
        .iCmd = -1,
        .nextSibling = -1,
        .firstChild = -1
    };

    trie->size++;
    assert(current_size <= INT_MAX);
    return (int)current_size;
}

static void trieNodeSetCommand(TRIE* const trie, const int iNode, const int iCmd)
{
    assert((iNode >= 0) && ((size_t)iNode < trie->capacity));
    TRIE_NODE* const tNode = trie->nodeArena + iNode;
    tNode->iCmd = iCmd;
}

static void trieNodeSetSibling(TRIE* const trie, const int iNode, const int iSibling)
{
    assert((iNode >= 0) && ((size_t)iNode < trie->capacity));
    TRIE_NODE* const tNode = trie->nodeArena + iNode;
    tNode->nextSibling = iSibling;
}

static void trieNodeSetChild(TRIE* const trie, const int iNode, const int iChild)
{
    assert((iNode >= 0) && ((size_t)iNode < trie->capacity));
    TRIE_NODE* const tNode = trie->nodeArena + iNode;
    tNode->firstChild = iChild;
}

void trieAddString(TRIE* const trie, const int iCmd, const char str[const])
{
    if (trie == NULL) {
        fprintf(stderr, "%s: ERROR: Null trie.\n", __func__);
        return;
    }

    if (iCmd < 0) {
        fprintf(stderr, "%s: ERROR: Invalid command index.\n", __func__);
        return;
    }

    if (str == NULL) {
        fprintf(stderr, "%s: ERROR: Null string.\n", __func__);
        return;
    }

    // The string must be of at least size 1 and not contain unprintable characters
    if (!isprint(str[0])) {
        fprintf(stderr, "%s: ERROR: Empty or invalid string.\n", __func__);
    }
    size_t strLen = 1;
    while (true) {
        const char currentChar = str[strLen];
        if (currentChar == '\0') {
            break;
        }
        if (!isprint(currentChar)) {
            fprintf(stderr, "%s: ERROR: Invalid string.\n", __func__);
            return;
        }
        strLen++;
    }

    int iNode = 0;
    size_t iStr = 0;

    // Operations are simpler for the very first string
    if (trie->size == 0) {
        // First character
        trieAddNode(trie, str[0]);
        iStr++;

        // Set each subsequent character as the child of the character before it
        while (iStr < strLen) {
            const int iNodeNext = trieAddNode(trie, str[iStr]);
            trieNodeSetChild(trie, iNode, iNodeNext);
            iNode = iNodeNext;
            iStr++;
        }

        trieNodeSetCommand(trie, iNode, iCmd);
        return;
    }

    TRIE_NODE* tNode = trie->nodeArena + iNode;

    // See how much of the string is already in the trie
    while (iNode >= 0) {
        const char currentChar = str[iStr];

        // See if the current level contains the current character
        bool didFindCharInCurrentLevel = false;
        while (true) {
            if (tNode->ch == currentChar) {
                didFindCharInCurrentLevel = true;
                break;
            }

            const int nextSibling = tNode->nextSibling;
            if (nextSibling < 0) {
                // Nothing else in the current level
                break;
            }

            // Try the next node in the current level
            iNode = nextSibling;
            tNode = trie->nodeArena + iNode;
        }

        if (!didFindCharInCurrentLevel) {
            break;
        }

        // The current character has been found in this node (and level),
        // so move on to the next character
        iStr++;

        if (str[iStr] == '\0') {
            const int iCmdOld = tNode->iCmd;
            if (iCmdOld >= 0) {
                printf("%s: WARNING: Changing node %d command index from %d to %d.\n", __func__, iNode, iCmdOld, iCmd);
            }
            tNode->iCmd = iCmd;
            return;
        }

        // Move on to the next level
        iNode = tNode->firstChild;
        tNode = trie->nodeArena + iNode;
    }

    if (iNode >= 0) {
        // The character wasn't found in the current level, so add a sibling
        assert(tNode->nextSibling < 0);
        assert(str[iStr] != '\0');
        const int nextSibling = trieAddNode(trie, str[iStr++]);
        trieNodeSetSibling(trie, iNode, nextSibling);
        iNode = nextSibling;
        tNode = trie->nodeArena + iNode;
    }
    
    // Keep adding the rest of the characters
    while (true) {
        const char currentChar = str[iStr];
        if (currentChar == '\0') {
            tNode->iCmd = iCmd;
            return;
        }

        const int firstChild = trieAddNode(trie, currentChar);
        trieNodeSetChild(trie, iNode, firstChild);

        iStr++;
        iNode = firstChild;
        tNode = trie->nodeArena + iNode;
    }
}

static void printAllCommands(
    const TRIE* const trie,
    const int iNode,
    const size_t cmdLen,
    char cmdBuf[const static MAX_WORD_LEN]
) {
    assert(trie != NULL);
    assert(cmdBuf != NULL);

    if ((iNode < 0) || ((size_t)iNode >= trie->size)) {
        return;
    }
    if (cmdLen >= MAX_WORD_LEN_SANS_NULL) {
        return;
    }

    const TRIE_NODE* const tNode = trie->nodeArena + iNode;
    cmdBuf[cmdLen] = tNode->ch;

    if (tNode->iCmd >= 0) {
        cmdBuf[cmdLen + 1] = '\0';
        printf("\n%s", cmdBuf);
    }

    // Try the children
    printAllCommands(trie, tNode->firstChild, cmdLen + 1, cmdBuf);

    // Try the siblings
    printAllCommands(trie, tNode->nextSibling, cmdLen, cmdBuf);
}

size_t trieAutocomplete(const TRIE* const trie, const size_t buflen, char usrBuf[const buflen])
{
    if (trie == NULL) {
        fprintf(stderr, "%s: ERROR: Null trie.\n", __func__);
        return 0;
    }
    if (trie->size == 0) {
        fprintf(stderr, "%s: ERROR: Empty trie.\n", __func__);
        return 0;
    }

    if ((buflen == 0) || (usrBuf == NULL)) {
        fprintf(stderr, "%s: ERROR: Invalid user buffer.\n", __func__);
        return 0;
    }
    if (memchr(usrBuf, '\0', buflen) == NULL) {
        fprintf(stderr, "%s: ERROR: The user buffer is not null-terminated.\n", __func__);
        return 0;
    }

    if (usrBuf[0] == '\0') {
        return 0;
    }

    // The string to find must start with a printable character
    if (!isprint(usrBuf[0])) {
        fprintf(stderr, "%s: ERROR: Invalid string.\n", __func__);
        return 0;
    }

    size_t iBuf = 0;
    int iNode = 0;

    // See if the current string is in the trie
    while ((iNode >= 0) && (iBuf < buflen)) {
        const TRIE_NODE* tNode = trie->nodeArena + iNode;
        const char currentChar = usrBuf[iBuf];

        // The end of the user's input has been reached
        if (currentChar == '\0') {
            break;
        }

        // See if the current level contains the current character
        while (true) {
            if (tNode->ch == currentChar) {
                // Found the current character, so move on
                break;
            }

            const int nextSibling = tNode->nextSibling;
            if (nextSibling < 0) {
                // No more nodes in the current level
                return 0;
            }

            // Try the next node in the current level
            tNode = trie->nodeArena + nextSibling;
        }

        // The current character has been found,
        // so move on to the next character and level
        iBuf++;
        iNode = tNode->firstChild;
    }

    const size_t size_sans_null = buflen - 1;

    // No more nodes in the tree or no more room in the buffer
    if ((iNode < 0) || (iBuf >= size_sans_null)) {
        // Nothing was added to the user's input
        return 0;
    }

    // Add as many characters with no siblings as possible
    // while leaving room for a null terminator
    while ((iNode >= 0) && (iBuf < size_sans_null)) {
        const TRIE_NODE* tNode = trie->nodeArena + iNode;
        if (tNode->nextSibling >= 0) {
            // This level contains more than 1 possible character
            break;
        }
        usrBuf[iBuf] = tNode->ch;
        iBuf++;
        iNode = tNode->firstChild;
    }

    usrBuf[iBuf] = '\0';

    // If there are still nodes in the trie, print all other possibilities
    if (iNode >= 0) {
        char cmdBuf[MAX_WORD_LEN];
        strncpy(cmdBuf, usrBuf, MAX_WORD_LEN);
        printAllCommands(trie, iNode, iBuf, cmdBuf);
        putchar('\n');
    }

    return iBuf;
}

int trieGetCmdIndex(const TRIE* const trie, const size_t buflen, char usrBuf[const buflen])
{
    if (trie == NULL) {
        fprintf(stderr, "%s: ERROR: Null trie.\n", __func__);
        return -1;
    }
    if ((trie->size == 0) || (trie->capacity == 0)) {
        fprintf(stderr, "%s: ERROR: Empty trie.\n", __func__);
        return -1;
    }

    if ((buflen == 0) || (usrBuf == NULL)) {
        fprintf (stderr, "%s: ERROR: Invalid user buffer.\n", __func__);
        return -1;
    }
    if (memchr(usrBuf, '\0', buflen) == NULL) {
        fprintf(stderr, "%s: ERROR: The user buffer is not null-terminated.\n", __func__);
        return -1;
    }

    if (usrBuf[0] == '\0') {
        return -1;
    }

    // Commands should contain at least one printable character
    if (!isprint(usrBuf[0])) {
        fprintf(stderr, "%s: ERROR: Invalid command.\n", __func__);
        return -1;
    }

    size_t iBuf = 0;
    int iNode = 0;

    while ((iNode >= 0) && (iBuf < buflen)) {
        const TRIE_NODE* tNode = trie->nodeArena + iNode;
        const char currentChar = usrBuf[iBuf];
        assert(currentChar != '\0');

        // See if the current level contains the current character
        while (true) {
            if (tNode->ch == currentChar) {
                break;
            }

            const int nextSibling = tNode->nextSibling;
            if (nextSibling < 0) {
                // No matching characters found in this level
                return -1;
            }

            tNode = trie->nodeArena + nextSibling;
        }

        // The current character has been found,
        // so move on to the next character
        iBuf++;

        // Peek ahead to the next char.
        // If it's the null terminator,
        // return the current node's command index.
        if (usrBuf[iBuf] == '\0') {
            return tNode->iCmd;
        }

        // This isn't the end of the string,
        // so move on to the next level
        iNode = tNode->firstChild;
    }

    return -1;
}

int trieDelete(TRIE* const trie)
{
    if (trie == NULL) {
        fprintf(stderr, "%s: ERROR: Null trie.\n", __func__);
        return -1;
    }
    if (trie->nodeArena == NULL) {
        fprintf(stderr, "%s: ERROR: This trie has already been deleted.\n", __func__);
        return -1;
    }
    free(trie->nodeArena);
    trie->nodeArena = NULL;
    trie->capacity = 0;
    trie->size = 0;
    return 0;
}

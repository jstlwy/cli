#include "trie.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // malloc(), free(, exit(), EXIT_FAILURE
#include <string.h>
#include <errno.h>

#define MAX_WORD_LEN 256

struct TrieNode {
    char c;
    int iCmd;

    // For characters branching off the same node
    struct TrieNode* next;

    // For characters branching off of this one
    size_t numChildren;
    struct TrieNode* children;
};

struct Trie trieInit(void)
{
    struct Trie trie = {
        .firstCharList = NULL
    };
    return trie;
}

void triePrintNodes(const struct TrieNode* const tNode, const size_t iCurrent)
{
    if (tNode == NULL) {
        fprintf (stderr, "%s: Error: Invalid trie node.\n", __func__);
        return;
    }

    printf("[%zu] = %c; numChildren = %zu\n", iCurrent, tNode->c, tNode->numChildren);
    for (struct TrieNode* child = tNode->children; child != NULL; child = child->next) {
        triePrintNodes(child, iCurrent+1);
    }
}

void triePrint(const struct Trie* const trie)
{
    if (trie == NULL) {
        fprintf(stderr, "%s: Error: Null trie.\n", __func__);
        return;
    }

    for (struct TrieNode* tNode = trie->firstCharList; tNode != NULL; tNode = tNode->next) {
        triePrintNodes(tNode, 0);
    }
}

void addStringHelper(
    struct TrieNode* const tNode,
    const char str[const],
    const int iCmd,
    const int iCurrent)
{
    const char currentChar = str[iCurrent];
    
    // If at the end of the input string, the current trie node
    // represents the last character in a legitimate word/string
    if (currentChar == '\0') {
        tNode->iCmd = iCmd;
        return;
    }

    // Check whether the current character is already in the list
    struct TrieNode* tail = NULL;
    for (struct TrieNode* child = tNode->children; child != NULL; child = child->next) {
        if (child->c == currentChar) {
            addStringHelper(child, str, iCmd, iCurrent+1);
            return;
        }
        tail = child;
    }

    // The char wasn't found, so create a new node for it
    // and append it to the current TrieNode's list of children
    struct TrieNode* const newChild = malloc(sizeof(struct TrieNode));
    if (newChild == NULL) {
        fprintf(stderr, "%s: Malloc failed.\n", __func__);
        exit(EXIT_FAILURE);
    }
    newChild->c = currentChar;
    newChild->iCmd = -1;
    newChild->next = NULL;
    newChild->numChildren = 0;
    newChild->children = NULL;

    // Check whether this is the current TrieNode's first child
    if (tail == NULL) {
        tNode->children = newChild;
    } else {
        tail->next = newChild;
    }
    tNode->numChildren++;

    addStringHelper(newChild, str, iCmd, iCurrent+1);
}

void trieAddString(struct Trie* const trie, const char str[const], const int iCmd)
{
    if (trie == NULL) {
        fprintf(stderr, "%s: Error: Null trie.\n", __func__);
        return;
    }
    if (str == NULL) {
        fprintf(stderr, "%s: Error: Null string.\n", __func__);
        return;
    }
    if (iCmd < 0) {
        fprintf(stderr, "%s: Error: Invalid command index.\n", __func__);
        return;
    }

    const char firstChar = str[0];
    if (firstChar == '\0') {
        return;
    }
    
    // Check whether the first letter of this string is already in the list
    struct TrieNode* tail = NULL;
    for (struct TrieNode* tNode = trie->firstCharList; tNode != NULL; tNode = tNode->next) {
        if (tNode->c == firstChar) {
            addStringHelper(tNode, str, iCmd, 1);
            return;
        }
        tail = tNode;
    }

    // The char wasn't found, so create new nodes for it
    // and append the new TrieNode to the firstCharlist
    struct TrieNode* const tNode = malloc(sizeof(struct TrieNode));
    if (tNode == NULL) {
        fprintf(stderr, "%s: Malloc failed.\n", __func__);
        exit(EXIT_FAILURE);
    }
    tNode->c = firstChar;
    tNode->iCmd = -1;
    tNode->next = NULL;
    tNode->numChildren = 0;
    tNode->children = NULL;

    // Check for the special case where the trie does not contain any chars yet
    if (tail == NULL) {
        trie->firstCharList = tNode;
    } else {
        tail->next = tNode;
    }

    addStringHelper(tNode, str, iCmd, 1);
}

void printStrings(
    const struct TrieNode* const tNode,
    const size_t iCurrent,
    char wordBuffer[const static MAX_WORD_LEN]
)
{
    if (iCurrent >= MAX_WORD_LEN) {
        fprintf(stderr, "%s: Error: Exceeded buffer length.\n", __func__);
        return;
    }

    if (tNode->iCmd >= 0) {
        wordBuffer[iCurrent] = '\0';
        puts(wordBuffer);
    }

    for (struct TrieNode* child = tNode->children; child != NULL; child = child->next) {
        wordBuffer[iCurrent] = child->c;
        printStrings(child, iCurrent+1, wordBuffer);
    }
}

size_t copyWhileUnique(
    const struct TrieNode* const tNode,
    const size_t buflen,
    char usrBuf[const buflen],
    const size_t iCurrent,
    char wordBuffer[const static MAX_WORD_LEN])
{
    if ((iCurrent >= buflen) || (iCurrent >= MAX_WORD_LEN)) {
        fprintf(stderr, "%s: Error: Exceeded buffer length.\n", __func__);
        return iCurrent-1;
    }

    const size_t numNext = tNode->numChildren;
    if (numNext == 0) {
        return iCurrent;
    }

    // If no longer unique, or a valid word was reached,
    // stop copying to usBuf here and print all further options
    if ((numNext > 1) || (tNode->iCmd >= 0)) {
        strncpy(wordBuffer, usrBuf, iCurrent);
        puts("");
        printStrings(tNode, iCurrent, wordBuffer);
        return iCurrent;
    }

    // Still on a unique path
    struct TrieNode* const child = tNode->children;
    if (child == NULL) {
        fprintf(stderr, "%s: Error: Unexpectedly encountered a null trie node.\n", __func__);
    }
    usrBuf[iCurrent] = child->c;
    return copyWhileUnique(child, buflen, usrBuf, iCurrent+1, wordBuffer);
}

size_t consumePrefix(
    const struct TrieNode* const tNode,
    const size_t buflen,
    char usrBuf[const buflen],
    const size_t iCurrent,
    char wordBuffer[const MAX_WORD_LEN])
{
    if (iCurrent >= buflen) {
        fprintf(stderr, "%s: Error: Exceeded user buffer length.\n", __func__);
        return buflen;
    }

    const char currentChar = usrBuf[iCurrent];
    if (currentChar == '\0') {
        return copyWhileUnique(tNode, buflen, usrBuf, iCurrent, wordBuffer);
    }

    for (struct TrieNode* child = tNode->children; child != NULL; child = child->next) {
        if (child->c == currentChar) {
            return consumePrefix(child, buflen, usrBuf, iCurrent+1, wordBuffer);
        }
    }

    // The current string is not in the trie.
    return 0;
}

size_t trieAutocomplete(const struct Trie* const trie, const size_t buflen, char usrBuf[const buflen])
{
    if (trie == NULL) {
        fprintf(stderr, "%s: Error: Null trie.\n", __func__);
        return 0;
    }
    if ((buflen == 0) || (usrBuf == NULL)) {
        fprintf(stderr, "%s: Error: Invalid user buffer.\n", __func__);
        return 0;
    }
    if (memchr(usrBuf, '\0', buflen) == NULL) {
        fprintf(stderr, "%s: Error: The user buffer is not null-terminated.\n", __func__);
        return 0;
    }

    const char firstChar = usrBuf[0];
    if (firstChar == '\0') {
        return 0;
    }

    char wordBuffer[MAX_WORD_LEN];
    for (struct TrieNode* tNode = trie->firstCharList; tNode != NULL; tNode = tNode->next) {
        if (tNode->c == firstChar) {
            return consumePrefix(tNode, buflen, usrBuf, 1, wordBuffer);
        }
    }

    return 0;
}

int trieGetCmdIndexHelper(
    const struct TrieNode* const tNode,
    const size_t buflen,
    char usrBuf[const buflen],
    const size_t iCurrent)
{
    if (iCurrent >= buflen) {
        fprintf(stderr, "%s: Error: Exceeded buffer length.\n", __func__);
        return -1;
    }

    const char currentChar = usrBuf[iCurrent];

    if (currentChar == '\0') {
        const int iCmd = tNode->iCmd;
        if (iCmd >= 0) {
            return iCmd;
        } else {
            return -1;
        }
    }

    for (struct TrieNode* child = tNode->children; child != NULL; child = child->next) {
        if (child->c == currentChar) {
            return trieGetCmdIndexHelper(child, buflen, usrBuf, iCurrent+1);
        }
    }

    return -1;
}

int trieGetCmdIndex(const struct Trie* const trie, const size_t buflen, char usrBuf[const buflen])
{
    if (trie == NULL) {
        fprintf(stderr, "%s: Error: Null trie.\n", __func__);
        return -1;
    }
    if ((buflen == 0) || (usrBuf == NULL)) {
        fprintf (stderr, "%s: Error: Invalid user buffer.\n", __func__);
        return -1;
    }
    if (memchr(usrBuf, '\0', buflen) == NULL) {
        fprintf(stderr, "%s: Error: The user buffer is not null-terminated.\n", __func__);
        return -1;
    }
    
    const char firstChar = usrBuf[0];
    if (firstChar == '\0') {
        return -1;
    }

    for (struct TrieNode* tNode = trie->firstCharList; tNode != NULL; tNode = tNode->next) {
        if (tNode->c == firstChar) {
            return trieGetCmdIndexHelper(tNode, buflen, usrBuf, 1);
        }
    }

    return -1;
}

void trieDeleteHelper(struct TrieNode* const tNode)
{
    // First, free all of this node's children
    struct TrieNode* child = tNode->children;
    while (child != NULL) {
        struct TrieNode* const next = child->next;
        trieDeleteHelper(child);
        child = next;
    }
    
    // Finally, free this node
    free(tNode);
}

void trieDelete(struct Trie* const trie)
{
    if (trie == NULL) {
        fprintf(stderr, "%s: Error: Null trie.\n", __func__);
        return;
    }

    struct TrieNode* tNode = trie->firstCharList;
    while (tNode != NULL) {
        struct TrieNode* const next = tNode->next;
        trieDeleteHelper(tNode);
        tNode = next;
    }
}


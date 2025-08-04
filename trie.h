#ifndef TRIE_H
#define TRIE_H

#include <sys/types.h>

typedef struct TrieNode {
    char ch;
    ssize_t iCmd;

    // For characters branching off the same node
    ssize_t nextSibling;

    // For characters branching off of this one
    ssize_t firstChild;
} TRIE_NODE;

typedef struct Trie {
    size_t capacity;
    size_t size;
    TRIE_NODE* nodeArena;
} TRIE;

TRIE trieGetNew(void);
void triePrint(const TRIE* const trie);
void trieAddString(TRIE* const trie, const ssize_t iCmd, const char str[const]);
size_t trieAutocomplete(const TRIE* const trie, const size_t buflen, char usrBuf[const buflen]);
ssize_t trieGetCmdIndex(const TRIE* const trie, const size_t buflen, char usrBuf[const buflen]);
int trieDelete(TRIE* const trie);

#endif

#ifndef TRIE_H
#define TRIE_H

#include <stddef.h>

typedef struct TrieNode {
    char ch;
    int iCmd;

    // For characters branching off the same node
    int nextSibling;

    // For characters branching off of this one
    int firstChild;
} TRIE_NODE;

typedef struct Trie {
    size_t capacity;
    size_t size;
    TRIE_NODE* trieArena;
} TRIE;

TRIE trieGetNew(void);
void triePrint(const TRIE* const trie);
void trieAddString(TRIE* const trie, const int iCmd, const char str[const]);
size_t trieAutocomplete(const TRIE* const trie, const size_t buflen, char usrBuf[const buflen]);
int trieGetCmdIndex(const TRIE* const trie, const size_t buflen, char usrBuf[const buflen]);
int trieDelete(TRIE* const trie);

#endif

#ifndef TRIE_H
#define TRIE_H

#include <stddef.h>

struct TrieNode;

struct Trie {
    struct TrieNode* firstCharList;
};

struct Trie trieInit(void);
void triePrint(const struct Trie* const trie);
void trieAddString(struct Trie* const trie, const char str[const], const int iCmd);
size_t trieAutocomplete(const struct Trie* const trie, const size_t buflen, char usrBuf[const buflen]);
int trieGetCmdIndex(const struct Trie* const trie, const size_t buflen, char usrBuf[const buflen]);
void trieDelete(struct Trie* const trie);

#endif

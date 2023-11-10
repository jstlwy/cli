#ifndef KEYCODES_H
#define KEYCODES_H

#include <stdbool.h>

#define KEY_BACKSPACE 8
#define KEY_TAB       9
#define KEY_ENTER     10
#define KEY_ESCAPE    27
#define KEY_SPACE     32
#define KEY_DELETE    127

extern const char ansiEraseLineSeq[];
extern const char ansiUpSeq[];

bool isVisibleAsciiChar(const char c);

#endif


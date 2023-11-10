#include "keycodes.h"

const char ansiEraseLineSeq[] = "\033[2K\r";
const char ansiUpSeq[] = "\033[A";

bool isVisibleAsciiChar(const char c)
{
    return (c >= '!') && (c <= '~');
}


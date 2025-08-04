OS := $(shell uname)
ifeq ($(OS), Darwin)
CC := clang
else
CC := gcc
endif
CFLAGS := -Wall -Wextra -Werror -pedantic
#DEFINES := -DDEBUG

src := $(wildcard *.c)
hdr := $(wildcard *.h)
obj := $(src:%.c=%.o)
bin := cli

.PHONY: all clean

all: $(bin)

$(bin): $(obj)
	$(CC) $^ -o $@

main.o: main.c $(hdr)
keycodes.o: keycodes.c keycodes.h
commands.o: commands.c commands.h
trie.o: trie.c trie.h
$(obj):
	$(CC) -c $(CFLAGS) $(DEFINES) $< -o $@

clean:
	rm -f $(obj) $(bin)

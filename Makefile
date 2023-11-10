OS := $(shell uname)
CFLAGS := -Wall -Wextra -Werror -Wpedantic
ifeq ($(OS), Darwin)
CC := clang
else
CC := gcc
endif

src := $(wildcard *.c)
hdr := $(wildcard *.h)
obj := $(src:%.c=%.o)
bin := cli

.PHONY: all clean

all: $(bin)

$(bin): $(obj)
	$(CC) $^ -o $@

main.o: main.c $(hdr)
keycodes.o: keycodes.c
commands.o: commands.c
$(obj):
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(obj) $(bin)


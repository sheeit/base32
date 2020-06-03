CFLAGS  = -Wall -Wextra -Werror -pedantic -ansi \
	  -fstack-protector-strong \
	 -O0 -g \
	 -Wno-parentheses
CC      = gcc
PROGRAM = b32.exe
SOURCES = main.c
OBJS    = main.o

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJS): $(SOURCES)

clean: $(OBJS)
	-@rm -f $(OBJS)

clean-all: $(PROGRAM) clean
	-@rm -f $(PROGRAM)

.PHONY: all clean clean-all

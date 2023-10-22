SRC = $(wildcard *.c)
TARGETS = $(SRC:%.c=%.x)

CC = gcc
CFLAGS = -g
LDFLAGS =
LIBS =

all: $(TARGETS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGETS): %.x: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	-rm $(TARGETS) $(SRC:%.c=%.o)


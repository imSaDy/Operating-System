CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -pthread

SOURCES = $(wildcard *.c)

TARGETS = $(SOURCES:.c=)

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
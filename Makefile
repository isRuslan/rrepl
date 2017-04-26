CC = clang

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
			-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
			-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
			-Wuninitialized -Wconversion -Wstrict-prototypes

PREFIX ?= /usr/local

CFLAGS = -fdiagnostics-color=always -std=gnu11 -c -g

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)
DEPS := $(SOURCES:.c=.d)
LIBRARIES := -lreadline
EXECUTABLE := rrepl

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBRARIES)

debug: CFLAGS += -DTYPE=DEBUG    -g3
debug: $(SOURCES)

.c.o:
	$(CC) $< -o $@ $(CFLAGS) $(WARNINGS)

.c.d:
	$(CC) -o $< -MM $(CFLAGS)

install: all
	cp -f rrepl $(PREFIX)/bin/rrepl

uninstall:
	rm -f $(PREFIX)/bin/rrepl

clean:
	rm -rf $(OBJECTS)

-include $(DEPS)

.PHONY: all clean

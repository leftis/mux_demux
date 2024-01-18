CC = gcc
CFLAGS = -Wall -Wextra

# For MacOS
ifeq ($(shell uname), Darwin)
    LDFLAGS = -lpthread
endif

# For Windows (MinGW)
ifeq ($(OS), Windows_NT)
    EXE_EXT = .exe
endif

all: mux demux

mux: mux.c
	$(CC) $(CFLAGS) $< -o $@$(EXE_EXT) $(LDFLAGS)

demux: demux.c
	$(CC) $(CFLAGS) $< -o $@$(EXE_EXT) $(LDFLAGS)

clean:
	rm -f mux$(EXE_EXT) demux$(EXE_EXT)

.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Wextra #-v

# For MacOS
ifeq ($(shell uname), Darwin)
    LDFLAGS = -lpthread -lpcap -lncurses
endif

# For Windows (MinGW)
ifeq ($(OS), Windows_NT)
    EXE_EXT = .exe
endif

# all: generate_data mux demux
all: mux demux

# generate_data:
# 	./gen.py

mux: mux.c data.h vector.h vec.c
	$(CC) $(CFLAGS) $< -o $@$(EXE_EXT) $(LDFLAGS)

demux: demux.c data.h vector.h vec.c
	$(CC) $(CFLAGS) $< -o $@$(EXE_EXT) $(LDFLAGS)

# cpa: cpa.c data.h
# 	$(CC) $(CFLAGS) $< -o $@$(EXE_EXT) $(LDFLAGS)

clean:
	rm -f mux$(EXE_EXT) demux$(EXE_EXT) cpa$(EXE_EXT) out1.txt out2.txt

.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Wextra

# For MacOS
ifeq ($(shell uname), Darwin)
    LDFLAGS = -lpthread -lpcap
endif

# For Windows (MinGW)
ifeq ($(OS), Windows_NT)
    EXE_EXT = .exe
endif

all: generate_data mux demux

generate_data:
	python3 gen.py

mux: mux.c data.h
	$(CC) $(CFLAGS) $< -o $@$(EXE_EXT) $(LDFLAGS)

demux: demux.c data.h
	$(CC) $(CFLAGS) $< -o $@$(EXE_EXT) $(LDFLAGS)

# cpa: cpa.c data.h
# 	$(CC) $(CFLAGS) $< -o $@$(EXE_EXT) $(LDFLAGS)

clean:
	rm -f mux$(EXE_EXT) demux$(EXE_EXT) cpa$(EXE_EXT) data.h

.PHONY: all clean

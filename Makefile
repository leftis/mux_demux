CC = gcc
CFLAGS = -Wall -Wextra #-v
# Flags needed for socket programming
LDFLAGS = -lpcap

# all: generate_data mux demux
all: mux demux

mux: mux.c data.h vector.h vec.c
	$(CC) $(CFLAGS) $< -o $@$(EXE_EXT) $(LDFLAGS)

demux: demux.c data.h vector.h vec.c
	$(CC) $(CFLAGS) $< -o $@$(EXE_EXT) $(LDFLAGS)

clean:
	rm -f mux$(EXE_EXT) demux$(EXE_EXT) cpa$(EXE_EXT) out0.txt out1.txt

.PHONY: all clean

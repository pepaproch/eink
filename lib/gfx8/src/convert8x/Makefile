all: convert8x

CC     = g++
CFLAGS = -Wall

convert8x: convert8x_cs.cpp
	$(CC) $(CFLAGS) $< -o $@
	strip $@

clean:
	rm -f convert8x

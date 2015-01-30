CC=gcc
CFLAGS=-Wall -Wextra -fPIC -O2

all: rlimitr.so rlimitr

clean:
	rm -f rlimitr.so rlimitr

rlimitr.so: rlimitr.c
	$(CC) $(CFLAGS) -shared -DRLIMITR_LIBRARY_ONLY -o rlimitr.so rlimitr.c

rlimitr: rlimitr.c
	$(CC) $(CFLAGS) -o rlimitr rlimitr.c

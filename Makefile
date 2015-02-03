CC?=gcc
CFLAGS+=-Wall -Wextra -fPIC -O2
PREFIX=`[ -e /usr/local/bin ] && echo '/usr/local' || echo '/usr'`

all: rlimitr.so rlimitr

clean:
	rm -f rlimitr.so rlimitr *.gcda *.gcno *.gcov

rlimitr.so: rlimitr.c
	$(CC) $(CFLAGS) -shared -DRLIMITR_LIBRARY_ONLY -o rlimitr.so rlimitr.c

rlimitr: rlimitr.c
	$(CC) $(CFLAGS) -o rlimitr rlimitr.c

install: rlimitr
	cp rlimitr $(PREFIX)/bin/rlimitr

test: rlimitr.so rlimitr
	./test.sh

coverage: clean
	CFLAGS="--coverage" CC=gcc make test

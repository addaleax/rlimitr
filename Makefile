all: rlimitr.so rlimitr

clean:
	rm -f rlimitr.so rlimitr

rlimitr.so: rlimitr.c
	gcc -fPIC -O2 -shared -DRLIMITR_LIBRARY_ONLY -o rlimitr.so rlimitr.c
 
rlimitr: rlimitr.c
	gcc -fPIC -O2 -o rlimitr rlimitr.c

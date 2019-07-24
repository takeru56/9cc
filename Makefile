CFLAGS=-std=c11 -g -static

micc: micc.c

test: micc
				./test.sh

clean:
				rm -f micc *.o *~ tmp*

.PHONY: test clean

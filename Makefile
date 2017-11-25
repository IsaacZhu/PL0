CC=gcc
CFLAGS=-g #-Wall
pl0.o:pl0.c
	${CC} ${CFLAGS} -o $@ $^

clean:
	rm -f *.o *.out

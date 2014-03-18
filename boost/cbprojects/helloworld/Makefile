
BOOSTINC=/usr/local/include/boost
BOOSTLIB=/usr/local/lib

# for c language only: CC=gcc
CC=g++
CFLAGS=-I${BOOSTINC} -L${BOOSTLIB}

PRG = helloworld.out

all: ${PRG}

helloworld.out: main.o
	${CC} ${CFLAGS} -o $@ main.o

clean:
	rm -f *.o ${PRG}

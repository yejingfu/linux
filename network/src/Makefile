include ../Make.defines

PROGS = a1daytime_client.out a1daytime_server.out a3byteorder.out a5echo_server.out a5echo_client.out \
	a6echo_client.out a6echo_server.out

all: ${PROGS}

a1daytime_client.out: a1daytime_client.o util.o
	${CC} ${CFLAGS} -o $@ a1daytime_client.o util.o ${LIBS}

a1daytime_server.out: a1daytime_server.o util.o
	${CC} ${CFLAGS} -o $@ a1daytime_server.o util.o ${LIBS}

a3byteorder.out : a3byteorder.o
	${CC} ${CFLAGS} -o $@ a3byteorder.o ${LIBS}

a5echo_server.out : a5echo_server.o
	${CC} ${CFLAGS} -o $@ a5echo_server.o ${LIBS}

a5echo_client.out : a5echo_client.o
	${CC} ${CFLAGS} -o $@ a5echo_client.o ${LIBS}

a6echo_client.out : a6echo_client.o
	${CC} ${CFLAGS} -o $@ a6echo_client.o ${LIBS}

a6echo_server.out : a6echo_server.o
	${CC} ${CFLAGS} -o $@ a6echo_server.o ${LIBS}

clean:
	rm -f ${PROGS} ${CLEANFILES}


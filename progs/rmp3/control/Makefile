# CFLAGS=-g -D_DEBUG
# LFLAGS=-lmpatrol -lbfd -liberty
CFLAGS=-O2

all:
	gcc -o control_generic.o $(CFLAGS) -c control_generic.c
	gcc -o cgeneric.so -shared $(LFLAGS) control_generic.o -lokienkoc -lavformat -lavcodec

clean:
	rm -f *.o *.so *~

install:
	install -d /usr/local/share/rmp3/control
	install cgeneric.so /usr/local/share/rmp3/control

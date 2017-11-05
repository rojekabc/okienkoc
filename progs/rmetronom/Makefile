# Where to install binary and manpage on "make install":
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

CC=gcc
LDFLAGS=-ldl -lpthread -lao -lprojewski-tools -lm
OBJECTS=rmetronom.o
CFLAGS=-DI386_ASSEM -DPENTIUM_OPT -DREAL_IS_FLOAT -DLINUX \
			-DOSS -DTERM_CONTROL\
			-Wall -O2\
			-fomit-frame-pointer -funroll-all-loops \
			-finline-functions -ffast-math \
			-I/usr/local/include/projewski

rmetronom-make: rmetronom

rmetronom: rmetronom.o Makefile
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

rmetronom.o: rmetronom.c

clean:
	rm -f *.o *core *~ rmetronom

install:
	strip rmetronom
	mkdir -p $(BINDIR)
	cp -f rmetronom $(BINDIR)
	chmod 755 $(BINDIR)/rmetronom

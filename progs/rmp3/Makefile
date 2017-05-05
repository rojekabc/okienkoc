# Where to install binary and manpage on "make install":
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
SHAREDIR=$(PREFIX)/share/rmp3

linux:
	$(MAKE) CC=gcc LDFLAGS="-ldl -lpthread -lavcodec -lavformat -lao -lokienkoc -lm -lavutil" \
		OBJECTS='aumix.o rmp3.o' \
		CFLAGS='$(CFLAGS) -DI386_ASSEM -DPENTIUM_OPT -DREAL_IS_FLOAT -DLINUX \
			-DOSS -DTERM_CONTROL\
			-Wall -O3\
			-fomit-frame-pointer -funroll-all-loops \
			-finline-functions -ffast-math' \
		rmp3-make

rmp3-make: rmp3

rmp3: rmp3.o Makefile aumix.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

rmp3.o:	rmp3.c aumix.h

clean:
	rm -f *.o *core *~ rmp3

install:
	strip rmp3
	install -d $(BINDIR)
	install -m 755 rmp3 $(BINDIR)
	install -d $(SHAREDIR)
	install -m 644 rmp3.logo $(SHAREDIR)

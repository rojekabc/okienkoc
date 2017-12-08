# Elementy wyjsciowe
OBJS = system.o ramka.o element.o napis.o wpis.o \
	przycisk.o lista.o wybor.o pasek.o procent.o \
	maska.o forma.o hotkey.o grupa.o slista.o \
	plista.o timer.o global.o maparaw.o \
	mapapos.o signals.o slidebar.o
TARGETPATH=target
TARGETOBJS = $(patsubst %.o, $(TARGETPATH)/%.o, $(OBJS))

# Flagi kompilacji i linkowania
CFLAGS = -Wall -fPIC
LFLAGS = -shared -lpthread -lprojewski-tools

include Makefile.properties

ifeq ($(DEBUG),true)
	CFLAGS += -g -O0 -D_DEBUG
	LFLAGS += memwatch.o
else
	CFLAGS += -O2 -s
	LFLAGS += -s
endif

ifeq ($(TERM_STD),true)
	CFLAGS += -DHAS_TERM_STD
	OBJS += term-std.o
endif
ifeq ($(TERM_RAW),true)
endif
ifeq ($(TERM_X11),true)
endif

	
OUTLIB = libprojewski-okienkoc.so.1.0
TARGETOUTLIB = $(TARGETPATH)/$(OUTLIB)
LIBLINK = libprojewski-okienkoc.so
TARGETLIBLINK = $(TARGETPATH)/$(LIBLINK)

# Wynikowy plik bledow
OUTERROR = ERRORS

# Elementy dodatkowe include
INCLUDEPATH =
#INCLUDEFILE = $(INCLUDEPATH)okienko.h
INCLUDEPATHS = -I/usr/local/include/projewski

# number of bits
LBITS = $(shell getconf LONG_BIT)

# Miejsca instalacji
PREFIX = /usr/local
ifeq ($(LBITS), 64)
	PREFIXLIB = $(PREFIX)${exec_prefix}/lib64
else ifeq ($(LBITS), 32)
	PREFIXLIB = $(PREFIX)${exec_prefix}/lib
else
	$(error Unknown $(LBITS)-bit system)
endif
PREFIXINCLUDE = $(PREFIX)/include/projewski/okienkoc

# TERMINALLIBS=libterm-linux.so libterm-xterm.so

all: prepare $(TARGETOUTLIB) $(TERMINALLIBS)

debug: preparedebug memwatch.o $(TARGETOUTLIB) $(TERMINALLIBS)

prepare:
	@echo "Preparing..."
	@rm -f ERRORS
	@rm -rf target
	@mkdir target

preparedebug:
	@echo "Preparing..."
	@rm -f ERRORS

$(TARGETOBJS): target/%.o: %.c $(INCLUDEFILE)
	@echo "--- Compile $< ---" >> $(OUTERROR)
	@echo $"Compile $<$"
	@gcc $< $(CFLAGS) -c -o $@ $(INCLUDEPATHS) $(DEBUGFLAGS) 2>> $(OUTERROR)

$(TARGETOUTLIB): $(TARGETOBJS)
	@echo $"--- Link $@ ---$" >> $(OUTERROR)
	@echo $"Link $@$"
	@gcc $(LFLAGS) -o $@ $(TARGETOBJS) $(LIBSIN) 2>> $(OUTERROR)
	@ln -sf $(OUTLIB) $(TARGETLIBLINK)

clean:
	@echo $"Cleaning$"
	@rm -f $(TARGETOBJS) $(TARGETOUTLIB) $(OUTERROR) $(TARGETLIBLINK) *.log *~ $(TERMINALLIBS) .*.un~

install:
	install  $(TARGETOUTLIB) $(PREFIXLIB)
	ln -sf $(OUTLIB) $(PREFIXLIB)/$(LIBLINK)
	install -d $(PREFIXINCLUDE)
	install -m 0644 *.h $(PREFIXINCLUDE)
#	install -d $(PREFIX)/share/projewski/okienkoc
#	install $(TERMINALLIBS) $(PREFIX)/share/projewski/okienkoc

uninstall:
	rm -f $(PREFIXLIB)/$(OUTLIB) $(PREFIXLIB)/$(LIBLINK)
	rm -rf $(PREFIXINCLUDE)
	rm -rf $(PREFIX)/share/projewski/okienkoc

# libterm-linux.so: term-linux.c
# 	@echo "--- Compile $< ---" >> $(OUTERROR)
#	@gcc -o $@ -shared $(CFLAGS) $(INCLUDEPATHS) $< 2>> $(OUTERROR)
#
# libterm-xterm.so: term-xterm.c
#	@echo "--- Compile $< ---" >> $(OUTERROR)
#	@gcc -o $@ -shared $(CFLAGS) $< -lX11 $(INCLUDEPATHS) 2>> $(OUTERROR)

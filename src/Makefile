# Elementy wyjsciowe
OBJS = system.o tablica.o term.o ramka.o screen.o element.o napis.o wpis.o \
	mystr.o przycisk.o lista.o wybor.o pasek.o procent.o nbitpola.o \
	maska.o forma.o plik.o font.o bits.o hotkey.o grupa.o slista.o \
	plista.o timer.o global.o maparaw.o \
	mapapos.o signals.o istream.o fileistream.o memistream.o tableiterator.o \
	iterator.o properties.o arguments.o memostream.o ostream.o mempipestream.o \
	slidebar.o
TARGETPATH=target
TARGETOBJS = $(patsubst %.o, $(TARGETPATH)/%.o, $(OBJS))

	
OUTLIB = libokienkoc.so.0.0
TARGETOUTLIB = $(TARGETPATH)/$(OUTLIB)
LIBLINK = libokienkoc.so
TARGETLIBLINK = $(TARGETPATH)/$(LIBLINK)

# Wynikowy plik bledow
OUTERROR = ERRORS

# Elementy dodatkowe include
INCLUDEPATH =
#INCLUDEFILE = $(INCLUDEPATH)okienko.h
INCLUDEPATHS =

# Miejsca instalacji
PREFIX = /usr/local
PREFIXLIB = $(PREFIX)${exec_prefix}/lib
PREFIXINCLUDE = $(PREFIX)/include/okienkoc

# Flagi kompilacji i linkowania
# DEBUGCFLAGS = -Wall -g -D_DEBUG -I.
DEBUGCFLAGS = -Wall -g -O0 -D_DEBUG -fPIC
NORMALCFLAGS = -Wall -O2 -s -fPIC
# DEBUGLFLAGS = -shared
# DEBUGLFLAGS = -shared -lxml2
DEBUGLFLAGS = -shared -lpthread memwatch.o
NORMALLFLAGS = -s -shared -ldl -lpthread

# Pliki wyjsciowe dla zapisania flag kompilacji / debugowania
CFLAGSFILE = .cflags
LFLAGSFILE = .lflags

TERMINALLIBS=libterm-linux.so libterm-xterm.so

all: prepare $(TARGETOUTLIB) $(TERMINALLIBS)

debug: preparedebug memwatch.o $(TARGETOUTLIB) $(TERMINALLIBS)

prepare:
	@echo "Preparing..."
	@rm -f ERRORS
	@rm -rf target
	@mkdir target
	@echo "$(NORMALCFLAGS)" > $(CFLAGSFILE)
	@echo "$(NORMALLFLAGS)" > $(LFLAGSFILE)

preparedebug:
	@echo "Preparing..."
	@rm -f ERRORS
	@echo "$(DEBUGCFLAGS)" > $(CFLAGSFILE)
	@echo "$(DEBUGLFLAGS)" > $(LFLAGSFILE)

$(TARGETOBJS): target/%.o: %.c $(INCLUDEFILE)
	@echo "--- Compile $< ---" >> $(OUTERROR)
	@echo $"Compile $<$"
	@gcc $< `cat $(CFLAGSFILE)` -c -o $@ $(INCLUDEPATHS) $(DEBUGFLAGS) 2>> $(OUTERROR)

$(TARGETOUTLIB): $(TARGETOBJS)
	@echo $"--- Link $@ ---$" >> $(OUTERROR)
	@echo $"Link $@$"
	@gcc `cat $(LFLAGSFILE)` -o $@ $(TARGETOBJS) $(LIBSIN) 2>> $(OUTERROR)
	@ln -sf $(OUTLIB) $(TARGETLIBLINK)

clean:
	@echo $"Cleaning$"
	@rm -f $(TARGETOBJS) $(TARGETOUTLIB) $(OUTERROR) $(TARGETLIBLINK) .cflags .lflags *.log *~ $(TERMINALLIBS) .*.un~

install:
	install  $(TARGETOUTLIB) $(PREFIXLIB)
	ln -sf $(OUTLIB) $(PREFIXLIB)/$(LIBLINK)
	install -d $(PREFIXINCLUDE)
	install -m 0644 *.h $(PREFIXINCLUDE)
	install -d $(PREFIX)/share/okienkoc
	install $(TERMINALLIBS) $(PREFIX)/share/okienkoc

uninstall:
	rm -f $(PREFIXLIB)/$(OUTLIB) $(PREFIXLIB)/$(LIBLINK)
	rm -rf $(PREFIXINCLUDE)
	rm -rf $(PREFIX)/share/okienkoc

libterm-linux.so: term-linux.c
	@echo "--- Compile $< ---" >> $(OUTERROR)
	@gcc -o $@ -shared `cat $(CFLAGSFILE)` $< 2>> $(OUTERROR)

libterm-xterm.so: term-xterm.c
	@echo "--- Compile $< ---" >> $(OUTERROR)
	@gcc -o $@ -shared `cat $(CFLAGSFILE)` $< -lX11 2>> $(OUTERROR)

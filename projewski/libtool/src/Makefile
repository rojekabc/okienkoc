# Elementy wyjsciowe
OBJS = tablica.o term.o screen.o mystr.o nbitpola.o plik.o font.o bits.o \
	istream.o fileistream.o memistream.o tableiterator.o \
	iterator.o properties.o arguments.o memostream.o ostream.o mempipestream.o
TARGETPATH=target
TARGETOBJS = $(patsubst %.o, $(TARGETPATH)/%.o, $(OBJS))

	
OUTLIB = libprojewski-tools.so.0.1
TARGETOUTLIB = $(TARGETPATH)/$(OUTLIB)
LIBLINK = libprojewski-tools.so
TARGETLIBLINK = $(TARGETPATH)/$(LIBLINK)

# Wynikowy plik bledow
OUTERROR = ERRORS

# Elementy dodatkowe include
INCLUDEPATH =
#INCLUDEFILE = $(INCLUDEPATH)okienko.h
INCLUDEPATHS =

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
PREFIXINCLUDE = $(PREFIX)/include/projewski/tools

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

all: prepare $(TARGETOUTLIB)

debug: preparedebug memwatch.o $(TARGETOUTLIB)

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
	@rm -f $(TARGETOBJS) $(TARGETOUTLIB) $(OUTERROR) $(TARGETLIBLINK) .cflags .lflags *.log *~ .*.un~

install:
	install  $(TARGETOUTLIB) $(PREFIXLIB)
	ln -sf $(OUTLIB) $(PREFIXLIB)/$(LIBLINK)
	install -d $(PREFIXINCLUDE)
	install -m 0644 *.h $(PREFIXINCLUDE)

uninstall:
	rm -f $(PREFIXLIB)/$(OUTLIB) $(PREFIXLIB)/$(LIBLINK)
	rm -rf $(PREFIXINCLUDE)

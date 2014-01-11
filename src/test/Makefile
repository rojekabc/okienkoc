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
DEBUGCFLAGS = -Wall -g -I/usr/include/libxml2 -D_DEBUG
NORMALCFLAGS = -Wall -O2 -s -I/usr/include/libxml2
# DEBUGLFLAGS = -shared
# DEBUGLFLAGS = -shared -lxml2
DEBUGLFLAGS = -lxml2 -lpthread -lX11 -lokienkoc
NORMALLFLAGS = -s -lxml2 -ldl -lpthread -lX11 -lokienkoc

# Pliki wyjsciowe dla zapisania flag kompilacji / debugowania
CFLAGSFILE = .cflags
LFLAGSFILE = .lflags

TESTPROG = test001 test002 test003 test004 testlog testiterator \
	testlosuj testterm teststream test-nbit testproperties teststring \
	testprint testmultitable testtable testxterm testmempipe testarguments

all: prepare $(TESTPROG) libtestlib.so

debug: preparedebug $(TESTPROG) libtestlib.so

prepare:
	@echo "Preparing..."
	@rm -f ERRORS
	@echo "$(NORMALCFLAGS)" > $(CFLAGSFILE)
	@echo "$(NORMALLFLAGS)" > $(LFLAGSFILE)

preparedebug:
	@echo "Preparing..."
	@rm -f ERRORS
	@echo "$(DEBUGCFLAGS)" > $(CFLAGSFILE)
	@echo "$(DEBUGLFLAGS)" > $(LFLAGSFILE)

clean:
	@echo $"Cleaning$"
	@rm -f $(OUTERROR) $(TESTPROG) .cflags .lflags *.log *~ libtestlib.so

libtestlib.so:
	gcc -o libtestlib.so -shared testlib-fob.c

$(TESTPROG): %: %.c
	@echo $"--- Compile $< ---$" >> $(OUTERROR)
	@echo $"Compile $<$"
	@gcc -o $@ $< `cat $(LFLAGSFILE)` -L. `cat $(CFLAGSFILE)` 2>> $(OUTERROR)


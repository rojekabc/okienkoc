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
DEBUGCFLAGS = -Wall -g -O0 -I/usr/include/libxml2 -D_DEBUG
NORMALCFLAGS = -Wall -O2 -s -I/usr/include/libxml2
# DEBUGLFLAGS = -shared
# DEBUGLFLAGS = -shared -lxml2
DEBUGLFLAGS = -shared -lxml2 -lpthread
NORMALLFLAGS = -s -shared -lxml2 -ldl -lpthread

# Pliki wyjsciowe dla zapisania flag kompilacji / debugowania
CFLAGSFILE = .cflags
LFLAGSFILE = .lflags

TESTPROG = sample-00 sample-01 sample-02 sample-03 sample-04 sample-05 \
	   sample-06 sample-07 sample-08 sample-09 sample-10 \
	   sample-11 sample-12 sample-13 sample-14 sample-15 sample-16 \
	   sample-17 sample-18 sample-19 sample-20

all: prepare $(TESTPROG)

debug: preparedebug $(TESTPROG)

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
	@rm -f $(OUTERROR) $(TESTPROG) .cflags .lflags *.log *~

$(TESTPROG): %: %.c
	@echo $"--- Compile $< ---$" >> $(OUTERROR)
	@echo $"Compile $<$"
	@gcc -o $@ $< -lokienkoc -L. `cat $(CFLAGSFILE)` 2>> $(OUTERROR)


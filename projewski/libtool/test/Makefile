# Wynikowy plik bledow
OUTERROR = ERRORS

# Miejsca instalacji
PREFIX = /usr/local
PREFIXLIB = $(PREFIX)${exec_prefix}/lib
PREFIXINCLUDE = $(PREFIX)/include/projewski


# Flagi kompilacji i linkowania
# DEBUGCFLAGS = -Wall -g -D_DEBUG -I.
DEBUGCFLAGS = -Wall -g -D_DEBUG -I$(PREFIXINCLUDE)
NORMALCFLAGS = -Wall -O2 -s -I$(PREFIXINCLUDE)
# DEBUGLFLAGS = -shared
# DEBUGLFLAGS = -shared -lxml2
DEBUGLFLAGS = -lpthread -lX11 -lprojewski-tools
NORMALLFLAGS = -s -ldl -lpthread -lX11 -lprojewski-tools

# Pliki wyjsciowe dla zapisania flag kompilacji / debugowania
CFLAGSFILE = .cflags
LFLAGSFILE = .lflags

OSTYPE=$(shell echo "$${OSTYPE}")

TESTPROG = test001 test003 testlog testiterator \
	testlosuj testterm teststream test-nbit testproperties teststring \
	testprint testmultitable testtable testmempipe testarguments

ifneq ($(findstring darwin,$(OSTYPE)), darwin)
	TESTPROG +=  test002 testxterm
endif
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
	@rm -f $(OUTERROR) $(TESTPROG) .cflags .lflags *.log *~ .*.un~

$(TESTPROG): %: %.c
	@echo $"--- Compile $< ---$" >> $(OUTERROR)
	@echo $"Compile $<$"
	@gcc -o $@ $< `cat $(LFLAGSFILE)` -L. `cat $(CFLAGSFILE)` 2>> $(OUTERROR)


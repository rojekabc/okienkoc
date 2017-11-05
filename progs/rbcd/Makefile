# Wynikowy plik bledow
OUTERROR = ERRORS

# Flagi kompilacji i linkowania
# DEBUGCFLAGS = -Wall -g -D_DEBUG -I.
DEBUGCFLAGS = -Wall -g -I/usr/include/libxml2 -D_DEBUG -I/usr/local/include/projewski -fPIC
NORMALCFLAGS = -Wall -O2 -s -I/usr/include/libxml2 -I/usr/local/include/projewski -fPIC
# DEBUGLFLAGS = -shared
# DEBUGLFLAGS = -shared -lxml2
DEBUGLFLAGS = -lprojewski-tools
NORMALLFLAGS = -lprojewski-tools

# Pliki wyjsciowe dla zapisania flag kompilacji / debugowania
CFLAGSFILE = .cflags
LFLAGSFILE = .lflags

OBJS = bcd.o test.o
TESTPROG = test

all: prepare $(TESTPROG) $(OBJS)

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
	@rm -f $(OUTERROR) $(TESTPROG) .cflags .lflags *.log *~ $(OBJS)

$(TESTPROG): $(OBJS)
	@echo $"--- Link $@ ---$" >> $(OUTERROR)
	@echo $"Link $@$"
	@gcc -o $@ $(OBJS) -L. `cat $(LFLAGSFILE)` 2>> $(OUTERROR)

$(OBJS): %.o: %.c
	@echo $"--- Compile $< ---$" >> $(OUTERROR)
	@echo $"Compile $<$"
	@gcc -o $@ -c $< `cat $(CFLAGSFILE)` 2>> $(OUTERROR)

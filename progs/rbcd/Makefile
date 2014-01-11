# Wynikowy plik bledow
OUTERROR = ERRORS

# Flagi kompilacji i linkowania
# DEBUGCFLAGS = -Wall -g -D_DEBUG -I.
DEBUGCFLAGS = -Wall -g -I/usr/include/libxml2 -D_DEBUG
NORMALCFLAGS = -Wall -O2 -s -I/usr/include/libxml2
# DEBUGLFLAGS = -shared
# DEBUGLFLAGS = -shared -lxml2
DEBUGLFLAGS = -shared -lxml2 -lpthread
NORMALLFLAGS = -s -shared -lxml2 -ldl -lpthread

# Pliki wyjsciowe dla zapisania flag kompilacji / debugowania
CFLAGSFILE = .cflags
LFLAGSFILE = .lflags

OBJS = bcd.o
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

$(TESTPROG): %: %.c $(OBJS)
	@echo $"--- Compile $< ---$" >> $(OUTERROR)
	@echo $"Compile $<$"
	@gcc -o $@ $< -lokienkoc -L. `cat $(CFLAGSFILE)` $(OBJS) 2>> $(OUTERROR)

$(OBJS): %.o: %.c
	@echo $"--- Compile $< ---$" >> $(OUTERROR)
	@echo $"Compile $<$"
	@gcc -o $@ -c $< -lokienkoc -L. `cat $(CFLAGSFILE)` 2>> $(OUTERROR)

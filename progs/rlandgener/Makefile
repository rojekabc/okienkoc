OBJS = 
	
OUTLIB = 
LIBLINK =

INCLUDEPATH =
INCLUDEPATHS =

OUTERROR = ERRORS

PREFIX = /usr/local
PREFIXLIB = $(PREFIX)${exec_prefix}/lib
PREFIXINCLUDE = $(PREFIX)/include/okienkoc

DEBUGCFLAGS = -Wall -g -D_DEBUG
NORMALCFLAGS = -Wall -O2 -s
DEBUGLFLAGS = -shared -lmpatrol -lbfd -liberty
NORMALLFLAGS = -s -shared

CFLAGSFILE = .cflags
LFLAGSFILE = .lflags

PROG = rlandgener
PROGLIBS = -lokienkoc -lm
PROGINCLUDE = -I/usr/local/include

all: prepare $(OUTLIB) $(PROG)

debug: preparedebug $(OUTLIB) $(PROG)

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

$(OBJS): %.o: %.c $(INCLUDEFILE)
	@echo "--- Compile $< ---" >> $(OUTERROR)
	@echo $"Compile $<$"
	@gcc $< `cat $(CFLAGSFILE)` -c -o $@ $(INCLUDEPATHS) $(DEBUGFLAGS) 2>> $(OUTERROR)

$(OUTLIB): $(OBJS)
	@echo $"--- Link $@ ---$" >> $(OUTERROR)
	@echo $"Link $@$"
	@gcc `cat $(LFLAGSFILE)` -o $@ $(OBJS) $(LIBSIN) 2>> $(OUTERROR)
	@ln -sf $(OUTLIB) $(LIBLINK)

clean:
	@echo $"Cleaning$"
	@rm -f $(OBJS) $(OUTLIB) $(OUTERROR) $(LIBLINK) $(PROG) .cflags .lflags *~

install:
	install  $(OUTLIB) $(PREFIXLIB)
	ln -sf $(OUTLIB) $(PREFIXLIB)/$(LIBLINK)
	install -d $(PREFIXINCLUDE)
	install -m 0644 *.h $(PREFIXINCLUDE)

uninstall:
	rm -f $(PREFIXLIB)/$(OUTLIB) $(PREFIXLIB)/$(LIBLINK)
	rm -rf $(PREFIXINCLUDE)

$(PROG): %: %.c
	@echo $"--- Compile $< ---$" >> $(OUTERROR)
	@echo $"Compile $<$"
	@gcc -o $@ $< $(PROGLIBS) $(PROGINCLUDE) `cat $(CFLAGSFILE)` 2>> $(OUTERROR)


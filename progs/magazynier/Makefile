OBJ=$(patsubst %.c,%.o,$(wildcard *.c))
ERRORFILE=ERRORS
COMPILER=gcc
LIBS=-lprojewski-okienkoc -lprojewski-tools
PROGNAME=magazynier
INCLUDE=-I/usr/local/include/projewski
# DEBUGFLAG=-g -D_DEBUG
# DEBUGLFLAGS=-lmpatrol -lbfd -liberty

all: prepare $(OBJ) $(PROGNAME)

prepare:
	rm -f $(ERRORFILE)

%.o: %.c
	$(COMPILER) $(INCLUDE) $(DEBUGFLAG) -c -o $@ $< 2>> $(ERRORFILE)

$(PROGNAME): $(OBJ)
	gcc -o $(PROGNAME) $(OBJ) $(LIBS) $(DEBUGLFLAGS) 2>> $(ERRORFILE)

clean:
	rm -f $(OBJ) $(ERRORFILE) $(PROGNAME) *~ .*.un~

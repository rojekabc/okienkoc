BUILD=raaow caaow rxml paaow

all: prepare $(BUILD)

prepare:
	@rm -f ERRORS

raaow: raaow.c
	gcc -o $@ $< -g 2>> ERRORS

caaow: caaow.c aaow.c aaow.h
	gcc -o $@ $< aaow.c -I/usr/include/libxml2 -lxml2 -lokienkoc -g 2>> ERRORS

rxml: rxml.c aaow.c aaow.h
	gcc -o $@ $< aaow.c aaow.h -I/usr/include/libxml2 -lxml2 -lokienkoc -g 2>> ERRORS

paaow: paaow.c aaow.c aaow.h
	gcc -o $@ $< aaow.c aaow.h -I/usr/include/libxml2 -lxml2 -lokienkoc -g 2>> ERRORS

clean:
	rm -rf $(BUILD) *~ ERRORS

BUILD=raaow caaow rxml paaow

all: prepare $(BUILD)

prepare:
	@rm -f ERRORS

raaow: raaow.c
	gcc -o $@ $< -g 2>> ERRORS

caaow: caaow.c aaow.c aaow.h common.c common.h original.h
	gcc -o $@ $< aaow.c common.c -I/usr/include/libxml2 -lxml2 -lokienkoc -g 2>> ERRORS

rxml: rxml.c aaow.c aaow.h common.c common.h
	gcc -o $@ $< aaow.c common.c -I/usr/include/libxml2 -lxml2 -lokienkoc -g 2>> ERRORS

paaow: paaow.c aaow.c aaow.h common.c common.h
	gcc -o $@ $< aaow.c common.c -I/usr/include/libxml2 -lxml2 -lokienkoc -g 2>> ERRORS

clean:
	rm -rf $(BUILD) *~ ERRORS .*~

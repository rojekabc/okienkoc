BUILD=raaow caaow rxml paaow

all: $(BUILD)

raaow: raaow.c
	gcc -o $@ $< -g

caaow: caaow.c aaow.c aaow.h
	gcc -o $@ $< aaow.c -I/usr/include/libxml2 -lxml2 -lokienkoc -g

rxml: rxml.c aaow.c aaow.h
	gcc -o $@ $< aaow.c aaow.h -I/usr/include/libxml2 -lxml2 -lokienkoc -g

paaow: paaow.c aaow.c aaow.h
	gcc -o $@ $< aaow.c aaow.h -I/usr/include/libxml2 -lxml2 -lokienkoc -g

clean:
	rm -rf $(BUILD) *~

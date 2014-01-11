all: rcip

rcip: rcip.c
	gcc -o rcip rcip.c -lokienkoc -Wall

clean:
	rm -f *~ rcip

install: rcip
	cp rcip /usr/local/bin

test: rcip
	rcip -c 123ABC rcip.c > rcip.enc
	rcip -c 123ABC rcip.enc > rcip.dec
	diff -s rcip.dec rcip.c
	rm -f rcip.enc rcip.dec

all:
	gcc -g -o playlist.o -c playlist.c
	gcc -o playlist.so -shared playlist.o -lokienkoc
	
clean:
	rm -f *.o *.so *~

install:
	install -d /usr/local/share/rmp3/playlist
	install playlist.so /usr/local/share/rmp3/playlist

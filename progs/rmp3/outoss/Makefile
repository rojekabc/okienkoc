all:
	gcc -o audio_oss.o -c audio_oss.c 2> OUT.TXT
	gcc -o outoss.so -shared audio_oss.o 2>> OUT.TXT

clean:
	rm -f *.o *.so

install:
	install -d /usr/local/share/mpg123_roj/output
	install outoss.so /usr/local/share/mpg123_roj/output

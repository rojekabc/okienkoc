#ifndef _GOC_ISTREAM_H_
#define _GOC_ISTREAM_H_

// Rezygnujê z funkcji sk³adowej readArray - bêdzie budowana
// tylko func readChar - zak³adam, ¿e nie wyst±pi a¿ tak znacz±ce
// spowolnienie w odczytywaniu w stosunku do pojedynczej iteracji

#define GOC_STRUCT_ISTREAM \
	int (*readChar)(struct GOC_IStream *); \
	int (*close)(struct GOC_IStream *); \
	int (*available)(struct GOC_IStream *); \
	int (*seek)(struct GOC_IStream *, long offset, int whence)

typedef struct GOC_IStream
{
	GOC_STRUCT_ISTREAM;
} GOC_IStream;

// >=0 odczytany znak
// -1 EOF
// -2 GOC_ERROR
int goc_isReadChar(GOC_IStream *is);

// >= 0 liczba odczytanych znakow
// -1 EOF
// -2 GOC_ERROR
int goc_isReadArray(GOC_IStream *is, unsigned char *pBuffer, unsigned int nBuffer);

// Odczytanie ze strumienia lini, która powinna byæ zakoñczona znakiem
// nowej linii. Znak ten nie jest przekazywany do strumienia.
// Zwrócony ci±g znaków zostanie zakoñczony znakiem koñca ci±gu.
// Je¿eli nie uda siê nic odczytaæ z powodu koñca pliku lub b³êdów
// zostanie zwrócone NULL
char *goc_isReadLine(GOC_IStream *is);

// == 0 OK
// -2 GOC_ERROR
int goc_isClose(GOC_IStream *is);

// >=0 available bytes
// -2 GOC_ERROR
int goc_isAvailable(GOC_IStream *is);

int goc_isSetPos(GOC_IStream *is, long pos);

int goc_isSeek(GOC_IStream *is, long offset, int whence);


#endif // ifndef _GOC_ISTREAM_H_

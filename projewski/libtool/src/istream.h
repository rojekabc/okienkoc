#ifndef _GOC_ISTREAM_H_
#define _GOC_ISTREAM_H_

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

// Odczytanie ze strumienia lini, kt�ra powinna by� zako�czona znakiem
// nowej linii. Znak ten nie jest przekazywany do strumienia.
// Zwr�cony ci�g znak�w zostanie zako�czony znakiem ko�ca ci�gu.
// Je�eli nie uda si� nic odczyta� z powodu ko�ca pliku lub b��d�w
// zostanie zwr�cone NULL
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

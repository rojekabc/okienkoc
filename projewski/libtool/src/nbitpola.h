#ifndef _GOC_NBITFIELD_H_
#define _GOC_NBITFIELD_H_

typedef struct GOC_StNBitField
{
	unsigned char *dane;	//tablica danych
	unsigned short lbnw;	// liczba bitow na wartosc
	unsigned int ld;	// liczba danych
	void *handler;		// funckje operujace
} GOC_StNBitField;

GOC_StNBitField *goc_nbitFieldCreate(unsigned short lbnw, unsigned int ld);
GOC_StNBitField *goc_nbitFieldRemove(GOC_StNBitField *pole);
void goc_nbitFieldSet(GOC_StNBitField *pole, unsigned int numer,
	register unsigned int wartosc);
unsigned int goc_nbitFieldGet(GOC_StNBitField *pole, unsigned int n);
void goc_nbitFieldAdd(GOC_StNBitField *pole, unsigned int wartosc);
void goc_nbitFieldInsert(GOC_StNBitField *pole, unsigned int pos, unsigned int wartosc);
void goc_nbitFieldClear(GOC_StNBitField *pole);
void goc_nbitFieldCut(GOC_StNBitField *pole, unsigned int pos);
char *goc_nbitFieldToString(GOC_StNBitField *pole);
void goc_nbitFieldFromString(GOC_StNBitField *pole, const char *str);
/*
 * zwraca rozmiar w bajtach, jakie zajmuje tablica dane dla danego pola
 * bitowego
 */
unsigned int goc_nbitFieldByteSize(GOC_StNBitField *pole);
#endif // ifndef _GOC_NBITFIELD_H_

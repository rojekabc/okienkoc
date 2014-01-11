#ifndef __BCD_H__
#define __BCD_H__

/* struktura BCD
 * Najni¿sze warto¶ci tablicy stanowi± najstarsz± czê¶æ liczby.
 * Nie wykorzystywane s± 4 najstarsze bity ka¿dego int'a w celu uwzglêdnienia
 * przeniesieñ. A wiêc w jednym int (4 Bajty) jest przechowywanych 7 cyfr
 * w kodzie upakowanym BCD.
 * Je¿eli 4 najstarsze bity w pierwszym elemencie tablicy daj± warto¶æ 0xF,
 * to oznacza to, ¿e jest to liczba ujemna.
 */
typedef struct {
	/* wskazanie na dane tablicy z kodami BCD */
	unsigned int *tab;
	/* rozmiar tablicy - liczba struktur unsigned int */
	unsigned short size;
} BCD;

void BCDfree( BCD *inp );
BCD *BCDalloc( unsigned int numOfChars );
BCD *BCDallocTable( BCD *inp, unsigned int numOfChars );
BCD *BCDfromAsc(const char *inp, BCD *outp);
char *BCDtoAsc( const BCD* inp, char* outp, unsigned int* len );
int BCDvalid( const BCD *inp );
int BCDcompare( const BCD *inp1, const BCD *inp2 );
BCD *BCDcopy( const BCD *inp, BCD *outp );
BCD *BCDadd( const BCD *inp1, const BCD *inp2, BCD *outp );
BCD *BCDtencomp( BCD *inp );
BCD* BCDdouble( const BCD *a, BCD *outp );
BCD *BCDmulInt( const BCD *a, unsigned int mul, BCD *outp );
int BCDcompare( const BCD *a, const BCD *b );
BCD *BCDshiftRight( const BCD *a, unsigned int shift, BCD *outp );

#endif // ifndef __BCD_H__

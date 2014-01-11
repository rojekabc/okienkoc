#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "bcd.h"

// liczy ile znaków moze pomiesciæ dana struktura maksymalnie
#define NUMOFCHARS(BCD) ((BCD->size<<1)*sizeof(int)-BCD->size)

// zwolnij pamiêæ zajmowan± przez strukturê, ³±cznie z sam± struktur±
void BCDfree( BCD *inp )
{
	if ( !inp )
		return;
	if ( inp->tab )
		free( inp->tab );
	inp->tab = NULL;
	inp->size = 0;
	free( inp );
}

// zaallokuj now± strukturê i przydziel jej zasobów na wskazan± liczbê cyfr
BCD *BCDalloc( unsigned int numOfChars )
{
	BCD *outp;
	outp = (BCD *)malloc( sizeof( BCD ) );
	numOfChars += 6; /* Aby zaalokowa³ dla 1 1ui, a dla 0 0ui, dla 7 1ui, 8 2ui*/
	numOfChars /= 7;
	outp->size = numOfChars;
	numOfChars *= sizeof( int );
	outp->tab = (unsigned int *)malloc( numOfChars );
	memset( outp->tab, 0, numOfChars );
	return outp;
}

// je¿eli inp == NULL  =>  allokowanie nowej struktury o obszarze mog±cym
// pomie¶ciæ dan± liczbê cyfr.
// je¿eli inp != NULL  =>  zmieñ rozmiar tablicy na podany rozmiar
// (z równoczesnym utworzeniem kopii starej zawarto¶ci danych).
BCD *BCDallocTable( BCD *inp, unsigned int numOfChars )
{
	unsigned int *oldtab; // jesli zmiana rozmiaru zachowaj stare dane
	unsigned int oldsize;

	if ( !inp )
		return BCDalloc( numOfChars );
	oldtab = inp->tab;
	oldsize = inp->size;

	numOfChars += 6; /* Aby zaalokowa³ dla 1 1ui, a dla 0 0ui, dla 7 1ui, 8 2ui*/
	numOfChars /= 7;
	inp->size = numOfChars;
	numOfChars *= sizeof( int );
	inp->tab = (unsigned int *)malloc( numOfChars );
	memset( inp->tab, 0, numOfChars );
	if ( oldtab ) // kopiuj stare dane
	{
		if ( oldsize <= inp->size )
		{
			if ( oldsize )
				memcpy(inp->tab+(inp->size-oldsize), oldtab, oldsize*sizeof(int));
		}
		else
		{
			if ( inp->size )
				memcpy(inp->tab, oldtab+(oldsize-inp->size), inp->size*sizeof(int));
		}
	}
	if ( oldtab ) // usuñ stare dane
		free( oldtab );
	return inp;
}

// Z podanego ci±gu cyfr utwórz strukturê BCD.
// Je¿eli outp == NULL  =>  nast±pi zaallokowanie nowej struktury
// Je¿eli outp != NULL  =>  dostosuj rozmiar i wype³nij podanymi danymi
BCD *BCDfromAsc(const char *inp, BCD *outp)
{
	unsigned int n; /* d³ugó¶æ ci±gu znaków */
	unsigned int *p; /* wskazanie aktualnie zmienianego int'a w BCD */
	unsigned char c; /* licznik odliczaj±cy do 7 - liczby cyfr w BCD/int */
	unsigned char isNegative = 0;

	if ( !inp )
		return NULL;
	/* Inicjowanie */
	n = strlen( inp );
	if ( *inp == '-' )
	{
		n--;
		inp++;
		isNegative |= 1;
	}
	outp = BCDallocTable( outp, n );
	memset( outp->tab, 0, outp->size*sizeof(unsigned int) );
	p = outp->tab;
	c = 7 - (n % 7);
	for ( ; *inp; inp++ )
//	while ( *inp )
	{
		*p <<= 4;
		*p |= *inp & 0x0F;
		c++;
		c %= 7;
		if ( !c )
			p++;
//		inp++;
	}

	if ( isNegative )
		outp = BCDtencomp( outp );
		
	return outp;
}

// Zamiana struktury BCD na ci±g cyfr czytelnych przez cz³owieczka
char *BCDtoAsc( const BCD* inp, char* outp, unsigned int* len )
{
	unsigned short i;
	unsigned int t;
	unsigned char j;
	char *wsk = outp;

	if ( (inp->tab[0] & 0xF0000000) == 0xF0000000 ) // z minusem
	{
		char *ret;
		BCD *x = BCDcopy( inp, NULL );
		BCDtencomp(x);
	//	x->tab[0] &= 0x0FFFFFFF;
		if ( outp )
		{
			*outp = '-';
			outp++;
		}
		ret = BCDtoAsc( x, outp, len );
		(*len)++;
		BCDfree(x);
		return ret;
	}

	/* Licz d³ugo¶æ */
	if ( len )
	{
		*len = inp->size;
		*len *= sizeof(int);
		*len <<= 1;
		*len -= inp->size;
	}

	/* Odejmij liczbê pocz±tkowych zer i przejd¼ do pozycji przekszta³cania na
	 * ciag znaków */
	j ^= j;
	i ^= i;
	t = inp->tab[i];
	while ( i < inp->size )
	{
		t <<= 4;
		j++;
		j %= 7;
		if ( t & 0xF0000000 )
			break;
		else {
			if ( len )
				(*len)--;
		}
		if ( !j )
		{
			i++;
			t = inp->tab[i];
		}
	}

	if ( !outp )
		return NULL;

	/* Umieszczaj w ci±gu */
	while ( i < inp->size )
	{
		*outp = ((t>>28)&(0xF))|'0';
		outp++;
		if ( !j )
		{
			i++;
			t = inp->tab[i];
		}
		t <<= 4;
		j++;
		j %= 7;
	}
	return wsk;
}

// sprawdzenie poprawno¶ci struktury wej¶ciowej (czy podane cyfry mieszcz± siê
// w zakresach)
int BCDvalid( const BCD *inp )
{
	unsigned short i;
	unsigned int t1, t2;
	for ( i ^= i; i < inp->size; i++ )
	{
		t1 = t2 = inp->tab[i];
		t1 += 0x06666666;
		t2 ^= 0x06666666;
		t1 ^= t2;
		t1 &= 0x11111110;
		if ( t1 )
			return 0;
	}
	return 1;
}

/* 
 * Porównanie:
 * <0 inp1 < inp2
 * 0  inp1 = inp2
 * >0 inp1 > inp2
 *
 * TODO: obsluga wartosci ujemnych
 */
int BCDcompare( const BCD *inp1, const BCD *inp2 )
{
	unsigned int i;
	
	if ( inp1->size != inp2->size )
		return inp1->size - inp2->size;

	for ( i ^= i; i < inp1->size; i++ )
	{
		if ( inp1->tab[i] != inp2->tab[i] )
			return inp1->tab[i] - inp2->tab[i];
	}

	return 0;
}

/*
 * Je¿eli rozmiar wynikowego jest wiekszy od wejsciowego, to zostanie
 * wykonana kopia na koniec wyjsciowego. Jezeli wejsciowy jest wiekszy,
 * to wyjsciowy zostanie powiekszony, jesli rowne to prosta kopia
 */
BCD *BCDcopy( const BCD *inp, BCD *outp )
{
	unsigned int *stl;

	if ( !outp )
		outp = BCDalloc( NUMOFCHARS(inp) );
	if ( outp->size != inp->size )
		BCDallocTable( outp, NUMOFCHARS(inp) );
	stl = outp->tab + (outp->size - inp->size);

	memcpy( stl, inp->tab, sizeof(int)*inp->size );

	return outp;
}

// Wykonanie przekszta³cenia, które spowoduje 'zmianê znaku' w strukturze
// i przeformatuje j± na kod zanegowany+1, co umo¿liwi jego pó¼niejsze
// traktowanie niczym liczby dodatniej i dodawanie. Dziêki nadaniu na pierwszych
// czterech bitach warto¶ci 0xF lub 0x0 okre¶lany jest znak
// Zmieniana jest struktura wej¶ciowa inp !
BCD *BCDtencomp( BCD *inp )
{
	unsigned int i = 0;
	for (i = 0; i < inp->size; i++ )
	{
		unsigned int t1 = 0xFFFFFFFF - inp->tab[i];
		unsigned int t2 = -inp->tab[i];
		unsigned int t3 = t1 ^ 0x00000001;
		unsigned int t4 = t2 ^ t3;
		unsigned int t5 = (~t4) & 0x11111110;
		unsigned int t6 = (t5>>2) | (t5>>3);
		inp->tab[i] = t2-t6;
	}
	return inp;
}

// Dodanie do siebie dwóch liczb w reprezentacji BCD i umieszczenie wyniku
// w strukturze outp
BCD *BCDadd( const BCD *inp1, const BCD *inp2, BCD *outp )
{
	const BCD *toAdd;
	unsigned int i;
	unsigned int delta; // ró¿nica wymiarów konieczna do wypozycjonowania
	// sumowanych warto¶ci wzglêdem siebie

	// ustaw tak, by wielko¶æ outp by³a wiêksza od toAdd
	if ( inp1->size > inp2->size )
	{
		outp = BCDcopy(inp1, outp);
		toAdd = inp2;
	}
	else
	{
		outp = BCDcopy(inp2, outp);
		toAdd = inp1;
	}
	delta = outp->size - toAdd->size;
	for ( i = toAdd->size-1; i != (unsigned int)-1; i-- )
	{
		unsigned int t1 = outp->tab[i+delta] + 0x06666666;
		unsigned int t2 = t1 + toAdd->tab[i];
		unsigned int t3 = t1 ^ toAdd->tab[i];
		unsigned int t4 = t2 ^ t3;
		unsigned int t5 = (~t4) & 0x11111110;
		unsigned int t6 = (t5>>2) | (t5>>3);
		outp->tab[i+delta] = t2-t6;
	}
	// Przeniesienia
	for ( i = 1; i < outp->size; i++ ) // Bez najstarszego (bo zmiana rozmiaru)
	{
		if ( outp->tab[i] & 0x10000000 )
		{
//			outp->tab[i-1]++;
//UWAGA ! TO DO KOREKTY - PRZENIESIENIE i DODANIE 1
// Obecnie chyba dobrze, ale trzeba to przyspieszyc i lepszy kod dac
// Test: (1212121212121212 * 19) lub (99999999 + 1) lub (12131212121212132 * 19)
			unsigned int t1 = outp->tab[i-1] + 0x06666666;
			unsigned int t2 = t1 + 0x1;
			unsigned int t3 = t1 ^ 0x1;
			unsigned int t4 = t2 ^ t3;
			unsigned int t5 = (~t4) & 0x11111110;
			unsigned int t6 = (t5>>2) | (t5>>3);
			outp->tab[i-1] = t2-t6;

			outp->tab[i] &= 0x0FFFFFFF;
		}
	}
	if ( (outp->tab[0] & 0xF0000000) == 0x10000000 ) // Dla najstarszego wyrównanie
	{
		outp->tab[0] &= 0x0FFFFFFF;
		outp = BCDallocTable( outp, NUMOFCHARS(outp)+1 );
		outp->tab[0]++;
	}
	
	return outp;
}

/*
 * Je¿eli outp == NULL za³o¿enie i ponowne utworzenie.
 * W przeciwnym razie nadpisanie.
 */
BCD* BCDdouble( const BCD *a, BCD *outp )
{
	unsigned int i;
	unsigned char isMinus;

	outp = BCDcopy( a, outp );
	if ( (outp->tab[0] & 0xF0000000) == 0xF0000000 )
	{
		isMinus = 1;
		BCDtencomp( outp );
	}
	else
		isMinus = 0;

	for ( i = outp->size-1; i != (unsigned int)-1; i-- )
	{
		unsigned int t1 = outp->tab[i] + 0x03333333;
		unsigned int t2 = t1 & 0x08888888;
		unsigned int t3 = t2 >> 2;
		unsigned int t4 = t2 - t3;
		unsigned int t5 = outp->tab[i] << 1;
		outp->tab[i] = t4 + t5;
	}
	// Przeniesienia
	for ( i = 1; i < outp->size; i++ ) // Bez najstarszego (bo zmiana rozmiaru)
	{
		if ( (outp->tab[i] & 0xF0000000) == 0x10000000 )
		{
			outp->tab[i] &= 0x0FFFFFFF;
			outp->tab[i-1]++;
		}
	}
	if ( (outp->tab[0] & 0xF0000000) == 0x10000000 ) // Dla najstarszego wyrównanie
	{
		outp->tab[0] &= 0x0FFFFFFF;
		outp = BCDallocTable( outp, NUMOFCHARS(outp)+1 );
		outp->tab[0]++;
	}
	if ( isMinus )
		BCDtencomp( outp );
	return outp;
}

void BCDDirectPrint( const BCD *a )
{
	int i;
	printf("[");
	for (i=0; i<a->size; i++ )
	{
		printf("%.8X ", a->tab[i]);
	}
	printf("]\n");
}

// wymno¿enie warto¶ci w a(BCD) przez sta³± mul i wpisanie wyniku w outp
// Algorytm:
// 1. tmp(BCD) = a(BCD)
// 2. outp(BCD) = 0
// 3. je¶li mul = 0 to koniec
// 4. je¶li mul & 1 jest ró¿ne od zera to outp(BCD) = outp(BCD) + tmp(BCD)
// 5. mul = mul >> 1
// 6. je¶li mul <> 0 to tmp(BCD) = tmp(BCD) * 2
// 7. skok do 3
// Je¿eli outp == NULL to za³o¿enie
BCD *BCDmulInt( const BCD *a, unsigned int mul, BCD *outp )
{
	BCD *tmp = NULL;
	BCD *store = NULL;
	store = BCDalloc( 0 );
	// 1.
	tmp = BCDcopy( a, tmp );
	// 2.
	if ( !outp )
		outp = BCDalloc( 0 );
	else
		outp = BCDcopy( store, outp );
	// 3.
	while ( mul )
	{
		// 4.
		if ( mul & 0x1 )
		{
			store = BCDadd( outp, tmp, store );
			outp = BCDcopy( store, outp );
		}
		// 5.
		mul >>= 1;
		// 6.
		if ( mul )
		{
			store = BCDdouble( tmp, store );
			tmp = BCDcopy( store, tmp );
		}
		// 7.
	}
	BCDfree( tmp );
	BCDfree( store );
	return outp;
}

// UWAGA !!! Poni¿ej jest stworzony SHIFT, aby móc wykorzystywaæ go do
// operacji mno¿enia dwóch liczb BCD, wed³ug tego samego algorytmu co
// mno¿enie dla sta³ej liczbowej
/*
 * Wykonuje przesuniêcie w prawo o zadan± liczbê miejsc. Przesuniêcie
 * jest bitowe !
 */
BCD *BCDshiftRight( const BCD *a, unsigned int shift, BCD *outp )
{
}

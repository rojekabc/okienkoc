#include "global-inc.h"
#include "nbitpola.h"
#ifndef _DEBUG
#	include <string.h>
#endif
#include "mystr.h"

// 18.04.2007 Zamieniam operacje /8 na >>3 oraz %8 na &0x07
// 12.08.2008 Dodano GOC_StFunctionSet i wykonano rozdzial
// Nie nastepuje sprawdzanie wyjscia poza dostepny obszar - to dla uzytkownika

// w zaleznosci od liczby bitow na wartosc moze byc uzyta inna func,
// dzieki czemy moze byc uzyskana poprawa szybkosci dzialania w niektorych
// przypadkach

typedef struct GOC_StFunctionSet
{
	GOC_StNBitField *(*allocBP)(GOC_StNBitField *data);
	GOC_StNBitField *(*freeBP)(GOC_StNBitField *data);
	unsigned int (*get)(GOC_StNBitField *data, unsigned int pos);
	GOC_StNBitField *(*set)(GOC_StNBitField *data, unsigned int pos, unsigned int value);
	GOC_StNBitField *(*add)(GOC_StNBitField *data, unsigned int value);
	GOC_StNBitField *(*rem)(GOC_StNBitField *data, unsigned int pos);
	GOC_StNBitField *(*ins)(GOC_StNBitField *data, unsigned int pos, unsigned int value);
	// zwroc rozmiar w bajtach zajmowany przez dane
	unsigned int (*size)(GOC_StNBitField *data);
} GOC_StFunctionSet;


// Any Bits
static GOC_StNBitField *allocAnyBits(GOC_StNBitField *data)
{
	if ( data->ld )
	{
		data->dane = (unsigned char *)malloc( ((data->lbnw*data->ld-1)>>3)+1 );
		memset(data->dane, 0, ((data->lbnw*data->ld-1)>>3)+1);
	}
	return data;
}

static GOC_StNBitField *freeAnyBits(GOC_StNBitField *data)
{
	if ( data->dane )
	{
		free( data->dane );
		data->dane = NULL;
	}
	return data;
}
static unsigned int getAnyBits(GOC_StNBitField *data, unsigned int pos)
{
	register unsigned int bits, bite, i;
	register unsigned int a;
//	printf("Daj\n");
	bits = bite = data->lbnw * pos;
	bite += data->lbnw;
	bite--;
	a ^= a;
	for (i=bite; i>=bits; i--)
	{
		a <<= 1;
//		printf("%d %d %d\n", pole->ld, i>>3, i&0x03);
		a |= (data->dane[i>>3]>>(i&0x07)) & 1;
		if ( i == bits )
			break;
	}
	return a;
}

static GOC_StNBitField *setAnyBits(GOC_StNBitField *data, unsigned int pos, unsigned int value)
{
	register unsigned int bits, bite, i;
//	printf("Ustaw\n");
	bits = bite = data->lbnw * pos;
	bite += data->lbnw;
	for (i=bits; i<bite; i++)
	{
		if (value & 1)
			data->dane[i>>3] |= (1 << (i&0x07));
		else
			data->dane[i>>3] &= ~(1 << (i&0x07));
		value >>= 1;
	}
	return data;
}

static GOC_StNBitField *addAnyBits(GOC_StNBitField *data, unsigned int value)
{
	if ( data->ld )
	{
		// sprawdz pojemnosc
//		printf("Dodaj - poszerz\n");
		if ((data->lbnw*data->ld-1)>>3 < (data->lbnw*(data->ld+1)-1)>>3)
		{
			data->dane = (unsigned char *)realloc(
				data->dane,((data->lbnw*(data->ld+1)-1)>>3)+1);
		}
	}
	else
	{
//		printf("Dodaj - utworz\n");
		data->dane = (unsigned char *)malloc( ((data->lbnw-1)>>3)+1 );
		memset( data->dane, 0, ((data->lbnw-1)>>3)+1 );
	}
//	printf("Dodaj - ustaw\n");
	return setAnyBits(data, data->ld, value);
}

static GOC_StNBitField *remAnyBits(GOC_StNBitField *data, unsigned int pos)
{
	unsigned int tmp;
	// przesuniecie danych o jedno miejsce od miejsca wyciecia
	for ( pos += 1; pos < data->ld; pos++ )
	{
		tmp = getAnyBits(data, pos);
		setAnyBits(data, pos-1, tmp);
	}
	// czy ulegnie zmianie przechowywana objetosc danych
	if ( (data->lbnw*(data->ld-1)-1)>>3 < (data->lbnw*data->ld-1)>>3 )
	{
		data->dane = (unsigned char *)realloc(
			data->dane, ((data->lbnw*(data->ld-1)-1)>>3)+1 );
	}
	return data;
}

static GOC_StNBitField *insAnyBits(GOC_StNBitField *data, unsigned int pos, unsigned int value)
{
	unsigned int tmp;
	
	// sprawdz pojemnosc
	if ( (data->lbnw*data->ld-1)>>3 < (data->lbnw*(data->ld+1)-1)>>3 )
	{
		// reallokacja zasobow
		data->dane = (unsigned char *)realloc(
			data->dane, ((data->lbnw*(data->ld+1)-1)>>3)+1 );
	}
	// przesuniecie tylow w stosunku do pozycji, w ktorej bedzie
	// nastepowalo umiejscowienie
	for ( ; pos < data->ld; pos++ )
	{
		tmp = getAnyBits(data, pos);
		setAnyBits(data, pos, value);
		value = tmp;
	}
	return setAnyBits(data, pos, value);
}

static unsigned int sizeAnyBits(GOC_StNBitField *data)
{
	return ((data->lbnw * data->ld - 1)>>3)+1;
}

static GOC_StFunctionSet anyBits =
{
	&allocAnyBits,
	&freeAnyBits,
	&getAnyBits,
	&setAnyBits,
	&addAnyBits,
	&remAnyBits,
	&insAnyBits,
	&sizeAnyBits
};


// One bit
static GOC_StNBitField *allocOneBits(GOC_StNBitField *data)
{
	if ( data->ld )
	{
		data->dane = (unsigned char *)malloc( ((data->ld-1)>>3)+1 );
		memset(data->dane, 0, ((data->ld-1)>>3)+1);
	}
	return data;
}

static unsigned int getOneBits(GOC_StNBitField *data, unsigned int pos)
{
	return (data->dane[pos>>3]>>(pos&0x07)) & 1;
}

static GOC_StNBitField *setOneBits(GOC_StNBitField *data, unsigned int pos, unsigned int value)
{
	if (value & 1)
		data->dane[pos>>3] |= (1 << (pos&0x07));
	else
		data->dane[pos>>3] &= ~(1 << (pos&0x07));
	return data;
}

static GOC_StNBitField *addOneBits(GOC_StNBitField *data, unsigned int value)
{
	if ( data->ld )
	{
		// sprawdz pojemnosc
//		printf("Dodaj - poszerz\n");
		if ((data->ld-1)>>3 < (data->ld)>>3)
		{
			data->dane = (unsigned char *)realloc(
				data->dane,((data->ld)>>3)+1);
		}
	}
	else
	{
//		printf("Dodaj - utworz\n");
		data->dane = (unsigned char *)malloc( 1 );
		memset( data->dane, 0, 1 );
	}
//	printf("Dodaj - ustaw\n");
	return setOneBits(data, data->ld, value);
}

static GOC_StNBitField *remOneBits(GOC_StNBitField *data, unsigned int pos)
{
	unsigned int tmp;
	// przesuniecie danych o jedno miejsce od miejsca wyciecia
	for ( pos += 1; pos < data->ld; pos++ )
	{
		tmp = getOneBits(data, pos);
		setOneBits(data, pos-1, tmp);
	}
	// czy ulegnie zmianie przechowywana objetosc danych
	if ( (data->ld-2)>>3 < (data->ld-1)>>3 )
	{
		data->dane = (unsigned char *)realloc(
			data->dane, ((data->ld-2)>>3)+1 );
	}
	return data;
}

static GOC_StNBitField *insOneBits(GOC_StNBitField *data, unsigned int pos, unsigned int value)
{
	unsigned int tmp;
	
	// sprawdz pojemnosc
	if ( (data->ld-1)>>3 < (data->ld)>>3 )
	{
		// reallokacja zasobow
		data->dane = (unsigned char *)realloc(
			data->dane, (data->ld>>3)+1 );
	}
	// przesuniecie tylow w stosunku do pozycji, w ktorej bedzie
	// nastepowalo umiejscowienie
	for ( ; pos < data->ld; pos++ )
	{
		tmp = getOneBits(data, pos);
		setOneBits(data, pos, value);
		value = tmp;
	}
	return setOneBits(data, pos, value);
}
static GOC_StFunctionSet oneBits =
{
	&allocOneBits,
	&freeAnyBits,
	&getOneBits,
	&setOneBits,
	&addOneBits,
	&remOneBits,
	&insOneBits,
	&sizeAnyBits
};

// One byte
static GOC_StNBitField *allocOneByte(GOC_StNBitField *data)
{
	if ( data->ld )
	{
		data->dane = (unsigned char *)malloc(data->ld);
		memset(data->dane, 0, data->ld);
	}
	return data;
}

static unsigned int getOneByte(GOC_StNBitField *data, unsigned int pos)
{
	return data->dane[pos];
}

static GOC_StNBitField *setOneByte(GOC_StNBitField *data, unsigned int pos, unsigned int value)
{
	data->dane[pos] = value & 0xFF;
	return data;
}

static GOC_StNBitField *addOneByte(GOC_StNBitField *data, unsigned int value)
{
	if ( data->ld )
	{
		data->dane = (unsigned char *)realloc(data->dane, data->ld+1);
	}
	else
	{
		data->dane = (unsigned char *)malloc(1);
		memset( data->dane, 0, 1 );
	}
	return setOneByte(data, data->ld, value);
}

static GOC_StNBitField *remOneByte(GOC_StNBitField *data, unsigned int pos)
{
	memmove(data->dane+pos, data->dane+pos+1, data->ld-pos-1);
	data->dane = realloc( data->dane, data->ld-1 );
	return data;
}

static GOC_StNBitField *insOneByte(GOC_StNBitField *data, unsigned int pos, unsigned int value)
{
	data->dane = realloc(data->dane, data->ld+1);
	memmove(data->dane + pos + 1, data->dane + pos, data->ld - pos);
	data->dane[pos] = value;
	return data;
}

static unsigned int sizeOneByte(GOC_StNBitField *data)
{
	return data->ld;
}

static GOC_StFunctionSet oneByte =
{
	&allocOneByte,
	&freeAnyBits,
	&getOneByte,
	&setOneByte,
	&addOneByte,
	&remOneByte,
	&insOneByte,
	&sizeOneByte
};

GOC_StNBitField *goc_nbitFieldCreate(unsigned short lbnw, unsigned int ld)
{
	GOC_StNBitField *tmp = NULL;
	GOC_StFunctionSet *fun = NULL;

	tmp = (GOC_StNBitField*)malloc(sizeof(GOC_StNBitField));
	memset( tmp, 0, sizeof(GOC_StNBitField));
	tmp->lbnw = lbnw;
	tmp->ld = ld;

	switch ( tmp->ld )
	{
		case 1:
			tmp->handler = fun = &oneBits; break;
		case 8:
			tmp->handler = fun = &oneByte; break;
		default:
			tmp->handler = fun = &anyBits; break;
	}

	return fun->allocBP( tmp );
}

void goc_nbitFieldClear(GOC_StNBitField *pole)
{
	GOC_StFunctionSet *fun = NULL;
	if ( !pole )
		return;

	fun = pole->handler;
	fun->freeBP( pole );
	pole->ld = 0;
}

GOC_StNBitField *goc_nbitFieldRemove(GOC_StNBitField *pole)
{
	if ( pole )
	{
		goc_nbitFieldClear(pole);
		free(pole);
	}
	return NULL;
}

unsigned int goc_nbitFieldGet(GOC_StNBitField *pole, unsigned int n)
{
	GOC_StFunctionSet *fun = NULL;
	fun = pole->handler;
	return fun->get(pole, n);
}

void goc_nbitFieldSet(GOC_StNBitField *pole, unsigned int numer,
	register unsigned int wartosc)
{
	GOC_StFunctionSet *fun = NULL;
	fun = pole->handler;
	fun->set(pole, numer, wartosc);
	return;
}

void goc_nbitFieldAdd(GOC_StNBitField *pole, unsigned int wartosc)
{
	GOC_StFunctionSet *fun = NULL;
	fun = pole->handler;
	fun->add(pole, wartosc);
	pole->ld++;
	return;
}

void goc_nbitFieldInsert(GOC_StNBitField *pole, unsigned int pos, unsigned int wartosc)
{
	GOC_StFunctionSet *fun = NULL;
	fun = pole->handler;
	if ( pole->ld != pos )
		fun->ins(pole, pos, wartosc);
	else
		fun->add(pole, wartosc);
	pole->ld++;
	return;
}

void goc_nbitFieldCut(GOC_StNBitField *pole, unsigned int pos)
{
	GOC_StFunctionSet *fun = NULL;
	fun = pole->handler;
	fun->rem(pole, pos);
	pole->ld--;
	return;

}

unsigned int goc_nbitFieldByteSize(GOC_StNBitField *pole)
{
	GOC_StFunctionSet *fun = NULL;
	if ( pole )
	{
		fun = pole->handler;
		return fun->size(pole);
	}
	return 0;
}

char *goc_nbitFieldToString(GOC_StNBitField *pole)
{
	const char *presentationChar = "0123456789ABCDEF";
	char *str = NULL;
	if ( (!pole) || (!pole->dane) )
	{
		str = goc_stringAdd(str, "<NULL>");
		return str;
	}
	if ( pole->lbnw <= strlen(presentationChar) )
	{
		int i;
		str = malloc(pole->ld+1);
		memset(str, 0, pole->ld+1);
		for ( i=0; i<pole->ld; i++ )
			str[i] = presentationChar[goc_nbitFieldGet(pole, i)];
	}
	else
	{
		int i;
		unsigned char v;
		str = malloc(pole->ld*2+1);
		memset(str, 0, pole->ld*2+1);
		for ( i=0; i<pole->ld; i++ )
		{
			v = goc_nbitFieldGet(pole, i);
			str[i] = v;
			str[i] &= 0xF0;
			str[i] >>= 4;
			str[i] = presentationChar[(int)str[i]];
			str[i<<1] = v;
			str[i<<1] &= 0x0F;
			str[i<<1] >>= 4;
			str[i<<1] = presentationChar[(int)str[i<<1]];
		}
	}
	return str;
}

void goc_nbitFieldFromString(GOC_StNBitField *pole, const char *str)
{
	const char *presentationChar = "0123456789ABCDEF";
	if ( pole == NULL )
		return;
	if ( pole->dane )
		free( pole->dane );
	pole->ld = 0;
	if ( str == NULL )
		return;
	if ( string_equals(str, "<NULL>") )
	{
		pole->dane = NULL;
		pole->ld = 0;
	}
	if ( pole->lbnw <= strlen(presentationChar) )
	{
		const char *pos;
		while ( *str )
		{
			pos = strchr( presentationChar, *str );
			if ( !pos )
				return;
			goc_nbitFieldAdd( pole, pos-presentationChar );
			str++;
		}
	}
	else
	{
		int i;
		unsigned char c;
		pole->ld = strlen(str);
		pole->ld >>= 1;
		pole->dane = (unsigned char *)malloc(pole->ld);
		for (i=0; i<pole->ld; i++)
		{
			c = str[i<<1];
			// zamie� znak 0-F na warto�� liczbow�
			if ( c & 0x40 )
			{
				c++;
				c |= 0x80;
			}
			c &= 0x0F;

			c <<= 4;
			pole->dane[i] = c;

			c = str[(i<<1)+1];
			// zamie� znak 0-F na warto�� liczbow�
			if ( c & 0x40 )
			{
				c++;
				c |= 0x80;
			}
			c &= 0x0F;
			pole->dane[i] |= c;
		}
	}
}

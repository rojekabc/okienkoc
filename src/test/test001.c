#include <okienkoc/nbitpola.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

#define _error_( x ) printf("GOC_ERROR: %s\n", x);
#define _lint_(x, y) printf("  log %s = %d\n", x, y);
#define _luint_(x, y) printf("  log %s = %u\n", x, y);

unsigned int timertick;

#define starttimer( _int_tickcounter_, _sigalarmfunction_ ) \
{ \
	struct itimerval tv; \
	struct sigaction sa; \
	memset( &sa, 0, sizeof( struct sigaction ) ); \
	sa.sa_handler=&_sigalarmfunction_; \
	sigaction(SIGALRM, &sa, 0); \
	memset( &tv, 0, sizeof( struct itimerval ) ); \
	tv.it_value.tv_usec=100; \
	tv.it_interval.tv_usec=10000; \
	_int_tickcounter_=0; \
	setitimer(ITIMER_REAL, &tv, 0); \
}

#define stoptimer() \
{ \
	setitimer(ITIMER_REAL, 0, 0); \
}

#define testspeed(_pchar_description_, _int_tickcounter_, _fun_sigalarm_, _int_loopcnt_, block_tested) \
{ \
	int i = 0; \
	printf(_pchar_description_); \
	printf(" ... "); \
	starttimer(_int_tickcounter_, _fun_sigalarm_); \
	for ( ; i<_int_loopcnt_; i++ ) \
		block_tested \
	stoptimer(); \
	printf(" %d ticks\n", _int_tickcounter_); \
}

void sigtimeralarm(int a)
{
	timertick++;
}

void testUstawianiaPobierania1(unsigned int lbnw, unsigned int ld)
{
	unsigned int i, j;
	unsigned int max;
	unsigned int tmp;

	GOC_StNBitField *p = NULL;

	p = goc_nbitFieldCreate(lbnw, ld);
	max = 0;
	for ( i = 0; i < lbnw; i++ )
	{
		max <<= 1;
		max |= 1;
	}
	for ( i = 0; i < ld; i++ )
	{
		for ( j = 0; j <= max; j++ )
		{
//			printf("Ustawiam pole %d na %d\n", i, j);
			goc_nbitFieldSet(p, i, j);
			tmp = goc_nbitFieldGet(p, i);
			if ( tmp != j )
			{
				_error_("Pola j oraz tmp nie sa rowne");
				_luint_("i", i);
				_luint_("j", j);
				_luint_("tmp", tmp);
			}
		}
	}
	p = goc_nbitFieldRemove(p);
}

void testUstawianiaPobierania2(unsigned int lbnw, unsigned int ld)
{
	unsigned int i, j;
	unsigned int max;
	unsigned int tmp;

	GOC_StNBitField *p = NULL;

	
	p = goc_nbitFieldCreate(lbnw, ld);
	max = 0;
	for ( i = 0; i < lbnw; i++ )
	{
		max <<= 1;
		max |= 1;
	}
	for ( i = 0, j=0; i < ld; i++ )
	{
//		printf("Ustawiam pole %d na %d\n", i, j);
		goc_nbitFieldSet(p, i, j);
		j++;
		if ( j > max )
			j = 0;
	}
	for ( i = 0, j=0; i < ld; i++ )
	{
		tmp = goc_nbitFieldGet(p, i);
		if ( tmp != j )
		{
			_error_("Pola j oraz tmp nie sa rowne");
			_luint_("i", i);
			_luint_("j", j);
			_luint_("tmp", tmp);
		}
		j++;
		if ( j > max )
			j = 0;
	}
	p = goc_nbitFieldRemove(p);
}

void testDodawania1(unsigned int lbnw, unsigned int ldToAdd)
{
	unsigned int max;
	unsigned int i, j;
	unsigned int tmp;
	GOC_StNBitField *p = NULL;

	p = goc_nbitFieldCreate(lbnw, 0);
	max = 0;
	for ( i = 0; i < lbnw; i++ )
	{
		max <<= 1;
		max |= 1;
	}
	for ( i = 0, j = 0; i < ldToAdd; i++ )
	{
		goc_nbitFieldAdd(p, j);
		j++;
		if ( j > max )
			j = 0;
	}
	for ( i = 0, j=0; i < p->ld; i++ )
	{
		tmp = goc_nbitFieldGet(p, i);
		if ( tmp != j )
		{
			_error_("Pola j oraz tmp nie sa rowne");
			_luint_("i", i);
			_luint_("j", j);
			_luint_("tmp", tmp);
		}
		j++;
		if ( j > max )
			j = 0;
	}
	p = goc_nbitFieldRemove(p);
}

void testWstawiania1(
	unsigned int lbnw, unsigned int ld,
	unsigned int posIns, unsigned int ldToIns)
{
	unsigned int max;
	unsigned int tmp;
	unsigned int i, j, k;
	GOC_StNBitField *p = NULL;

	p = goc_nbitFieldCreate(lbnw, 0);
	max = 0;
	for ( i = 0; i < lbnw; i++ )
	{
		max <<= 1;
		max |= 1;
	}
	for ( i = 0, j = 0; i < ld; i++ )
	{
		goc_nbitFieldAdd(p, j);
		j++;
		if ( j > max )
			j = 0;
	}
	for ( i = 0, k = 0; i < ldToIns; i++ )
	{
		goc_nbitFieldInsert(p, posIns + i, k );
		k++;
		if ( k > max )
			k = 0;
	}
	// sprawdzenie
	//   dane przed wstawieniem
	for ( i = 0, j = 0; i < posIns; i++ )
	{
		tmp = goc_nbitFieldGet(p, i);
		if ( tmp != j )
		{
			_error_("Pola j oraz tmp nie sa rowne");
			_luint_("i", i);
			_luint_("j", j);
			_luint_("tmp", tmp);
		}
		j++;
		if ( j > max )
			j = 0;
	}
	//   dane wstawione
	for ( k = 0; i < posIns + ldToIns; i++ )
	{
		tmp = goc_nbitFieldGet(p, i);
		if ( tmp != k )
		{
			_error_("Pola k oraz tmp nie sa rowne");
			_luint_("i", i);
			_luint_("k", k);
			_luint_("tmp", tmp);
		}
		k++;
		if ( k > max )
			k = 0;
	}
	//   dane po wstawionych
	for ( ; i < ld + ldToIns; i++ )
	{
		tmp = goc_nbitFieldGet(p, i);
		if ( tmp != j )
		{
			_error_("Pola j oraz tmp nie sa rowne");
			_luint_("i", i);
			_luint_("j", j);
			_luint_("tmp", tmp);
		}
		j++;
		if ( j > max )
			j = 0;
	}
	p = goc_nbitFieldRemove(p);
}

int main()
{
	testspeed(
		"Test numer 1: Ustawiania i pobieranie No.1",
		timertick, sigtimeralarm, 1,
		{
	testUstawianiaPobierania1(3, 10);
		}
	);
	testUstawianiaPobierania1(3, 10);
	testspeed(
		"Test numer 2: Ustawiania i pobieranie No.1",
		timertick, sigtimeralarm, 1,
		{
	testUstawianiaPobierania1(1, 100);
		}
	);
	testspeed(
		"Test numer 3: Ustawiania i pobieranie No.1",
		timertick, sigtimeralarm, 1,
		{
	testUstawianiaPobierania1(9, 10);
		}
	);
	testspeed(
		"Test numer 4: Ustawiania i pobieranie No.1",
		timertick, sigtimeralarm, 1,
		{
	testUstawianiaPobierania1(7, 50);
		}
	);
	testspeed(
		"Test numer 5: Ustawiania i pobieranie No.2",
		timertick, sigtimeralarm, 1,
		{
	testUstawianiaPobierania2(3, 10);
		}
	);
	testspeed(
		"Test numer 6: Ustawiania i pobieranie No.2",
		timertick, sigtimeralarm, 1,
		{
	testUstawianiaPobierania2(1, 100);
		}
	);
	testspeed(
		"Test numer 7: Ustawiania i pobieranie No.2",
		timertick, sigtimeralarm, 1,
		{
	testUstawianiaPobierania2(9, 10);
		}
	);
	testspeed(
		"Test numer 8: Ustawiania i pobieranie No.2",
		timertick, sigtimeralarm, 1,
		{
	testUstawianiaPobierania2(7, 50);
		}
	);
	testspeed(
		"Test numer 9: Ustawiania i pobieranie No.2",
		timertick, sigtimeralarm, 1,
		{
	testUstawianiaPobierania2(32, 1000);
		}
	);
	testspeed(
		"Test numer 10: Dodawanie No.1",
		timertick, sigtimeralarm, 1,
		{
	testDodawania1(1, 20);
		}
	);
	testspeed(
		"Test numer 11: Dodawanie No.1",
		timertick, sigtimeralarm, 1,
		{
	testDodawania1(2, 20);
		}
	);
	testspeed(
		"Test numer 12: Dodawanie No.1",
		timertick, sigtimeralarm, 1,
		{
	testDodawania1(7, 100);
		}
	);
	testspeed(
		"Test numer 13: Dodawanie No.1",
		timertick, sigtimeralarm, 1,
		{
	testDodawania1(9, 100);
		}
	);
	testspeed(
		"Test numer 14: Dodawanie No.1",
		timertick, sigtimeralarm, 1,
		{
	testDodawania1(32, 100);
		}
	);
	testspeed(
		"Test numer 15: Wstawianie No.1",
		timertick, sigtimeralarm, 1,
		{
	testWstawiania1(1, 10, 5, 20);
		}
	);
	testspeed(
		"Test numer 16: Wstawianie No.1",
		timertick, sigtimeralarm, 1,
		{
	testWstawiania1(4, 10, 5, 20);
		}
	);
	testspeed(
		"Test numer 17: Wstawianie No.1",
		timertick, sigtimeralarm, 1,
		{
	testWstawiania1(7, 10, 5, 20);
		}
	);
	testspeed(
		"Test numer 18: Wstawianie No.1",
		timertick, sigtimeralarm, 1,
		{
	testWstawiania1(9, 10, 5, 20);
		}
	);
	testspeed(
		"Test numer 19: Wstawianie No.1",
		timertick, sigtimeralarm, 1,
		{
	testWstawiania1(9, 100, 5, 200);
		}
	);
	testspeed(
		"Test numer 20: Wstawianie No.1",
		timertick, sigtimeralarm, 1,
		{
	testWstawiania1(9, 1000, 5, 2000);
		}
	);
	return 0;
}

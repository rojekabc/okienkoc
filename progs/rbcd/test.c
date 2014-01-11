#include <stdio.h>
#include <string.h>
#include "bcd.h"

int main()
{
	BCD *p = NULL;
	BCD *r = NULL;
	BCD *q = NULL;
	BCD *t = NULL;
	char buf[50];
	unsigned int l;
	int i;
	
	memset( buf, 0, 50 );
	fgets( buf, 50, stdin );
	buf[strlen(buf)-1]=0;
	printf("Input string: [%s]\n", buf);
	p = BCDfromAsc(buf, NULL);
	memset( buf, 0, 50 );
	BCDtoAsc( p, buf, &l );
	t = BCDcopy( p, t );
	printf("P = [%s], Length = [%d]\n", buf, l );
	printf("Valid checking P: [%s]\n", BCDvalid( p ) ? "true" : "false" );
	printf("Compare P to P: [%s]\n", BCDcompare( p, p ) ? "not equal" : "equal" );
	r = BCDcopy( p, r );
	memset( buf, 0, 50 );
	BCDtoAsc( r, buf, &l );
	printf("After copying R = [%s] Length: [%d]\n", buf, l );
	q = BCDadd( p, r, q );
	memset( buf, 0, 50 );
	BCDtoAsc( q, buf, &l );
	printf("After adding Q = P+R = [%s] Length: [%d]\n", buf, l );
	
	r = BCDtencomp( r );
	memset( buf, 0, 50 );
	BCDtoAsc( r, buf, &l );
	printf("After tencomp R = [%s] Length: [%d]\n", buf, l );

	q = BCDadd( p, r, q );
	memset( buf, 0, 50 );
	BCDtoAsc( q, buf, &l );
	printf("After adding Q = P+R = [%s] Length: [%d]\n", buf, l );

	r = BCDtencomp( r );
	memset( buf, 0, 50 );
	BCDtoAsc( r, buf, &l );
	printf("After tencomp R = [%s] Length: [%d]\n", buf, l );
	
	p = BCDdouble( r, p );
	memset( buf, 0, 50 );
	BCDtoAsc( p, buf, &l );
	printf("After double P = 2*R = [%s] Length: [%d]\n", buf, l );

	r = BCDtencomp( r );
	memset( buf, 0, 50 );
	BCDtoAsc( r, buf, &l );
	printf("After tencomp R = [%s] Length: [%d]\n", buf, l );
	
	p = BCDdouble( r, p );
	memset( buf, 0, 50 );
	BCDtoAsc( p, buf, &l );
	printf("After double P = 2*R = [%s] Length: [%d]\n", buf, l );

	memset( buf, 0, 50 );
	BCDtoAsc( t, buf, &l );
	printf("T = [%s] Length: [%d]\n", buf, l );

	for ( i = 0; i < 20; i++ )
	{
		p = BCDmulInt( t, i, p );
		memset( buf, 0, 50 );
		BCDtoAsc( p, buf, &l );
		printf("p = T*%d = [%s] Length: [%d]\n", i, buf, l );
	}
	
	p = BCDfromAsc("1", p);
	r = BCDfromAsc("99999999", r);
	t = BCDadd( p, r, t );
	memset( buf, 0, 50 );
	BCDtoAsc( t, buf, &l );
	printf("T = P+R = 1 + 99999999 = [%s] Length: [%d]\n", buf, l );

	p = BCDfromAsc("1", p);
	r = BCDfromAsc("99999999999999", r);
	t = BCDadd( p, r, t );
	memset( buf, 0, 50 );
	BCDtoAsc( t, buf, &l );
	printf("T = P+R = 1 + 99999999999999 = [%s] Length: [%d]\n", buf, l );

	BCDfree( p );
	BCDfree( r );
	BCDfree( q );
	BCDfree( t );
	return 0;
}

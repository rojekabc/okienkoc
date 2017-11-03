#include <tools/tablica.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>

typedef struct StTest
{
	int a;
	GOC_TABLEPTR(char *, name);
	int b;
} StTest;

int main()
{
	int i;
	GOC_TABLEIPTR(char *, name);
	char* x = GOC_TABLEPUTPTR(char *, name, strdup("alfa"));
	GOC_TABLEPUTPTR(char *, name, strdup("beta"));
	GOC_TABLEPUTPTR(char *, name, strdup("gamma"));
	printf("String: %s\n", x);
	i=0;
	while (i<nname)
		printf("String at %d of %d: %s\n", ++i, nname, pname[i]);

	return 0;
}

#include <okienkoc/okienkoc.h>

int main()
{
	GOC_StMessage msg;
	while (goc_systemCheckMsg( &msg ));
	return 0;
}

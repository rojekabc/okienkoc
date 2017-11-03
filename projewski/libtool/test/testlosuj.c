#include <tools/random.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int min, max, count, tmp;
	int *stat;
	if ( argc < 4 )
	{
		printf("Usage: %s min max count\n", argv[0]);
		return 0;
	}
	srand(time(NULL));
	min = atoi(argv[1]);
	max = atoi(argv[2]);
	count = atoi(argv[3]);
	stat = malloc(sizeof(int)*(max-min));
	memset(stat, 0, sizeof(int)*(max-min));
	tmp = count;
	while ( tmp-- )
	{
		int los = goc_random(max);
		if ( los >= max )
			printf("GOC_ERROR: Random over max\n");
		stat[los]++;
	}
	for ( tmp=min; tmp<max; tmp++)
		printf("%04d: %05d\n", tmp, stat[tmp-min]);

	return 0;
}


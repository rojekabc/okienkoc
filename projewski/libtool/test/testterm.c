#include <tools/term.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int i, x;
	printf("Screen size: WIDTH=%d HEIGHT=%d\n", goc_screenGetWidth(), goc_screenGetHeight());
	printf("Terminal type: %s\n", getenv("TERM"));
	return 0;
}

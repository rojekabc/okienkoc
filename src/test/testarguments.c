#include <okienkoc/arguments.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	GOC_Arguments* args = goc_argumentsAlloc();
	char *string = NULL;
	int value = 0;
	unsigned char flag = 0;

	args = goc_argumentsAdd(args,
		"-h", "Print help", goc_argumentHelpFunction, &args);
	args = goc_argumentsAdd(args,
		"--help", "Print help", goc_argumentHelpFunction, &args);
	args = goc_argumentsAdd(args,
		"-?", "Print help", goc_argumentHelpFunction, &args);
	args = goc_argumentsSetUnknownFunction(args, goc_argumentHelpFunction);
	args = goc_argumentsAdd(args,
		"-s", "Set a string", goc_argumentStringFunction, &string);
	args = goc_argumentsAdd(args,
		"-i", "Set an integer", goc_argumentIntFunction, &value);
	args = goc_argumentsAdd(args,
		"-f", "Set a flag", goc_argumentVoidFunction, &flag);
	goc_argumentsParse(args, argc, argv);
	args = goc_argumentsFree(args);
	fprintf(stdout, "Arguments values: string [%s], integer [%d], flag [%s]\n", string ? string : "null", value, flag ? "true" : "false");
	return 0;
}

#include <tools/mystr.h>
#include <stdio.h>

int main()
{
	char *tmp = NULL;
	printf("%d\n", goc_stringEndsWithCase("xxxsd dsd s.MP2", ".mp3"));

	tmp = goc_stringCopy(tmp, "Alfa");
	tmp = goc_stringMultiAdd(tmp, "Beta", "Gamma", "Fi");
	printf("[%s]\n", tmp);

	tmp = goc_stringCopy(tmp, "Alfa");
	tmp = goc_stringMultiAdd(tmp, "Beta");
	printf("[%s]\n", tmp);

	tmp = goc_stringCopy(tmp, "Alfa");
	tmp = goc_stringMultiAdd(tmp, NULL);
	printf("[%s]\n", tmp);

	tmp = goc_stringCopy(tmp, "Alfa");
	tmp = goc_stringMultiAdd(tmp, NULL, "Beta", NULL, "Gamma", NULL);
	printf("[%s]\n", tmp);

	tmp = goc_stringMultiCopy(tmp, "Alfa", "Beta", "Gamma", "Fi");
	printf("[%s]\n", tmp);

	tmp = goc_stringMultiCopy(tmp, "Alfa", "Beta");
	printf("[%s]\n", tmp);

	tmp = goc_stringMultiCopy(tmp, "Alfa");
	printf("[%s]\n", tmp);

	tmp = goc_stringMultiCopy(tmp, "Alfa", NULL, "Beta", NULL, "Gamma");
	printf("[%s]\n", tmp);
}

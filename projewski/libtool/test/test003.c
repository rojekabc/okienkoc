#include <tools/mystr.h>
#include <tools/screen.h>
#include <stdio.h>


#define TEST_TRUE(name, boolean, getTrue, getFalse) \
	printf("%s: ", (name)); \
	if ( boolean ) \
		{ goc_textallcolor(GOC_GREEN | GOC_FBOLD); printf("%s\n", (getTrue)); goc_textallcolor(GOC_WHITE); }\
	else \
		{ goc_textallcolor(GOC_RED | GOC_FBOLD); printf("%s (%s:%d)\n", (getFalse), __FILE__, __LINE__); goc_textallcolor(GOC_WHITE); }

int totalTrue;
int totalFalse;

static char *testTrue()
{
	totalTrue++;
	return "Ok";
}

static char *testFalse()
{
	totalFalse++;
	return "Failed";
}

#define TEST_START()\
	totalTrue = 0; \
	totalFalse = 0; \
	printf("Start testing.\n");

#define TEST_END()\
	printf("End testing.\nResults:  Positive: %03d  Negative: %03d\n", totalTrue, totalFalse);


int main()
{
	char *tmp = NULL;
	TEST_START();
	tmp = goc_stringCopy(tmp, "\t \tCiag znakow\t \t \n\r");
	tmp = goc_stringTrim(tmp);
	TEST_TRUE("Test goc_stringTrim z zestawem do usuniecia z obu stron",
			goc_stringEquals(tmp, "Ciag znakow"),
			testTrue(), testFalse());

	tmp = goc_stringCopy(tmp, "Ciag znakow\t \t \n\r");
	tmp = goc_stringTrim(tmp);
	TEST_TRUE("Test goc_stringTrim z zestawem do usuniecia na koncu",
			goc_stringEquals(tmp, "Ciag znakow"),
			testTrue(), testFalse());

	tmp = goc_stringCopy(tmp, "\t \t\n\rCiag znakow");
	tmp = goc_stringTrim(tmp);
	TEST_TRUE("Test goc_stringTrim z zestawem do usuniecia na poczatku",
			goc_stringEquals(tmp, "Ciag znakow"),
			testTrue(), testFalse());

	tmp = goc_stringCopy(tmp, "Ciag znakow");
	tmp = goc_stringTrim(tmp);
	TEST_TRUE("Test goc_stringTrim bez usuwania",
			goc_stringEquals(tmp, "Ciag znakow"),
			testTrue(), testFalse());

	tmp = goc_stringCopy(tmp, "\tCiag znakow");
	tmp = goc_stringTrim(tmp);
	TEST_TRUE("Test goc_stringTrim z jednym na poczatku do usuniecia",
			goc_stringEquals(tmp, "Ciag znakow"),
			testTrue(), testFalse());

	tmp = goc_stringCopy(tmp, "Ciag znakow\t");
	tmp = goc_stringTrim(tmp);
	TEST_TRUE("Test goc_stringTrim z jednym na koncu do usuniecia",
			goc_stringEquals(tmp, "Ciag znakow"),
			testTrue(), testFalse());

	tmp = goc_stringCopy(tmp, "\t \n\r\t\t\t\t\t     \n\t\n\n\n\r");
	tmp = goc_stringTrim(tmp);
	TEST_TRUE("Test goc_stringTrim z wszystkim do usuniecia",
			goc_stringEquals(tmp, NULL),
			testTrue(), testFalse());

	tmp = goc_stringCopy(tmp, "");
	tmp = goc_stringTrim(tmp);
	TEST_TRUE("Test goc_stringTrim bez niczego",
			goc_stringEquals(tmp, NULL),
			testTrue(), testFalse());

	tmp = goc_stringCopy(tmp, NULL);
	tmp = goc_stringTrim(tmp);
	TEST_TRUE("Test goc_stringTrim z NULL",
			goc_stringEquals(tmp, NULL),
			testTrue(), testFalse());
	tmp = goc_stringFree(tmp);
	TEST_END();
	return 0;
}

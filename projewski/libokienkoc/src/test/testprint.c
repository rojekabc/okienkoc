// Test powinno siê wywo³ywaæ z przekierowaniem strumienia stderr do /dev/null
// Wnioski z testu: ca³kowity absurd wyników dla wyj¶cia /dev/null. Dla wyj¶cia
// stderr rzeczywi¶cie mo¿e mieæ istotny wp³yw podanie ka¿dego kolejnego argumentu
//
// Wnioski z dzia³ania fprintf:
// Bez podania dodatkowych argumentów zapewne jest wywo³ywane samo fputs,
// Po podaniu argumentów zapewne nastêpuje przeszukiwanie za znakami formatowania
// Je¶li s± znaki formatowania kolejne jednostki czasowe poch³ania samo formatowanie
#include <okienkoc/log.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

unsigned int timertick;

#define testspeed(_pchar_description_, _int_tickcounter_, _fun_sigalarm_, _int_loopcnt_, block_tested) \
{ \
	int i = 0; \
	struct itimerval tv; \
	struct sigaction sa; \
	printf(_pchar_description_); \
	printf(" ... "); \
	memset( &sa, 0, sizeof( struct sigaction ) ); \
	sa.sa_handler=&_fun_sigalarm_; \
	sigaction(SIGALRM, &sa, 0); \
	memset( &tv, 0, sizeof( struct itimerval ) ); \
	tv.it_value.tv_usec=100; \
	tv.it_interval.tv_usec=10000; \
	_int_tickcounter_=0; \
	setitimer(ITIMER_REAL, &tv, 0); \
	for ( ; i<_int_loopcnt_; i++ ) \
	{ \
		block_tested; \
	} \
	setitimer(ITIMER_REAL, 0, 0); \
	printf(" %d ticks\n", _int_tickcounter_); \
}

static void sigtimeralarm(int a)
{
	timertick++;
}

static void testPrintf(const char* printTxt, FILE* out)
{
	fprintf(out, printTxt);
}

static void testPrintfArgue(const char* printTxt, FILE* out)
{
	fprintf(out, printTxt, "String Argument", 12345);
}

static void testFputs(const char* printTxt, FILE* out)
{
	fputs(printTxt, out);
}

static void testPrintfEmpty(const char* printTxt, FILE* out)
{
	fprintf(out, printTxt, "String Argument", 12345);
}

int main()
{
	char *bufTxt = "\tThis is test text without any kind of formatting.\n";
	char *bufArgTxt = "\tThis is test text with formatting %s: %d.\n";
	int loopcnt = 10000000;
	FILE *file = fopen("/dev/null", "rb");
	if ( file == NULL )
	{
		GOC_ERROR("Cannot open /dev/null");
		return -1;
	}
	testspeed(
		"Test 1: fprintf without agrs to /dev/null",
		timertick, sigtimeralarm, loopcnt,
		testPrintf(bufTxt, file)
	);
	testspeed(
		"Test 2: fputs to /dev/null",
		timertick, sigtimeralarm, loopcnt,
		testFputs(bufTxt, file)
	);
	testspeed(
		"Test 3: fprintf with args to /dev/null",
		timertick, sigtimeralarm, loopcnt,
		testPrintfArgue(bufArgTxt, file)
	);
	fclose(file);

	file = stderr;
	testspeed(
		"Test 4: fprintf without agrs to stderr",
		timertick, sigtimeralarm, loopcnt,
		testPrintf(bufTxt, file)
	);
	testspeed(
		"Test 5: fputs to stderr",
		timertick, sigtimeralarm, loopcnt,
		testFputs(bufTxt, file)
	);
	testspeed(
		"Test 6: fprintf with args to stderr",
		timertick, sigtimeralarm, loopcnt,
		testPrintfArgue(bufArgTxt, file)
	);
	testspeed(
		"Test 7: fprintf without agrs to stderr (not used args)",
		timertick, sigtimeralarm, loopcnt,
		testPrintfEmpty(bufTxt, file)
	);
	return 0;
}

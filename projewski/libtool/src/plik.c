#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#define __USE_GNU
#include <unistd.h>
#include "global-inc.h"

int goc_isFileExists( const char *nazwa )
{
	struct stat filestat;
	return !stat( nazwa, &filestat ); // Brak zwróci -1
}

int goc_isFolder( const char *nazwa )
{
	struct stat filestat;
	if ( stat( nazwa, &filestat ) )
		return 0; // Nie istnieje
	return S_ISDIR( filestat.st_mode ); // Czy flag katalogu
}

char *goc_getCurrentWorkdir() {
	int size = 256;
	char* result = NULL;
	while (result == NULL) {
		char* buf = malloc(size);
		result = getcwd(buf, size);
		if (result == NULL) {
			if (errno != ERANGE) {
				break;
			}
			free(buf);
			size += 256;
		}
	}
	return result;
}

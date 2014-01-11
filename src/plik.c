#include <unistd.h>
#include <sys/stat.h>
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

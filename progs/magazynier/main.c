// Poniewa¿ SISI jest pe³en bug'ów zwi±zanych z orzganizowaniem pamiêci, pe³nym
// wycieków i nieprzemy¶lanych allokacji, piszê w³asnego shit'a
#include <okienkoc/okienkoc.h>
#include <okienkoc/mystr.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#ifdef _DEBUG
#include <mpatrol.h>
#else
#include <malloc.h>
#endif
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>

#define GOC_PRINTERROR
#include <okienkoc/log.h>

#define _TESTING_

#define PROGRAM_AUTHOR "Piotr Rojewski"
#define PROGRAM_TITLE "Magazynier - wersja 1.2"

#define KONFKLUCZ_FOLDER "Katalog plansz"

#define TRYB_GLOBUS 0
#define TRYB_GLOBAL 1

// globus.cnf
#define GLOBUS_AUTOR "Author"
#define GLOBUS_TYTUL "Title"
#define GLOBUS_MAKS "Level Ammount"

#define ZNAK_PODLOGA 0
#define ZNAK_LUDEK 1
#define ZNAK_LUDEK_MAGAZYN 2
#define ZNAK_BALON 3
#define ZNAK_BALON_MAGAZYN 4
#define ZNAK_SCIANA 5
#define ZNAK_MAGAZYN 6

// default gamedata folder
#define GAME_FOLDER "games"
			
#define piszblad( komunikat, opis ) \
	std::cerr << __FILE__ << ":" << __LINE__ << std::endl \
			<< komunikat << " " << opis << std::endl

// czas rozpoczêcia gry na danej planszy
time_t czas_start;
GOC_HANDLER formaPoziom;
// Struktura magzynujaca ustawienia
typedef struct StKonfPlik
{
	char *name;
	char **pKonf;
	unsigned int nKonf;
} StKonfPlik;
typedef struct StBazowaScena
{
	GOC_HANDLER lista, status, aktokno;
	unsigned int poziom;
	StKonfPlik *konf, *globus;
} StBazowaScena;

// Struktura Do Przechowywania Ostatniego Ruchu
typedef struct StOddo
{
	short x, y;
	unsigned char w;
} StOddo;

typedef struct
{
		GOC_BOOL czyStart;
		unsigned int dpl, wpl;
		unsigned int graczX;
		unsigned int graczY;

		unsigned int minPosow;
		unsigned int minRuch;
		unsigned int liczPosow;
		unsigned int liczRuch;

		GOC_HANDLER forma, gra, status, inform, ruchInfo, pychInfo, levelInfo;
#ifdef _TESTING_
		GOC_HANDLER pustakInfo;
		unsigned int liczPustki;
#endif
		StOddo *cofRuch;
		unsigned int ncofRuch;
} StOknoPlansza;

StBazowaScena *_scena;
StOknoPlansza *_plan;


int planLiczDlugosc(const char **pString, unsigned int nString);
// utworzenie nazwy pliku logowania na podtawie nazwy aktualnego u¿ytkownika
// oraz rozszerzenia log
char *dajRekordPlik(char *pBuf, unsigned int nBuf)
{
	struct passwd *pwd = NULL;
	pwd = getpwuid( getuid() );
	if ( !pwd )
		return NULL; // !!!!
	// nazwa pliku loga
	if ( strlen(pwd->pw_name) + 4 >= nBuf )
		return NULL;
	sprintf(pBuf, "%s.log", pwd->pw_name);
	return pBuf;
}

typedef struct StTabPlik
{
	char *nazwa;
	FILE *plik;
	char *koment;
	char *delim;
	char *buf;
} StTabPlik;

StTabPlik *tabplikKreuj(const char *nazwa, const char *delim, const char *kom)
{
	StTabPlik* tmp = (StTabPlik*)malloc(sizeof(StTabPlik));
	memset(tmp, 0, sizeof(StTabPlik));
	tmp->nazwa = goc_stringCopy(tmp->nazwa, nazwa);
	tmp->koment = goc_stringCopy(tmp->koment, kom);
	tmp->delim = goc_stringCopy(tmp->delim, delim);
	tmp->buf = (char *)malloc(256);
	memset(tmp->buf, 0, 256);
	return tmp;
}

StTabPlik *tabplikUsun(StTabPlik *tp)
{
	if ( tp->nazwa )
		free(tp->nazwa);
	if ( tp->koment )
		free(tp->koment);
	if ( tp->delim )
		free(tp->delim);
	if ( tp->plik )
		fclose(tp->plik);
	if ( tp->buf )
		free(tp->buf);
	free(tp);
	return NULL;
}

int tabplikUstZnakiKomentarza(StTabPlik *tp, const char *koment)
{
	tp->koment = goc_stringCopy(tp->koment, koment);
	return 0;
}

GOC_BOOL tabplikCzytajWiersz(StTabPlik *tp)
{
	if ( !(tp->plik) )
		tp->plik = fopen(tp->nazwa, "r");
	if ( !(tp->plik) )
		return GOC_FALSE;
	while ( fgets(tp->buf, 256, tp->plik) )
	{
		if ( (tp->delim) && (strchr(tp->koment, tp->buf[0])) )
		{
			memset(tp->buf, 0, 256);
			continue;
		}
		{
			char *p = NULL;
			p = strchr(tp->buf, '\n');
			if ( p )
				*p = 0;
			p = strchr(tp->buf, '\r');
			if ( p )
				*p = 0;
			p = tp->buf;
			while ( (p = strpbrk(p, tp->delim)) )
			{
				*p = 0;
				p++;
			}
		}
		return GOC_TRUE;
	}
	return GOC_FALSE;
}

const char *tabplikDajKolumna(StTabPlik *tp, unsigned int n)
{
	const char *ret;
	ret = tp->buf;
	while ( n )
	{
		ret += strlen(ret);
		ret++;
		n--;
	}
	return ret;
}

void tabplikSkokPoczatek(StTabPlik *tp)
{
	if ( tp->plik )
		fclose(tp->plik);
	tp->plik = NULL;
}

// liczba kolumn w aktualnym wierszu
int tabplikDajRozmiar(StTabPlik *tp)
{
	const char *ret;
	int i=0;
	ret = strtok(tp->buf, tp->delim);
	if ( !ret )
		return i;
	while ( ret = strtok(NULL, tp->delim) )
		i++;
	return i;
}

GOC_BOOL tabplikTworzPlik(StTabPlik *tp)
{
	FILE *plik;
	if (tp->plik)
		return GOC_FALSE;
	plik = fopen(tp->nazwa, "w");
	fclose(plik);
	return GOC_TRUE;
}

// dodanie wiersza
GOC_BOOL tabplikDodajWiersz(StTabPlik *tp, const char **tStr, int n, char delim)
{
	int i;
	if ( tp->plik )
		return GOC_FALSE;
	tp->plik = fopen(tp->nazwa, "a");
	if ( !(tp->plik) )
		return GOC_FALSE;
	for ( i=0; i<n; i++ )
		fprintf(tp->plik, "%s%c", tStr[i], delim);
	fprintf(tp->plik, "\n");
	fclose(tp->plik);
	tp->plik = NULL;
	return GOC_TRUE;
}

// dodanie juz sformatowanego wiersz
GOC_BOOL tabplikDodajLinia(StTabPlik *tp, const char *pStr)
{
	if ( tp->plik )
		return GOC_FALSE;
	tp->plik = fopen(tp->nazwa, "a");
	if ( !(tp->plik) )
		return GOC_FALSE;
	fprintf(tp->plik, "%s\n", pStr);
	fclose(tp->plik);
	tp->plik = NULL;
	return GOC_TRUE;
}

void cofRuchDodaj( short x, short y, unsigned char w )
{
	_plan->cofRuch = goc_tableAdd( _plan->cofRuch, &_plan->ncofRuch, sizeof(StOddo) );
	_plan->cofRuch[_plan->ncofRuch-1].x = x;
	_plan->cofRuch[_plan->ncofRuch-1].y = y;
	_plan->cofRuch[_plan->ncofRuch-1].w = w;
}
GOC_BOOL cofRuchCzyJest()
{
	if ( _plan->ncofRuch )
		return GOC_TRUE;
	else
		return GOC_FALSE;
}

void cofRuchCzysc()
{
	_plan->cofRuch = goc_tableClear(_plan->cofRuch, &_plan->ncofRuch);
}

void cofRuchCofnij()
{
	_plan->cofRuch = goc_tableRemove(_plan->cofRuch, &_plan->ncofRuch, sizeof(StOddo), _plan->ncofRuch-1);
}

short cofRuchDajx()
{
	return _plan->cofRuch[_plan->ncofRuch-1].x;
}
short cofRuchDajy()
{
	return _plan->cofRuch[_plan->ncofRuch-1].y;
}
unsigned char cofRuchDajw()
{
	return _plan->cofRuch[_plan->ncofRuch-1].w;
}

const char *konfplikDajParametr( StKonfPlik *kp, const char *klucz )
{
	unsigned int i;
	for ( i=0; i<kp->nKonf; i++, i++ )
	{
		if ( strcmp( kp->pKonf[i], klucz ) == 0 )
			return kp->pKonf[i+1];
	}
	return "";
}

// dodaje i ustawia parametr i zwraca ustawiona wartosc
const char *konfplikUstParametr( StKonfPlik *kp, const char *klucz, const char *wartosc )
{
	char *pBuf;
	kp->pKonf = goc_tableAdd( kp->pKonf, &kp->nKonf, sizeof(char*) );
	kp->pKonf[kp->nKonf-1] = strdup(klucz);
	kp->pKonf = goc_tableAdd( kp->pKonf, &kp->nKonf, sizeof(char*) );
	kp->pKonf[kp->nKonf-1] = strdup(wartosc);
	return pBuf;
}

// zwraca parametr, je¶li nie ustawiony to go te¿ ustawia
const char *konfplikDajUstParametr( StKonfPlik *kp, const char *klucz, const char *wartosc )
{
	unsigned int i;
	for ( i=0; i<kp->nKonf; i++, i++ )
	{
		if ( strcmp( kp->pKonf[i], klucz ) == 0 )
			return kp->pKonf[i+1];
	}
	return konfplikUstParametr( kp, klucz, wartosc );
}

StKonfPlik *konfplikKreuj( const char *nazwa, const char *delim )
{
	StKonfPlik *tmp = (StKonfPlik*)malloc(sizeof(StKonfPlik));
	StTabPlik *tp = tabplikKreuj(nazwa, delim, "#");
	memset(tmp, 0, sizeof(StKonfPlik));
	if ( goc_isFileExists( nazwa ) )
	{
		while ( tabplikCzytajWiersz(tp) )
			konfplikUstParametr( tmp, tabplikDajKolumna(tp, 0), tabplikDajKolumna(tp, 1) );
	}
	else
	{
		int i;
		tabplikTworzPlik(tp);
		konfplikUstParametr( tmp, KONFKLUCZ_FOLDER, "domyslny" );
		for ( i=0; i<tmp->nKonf; i++, i++ )
			tabplikDodajWiersz(tp, (const char**)(tmp->pKonf)+i, 2, ':');
	}
	if ( tp )
		tabplikUsun(tp);
	return tmp;
}

void konfplikUsun(StKonfPlik *kp)
{
	unsigned int i;
	if ( !kp )
		return;
	if ( kp->name )
		free(kp->name);
	for ( i=0; i<kp->nKonf; i++)
		free(kp->pKonf[i]);
	kp->pKonf = goc_tableClear(kp->pKonf, &kp->nKonf );
	free( kp );
}

GOC_HANDLER wprow, ok;

static int okAction() {
	GOC_StEdit *wpis = (GOC_StEdit*)wprow;
	if ( wpis->tekst )
		_scena->poziom = atoi(wpis->tekst);
	goc_formHide(formaPoziom);
	return GOC_ERR_OK;
}

int fpoziomNasluch(GOC_HANDLER u, GOC_StMessage* msg)
{
	if ( (u == wprow) && (msg->id == GOC_MSG_CHAR_ID) )
	{
		GOC_StMsgChar* msgchar = (GOC_StMsgChar*)msg;
		if ( (msgchar->charcode >= '0') && (msgchar->charcode <= '9') ) {
			int maxi = atoi(konfplikDajParametr( _scena->globus, GLOBUS_MAKS ));
			GOC_StEdit *wpis = (GOC_StEdit*)u;
			goc_systemDefaultAction(u, msg);
			if ( atoi(wpis->tekst) > maxi ) {
				GOC_MSG_CHAR( msgsend, 0x7F );
				goc_systemSendMsg(u, msgsend);
			}
			return GOC_ERR_OK;
		} else if ( msgchar->charcode == 13 ) {
			return okAction();
		} else if ( msgchar->charcode >= 0x7F ) {
			return goc_systemDefaultAction(u, msg);
		} else if ( msgchar->charcode < 0x20 ) {
			return goc_systemDefaultAction(u, msg);
		} else {
			return GOC_ERR_OK;
		}
	}
	if ( (u == ok) && (msg->id == GOC_MSG_ACTION_ID) )
	{
		return okAction();
	}
	return goc_systemDefaultAction(u, msg);
}

// Okno wybierania poziomu
void pokazOknoWybieraniaPoziomu()
{
	GOC_HANDLER ramka, napis;
	char *pBuf = NULL;
	/*
	unsigned int maksInt;
	konfplikUstParametr( tmp->globus, GLOBUS_MAKS,
		  	liczPoziomy(konfplikDajParametr(tmp->konf, KONFKLUCZ_FOLDER)) );
	*/

	formaPoziom = goc_elementCreate( GOC_ELEMENT_FORM,
		(goc_screenGetWidth()>>1)-15, 4, 34, 9,
		GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, GOC_HANDLER_SYSTEM );
	
	ramka = goc_elementCreate( GOC_ELEMENT_FRAME, 1, 1, 0, 0, GOC_EFLAGA_PAINTED,
			GOC_WHITE, formaPoziom);

	pBuf = goc_stringAdd( pBuf, konfplikDajUstParametr( _scena->konf, "T6", "Wybierz poziom" ) );
	pBuf = goc_stringAdd( pBuf, " <001; " );
	pBuf = goc_stringAdd( pBuf, konfplikDajParametr( _scena->globus, GLOBUS_MAKS ) );
	pBuf = goc_stringAdd( pBuf, ">");
	
	napis = goc_elementCreate( GOC_ELEMENT_LABEL, 2, 3, -2, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER,
			GOC_GREEN, formaPoziom );
	goc_labelSetText( napis, pBuf, GOC_FALSE );

	wprow = goc_elementCreate( GOC_ELEMENT_EDIT, 10, 5, 16, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER | GOC_EFLAGA_ENABLE,
			GOC_WHITE, formaPoziom );
	sprintf(pBuf, "%d", _scena->poziom );
	goc_editSetText( wprow, pBuf );

	ok = goc_elementCreate( GOC_ELEMENT_BUTTON, 11, 7, 12, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE | GOC_EFLAGA_CENTER,
			GOC_WHITE | GOC_FBOLD, formaPoziom );
	goc_buttonSetText( ok, konfplikDajUstParametr( _scena->konf, "T7", "Akceptuj") );

	goc_elementSetFunc( ok, GOC_FUNID_LISTENER, &fpoziomNasluch );
	goc_elementSetFunc( wprow, GOC_FUNID_LISTENER, &fpoziomNasluch );

	goc_formShow( formaPoziom );
	free( pBuf );
}

void planRobLog()
{
	FILE *plik;
	time_t czas;
	char *tmp = NULL;
	char buf[40];
	char *poz;
			
	time( &czas );
	sprintf( buf, "%d|", _scena->poziom );
	tmp = goc_stringAdd( tmp, buf );

	sprintf( buf, "%d|", _plan->liczRuch );
	tmp = goc_stringAdd( tmp, buf );
	
	sprintf( buf, "%d|", _plan->liczPosow );
	tmp = goc_stringAdd( tmp, buf );
			
	sprintf( buf, "%.1lf|", difftime(czas, czas_start) );
	tmp = goc_stringAdd( tmp, buf );

	sprintf(buf, "%s", ctime( &czas_start ));
	if ( (poz = strchr(buf, '\n')) )
		*poz = 0;
	if ( (poz = strchr(buf, '\r')) )
		*poz = 0;
	tmp = goc_stringAdd( tmp, buf );
	tmp = goc_stringAdd( tmp, "|" );
	
	// loguj typ planszy
	tmp = goc_stringAdd( tmp, konfplikDajParametr( _scena->konf, KONFKLUCZ_FOLDER ) );

	// dodawanie do pliku logow
	plik = fopen( dajRekordPlik(buf, 80), "a");
	fprintf(plik, "%s\n", tmp);
	fclose(plik);
	free(tmp);
}

unsigned int planSzukajNajlepszy( unsigned int lev, unsigned int kol )
{
	unsigned int min = 5000;
	unsigned int tmp;
	unsigned int rpoz;
	char pBuf[256];
	StTabPlik *tp;
			
	if ( !goc_isFileExists( dajRekordPlik(pBuf, 256) ) )
		return min;

	tp = tabplikKreuj( pBuf, "|", "#" );

	while ( tabplikCzytajWiersz(tp) )
	{
		tmp = atoi( tabplikDajKolumna( tp, kol ) );
		rpoz = atoi( tabplikDajKolumna( tp, 0 ) );
		if ( (rpoz == lev) && (tmp < min)
			  	&&	( strcmp(
						konfplikDajParametr(_scena->konf, KONFKLUCZ_FOLDER),
					  	tabplikDajKolumna( tp, 5 )
						) == 0
				  	)
		  	)
					min = tmp;
	}

	tabplikUsun( tp );
	return min;
}

void budujNapisInt(GOC_HANDLER napis, const char *opis, int value, int min)
{
	char pBuf[80];
	memset(pBuf, 0, 80);
	sprintf(pBuf, "%s: %04d/%04d", opis, value, min);
	goc_labelSetText(napis, pBuf, GOC_FALSE);
}

void planBuduj(char **pDane, int nDane)
{
	const char *poz;
	unsigned int i, j;
	char *tmp = NULL;

	for ( i=0; i<nDane; i++ )
	{
		poz = strstr( pDane[i], GLOBUS_TYTUL );
		if ( poz )
		{
			poz = strchr( pDane[i], ':' );
			if ( !poz )
				continue;
			konfplikUstParametr( _scena->globus, GLOBUS_TYTUL, poz+1);
			continue;
		}
		for ( j=0; j<strlen(pDane[i]); j++ )
		{
			switch ( pDane[i][j] )
			{
				case ' ':
					break;
				case '@':
					goc_maskSet( _plan->gra, (_plan->graczX = j), (_plan->graczY = i), ZNAK_LUDEK );
					break;
				case '+':
					goc_maskSet( _plan->gra, (_plan->graczX = j), (_plan->graczY = i), ZNAK_LUDEK_MAGAZYN );
					break;
				case 'o':
					goc_maskSet( _plan->gra, j, i, ZNAK_BALON );
					break;
				case 'O':
					goc_maskSet( _plan->gra, j, i, ZNAK_BALON_MAGAZYN );
					break;
				case '#':
					goc_maskSet( _plan->gra, j, i, ZNAK_SCIANA );
					break;
				case '.':
					_plan->liczPustki++;
					goc_maskSet( _plan->gra, j, i, ZNAK_MAGAZYN );
					break;
				default:
					GOC_FERROR( goc_stringAddInt( goc_stringCopy(NULL, "Nie rozpoznany znak o kodzie "), pDane[i][j]) );
					break;
			}
		}
	}
	budujNapisInt(_plan->ruchInfo, konfplikDajUstParametr(_scena->konf, "T8", "Ruch"), _plan->liczRuch, _plan->minRuch);
	budujNapisInt(_plan->pychInfo, konfplikDajUstParametr(_scena->konf, "T9", "Pych"), _plan->liczPosow, _plan->minPosow);
	tmp = goc_stringCopy(tmp, "L:");
	tmp = goc_stringAddInt(tmp, _scena->poziom);
	goc_labelSetText(_plan->levelInfo, tmp, GOC_FALSE);
	tmp = goc_stringFree(tmp);
#ifdef _TESTING_
	budujNapisInt(_plan->pustakInfo, "Pustaki", _plan->liczPustki, _plan->liczPustki);
#endif
}

// folder - folder gry w katalogu gier (zwykle oznacza zestaw)
// poziom - poziom gry
void planZaladujGre(const char *folder, unsigned int poziom)
{
	char buftmp[160];
	char *tmp;
	char **pInfo=NULL;
	unsigned int nInfo=0;
	FILE *plik = NULL;
	unsigned int i;

	memset( buftmp, 0, 160 );
	sprintf( buftmp, "%03d", poziom );

	tmp = goc_stringCopy(NULL, GAME_FOLDER);
	tmp = goc_stringAdd(tmp, "/");
	tmp = goc_stringAdd(tmp, folder);

	if ( !goc_isFolder( tmp ) )
	{
		GOC_FERROR( goc_stringAdd( goc_stringCopy(NULL, "Brak katalogu z poziomami: "), tmp) );
		goc_stringFree(tmp);
		exit(-1);
	}

	tmp = goc_stringAdd(tmp, "/level");
	tmp = goc_stringAdd(tmp, buftmp);

	if ( !goc_isFileExists( tmp ) )
	{
		GOC_FERROR( goc_stringAdd( goc_stringCopy(NULL, "Brak pliku poziomu: "), tmp) );
		goc_stringFree(tmp);
		exit(-1);
	}

	plik = fopen(tmp, "r");
	if ( !plik )
	{
		GOC_FERROR( goc_stringAdd( goc_stringCopy(NULL, "Problem otwarcia pliku poziomów: "), tmp) );
		goc_stringFree(tmp);
		exit(-1);
	}
	tmp = goc_stringFree(tmp);

	memset(buftmp, 0, 160);
	while ( fgets(buftmp, 160, plik) )
	{
		char *tmp;
		if ( (tmp = strchr(buftmp, '\n')) )
			*tmp = 0;
		if ( (tmp = strchr(buftmp, '\n')) )
			*tmp = 0;
		pInfo = goc_tableAdd(pInfo, &nInfo, sizeof(char*));
		pInfo[nInfo-1] = goc_stringCopy(0, buftmp);
		memset(buftmp, 0, 160);
	}
	fclose(plik);

	_plan->dpl = planLiczDlugosc( (const char **)pInfo, nInfo );
	_plan->wpl = nInfo;
	goc_maskSetRealArea( _plan->gra, _plan->dpl, _plan->wpl );
	goc_elementSetX( _plan->gra, (goc_screenGetWidth()>>1)-(_plan->dpl>>1));
	goc_elementSetY( _plan->gra, (goc_screenGetHeight()>>1)-(_plan->wpl>>1));
	goc_elementSetWidth(_plan->gra, _plan->dpl);
	goc_elementSetHeight(_plan->gra, _plan->wpl);

	_plan->liczPustki = 0;
	_plan->liczRuch = 0;
	_plan->liczPosow = 0;
	cofRuchCzysc();
	_plan->czyStart = GOC_FALSE; // czy wystartowal (aby rozpoczac liczenie czasu
	// i aby nie mozna bylo zerowac czasu po powrocie do poczatku poprzez
	// cofanie ruchów)
	_plan->minRuch = planSzukajNajlepszy( _scena->poziom, 1 );
	_plan->minPosow = planSzukajNajlepszy( _scena->poziom, 2 );
	planBuduj( pInfo, nInfo );

	for (i=0; i<nInfo; i++)
		free(pInfo[i]);
	pInfo = goc_tableClear(pInfo, &nInfo);

//	ustFocus( gra );
}

void planRuch( int x, int y )
{
	register short nx = _plan->graczX + x;
	register short ny = _plan->graczY + y;
	register short pny = _plan->graczY + 2*y;
	register short pnx = _plan->graczX + 2*x;
	register unsigned char wart1 = goc_maskGet( _plan->gra, nx, ny );
	register unsigned char wart2 = ZNAK_SCIANA;
	if ( (pnx >= 0) && (pny >= 0) && (pnx < _plan->dpl) && (pny < _plan->wpl) )
		wart2 = goc_maskGet( _plan->gra, pnx, pny );
	if ( wart1 == ZNAK_SCIANA )// scianka
		return;
	if ( (wart1 == ZNAK_BALON || wart1 == ZNAK_BALON_MAGAZYN )
		  	&&	( wart2 == ZNAK_BALON || wart2 == ZNAK_BALON_MAGAZYN
			  	||	wart2 == ZNAK_SCIANA ) ) // jagody
		return;
	
	// Je¿eli jest to pierwszy ruch to rozpocznij liczenie czasu
	if ( _plan->czyStart == GOC_FALSE )
  	{
		time( &czas_start );
		_plan->czyStart = GOC_TRUE;
	}

	_plan->liczRuch++;
	// wymaz gracza
	if ( goc_maskGet( _plan->gra, _plan->graczX, _plan->graczY ) == ZNAK_LUDEK )//pustka pod graczem
	{
		cofRuchDodaj( _plan->graczX, _plan->graczY, ZNAK_LUDEK );
		goc_maskSet( _plan->gra, _plan->graczX, _plan->graczY, ZNAK_PODLOGA );
	}
	else // pole skladowania pod graczem
	{
		cofRuchDodaj( _plan->graczX, _plan->graczY, ZNAK_LUDEK_MAGAZYN );
		goc_maskSet( _plan->gra, _plan->graczX, _plan->graczY, ZNAK_MAGAZYN );
	}
	goc_maskPaintPoint( _plan->gra, _plan->graczX, _plan->graczY );

	cofRuchDodaj( nx, ny, wart1 );
	// zabawa z jagodami i polami
	if ( wart1 ) {// jagoda lub pole 
		if ( wart1 == ZNAK_MAGAZYN ) // pole
		{
			goc_maskSet( _plan->gra, (_plan->graczX = nx), (_plan->graczY = ny), ZNAK_LUDEK_MAGAZYN );
		}
		else { // jagoda
			_plan->liczPosow++;
			cofRuchDodaj( pnx, pny, wart2 );
			if ( wart1 == ZNAK_BALON ) {// jagoda na pustym
				if ( wart2 ) {// idzie na pole
					_plan->liczPustki--;
					goc_maskSet( _plan->gra, pnx, pny, ZNAK_BALON_MAGAZYN );
				} else {// idzie na puste
					goc_maskSet( _plan->gra, pnx, pny, ZNAK_BALON );
				}
				goc_maskSet( _plan->gra, (_plan->graczX = nx), (_plan->graczY = ny), ZNAK_LUDEK );
			}	else {// jagoda na pelnym
				if ( wart2 ) {// idzie na pole
					goc_maskSet( _plan->gra, pnx, pny, ZNAK_BALON_MAGAZYN );
				} else {// idzie na puste
					_plan->liczPustki++;
					goc_maskSet( _plan->gra, pnx, pny, ZNAK_BALON );
				}
				goc_maskSet( _plan->gra, (_plan->graczX = nx), (_plan->graczY = ny), ZNAK_LUDEK_MAGAZYN );
			}
		}
	} else // pustka
	{
		goc_maskSet( _plan->gra, (_plan->graczX = nx), (_plan->graczY = ny), ZNAK_LUDEK );
	}

	goc_maskPaintPoint( _plan->gra, _plan->graczX, _plan->graczY );
	goc_maskPaintPoint( _plan->gra, pnx, pny );
	budujNapisInt(_plan->ruchInfo, konfplikDajUstParametr(_scena->konf, "T8", "Ruch"), _plan->liczRuch, _plan->minRuch);
	budujNapisInt(_plan->pychInfo, konfplikDajUstParametr(_scena->konf, "T9", "Pych"), _plan->liczPosow, _plan->minPosow);
#ifdef _TESTING_
	budujNapisInt(_plan->pustakInfo, "Pustaki", _plan->liczPustki, _plan->liczPustki);
#endif
	GOC_MSG_PAINT( msgpaint );
	goc_systemSendMsg(_plan->ruchInfo, msgpaint);
	goc_systemSendMsg(_plan->pychInfo, msgpaint);
#ifdef _TESTING_
	goc_systemSendMsg(_plan->pustakInfo, msgpaint);
#endif
	goc_gotoxy(1, 1);

//			gra->rysuj();
	fflush( stdout );
	// Czy gra dobieg³a koñca
	if ( !_plan->liczPustki )
	{
		GOC_StElement e;
		// Logowanie
		planRobLog();
		// Rysowanie informacji
		memset(&e, 0, sizeof(GOC_StElement));
		e.x = 25; e.y = 10; e.width = 30; e.height = 6;
		e.color = GOC_WHITE | GOC_FBOLD;
		goc_frameDrawer( &e, "*-*|.|*-*" );
		e.x = 27; e.y = 12; e.width = 26; e.height = 1;
		e.color = GOC_CYAN | GOC_FBOLD;
		e.flag |= GOC_EFLAGA_CENTER;
		goc_labelDrawer( &e, konfplikDajUstParametr(_scena->konf, "TA", "Gratulacje"));
		e.x = 27; e.y = 13; e.width = 26; e.height = 1;
		goc_labelDrawer( &e, konfplikDajUstParametr(_scena->konf, "TB", "Gotuj siê na trudniejsze"));
		goc_gotoxy(1, 1);
		fflush(stdout);
		sleep(3);
		// Przejscie do nastepnego poziomu
		(_scena->poziom)++;
		planZaladujGre( konfplikDajParametr(_scena->konf, KONFKLUCZ_FOLDER), _scena->poziom );
		goc_clearscreen();
		goc_systemSendMsg(_plan->forma, msgpaint);
	}
}

int planLiczDlugosc(const char **pString, unsigned int nString)
{
	unsigned int i = 0;
	unsigned int j = 0;
	for ( j=0; j<nString; j++ )
	{
		if ( i<strlen(pString[j]) )
			i = strlen(pString[j]);
	}
	return i;
}


void planBudujOtoczenie()
{
	char *tmp = NULL;
	
	tmp = goc_stringAdd(tmp, konfplikDajUstParametr(_scena->konf, "T4", "Autor"));
	tmp = goc_stringAdd(tmp, ": ");
	tmp = goc_stringAdd(tmp, konfplikDajParametr(_scena->globus, GLOBUS_AUTOR));
	tmp = goc_stringAdd(tmp, "  ");
	tmp = goc_stringAdd(tmp, konfplikDajUstParametr(_scena->konf, "T5", "Tytu³"));
	tmp = goc_stringAdd(tmp, ": ");
	tmp = goc_stringAdd(tmp, konfplikDajParametr(_scena->globus, GLOBUS_TYTUL));
	goc_labelSetText(_plan->inform, tmp, GOC_FALSE);
	free(tmp);
}

GOC_COLOR planDoKolor(const char *ciag)
{
	GOC_COLOR tmp = 0;
	if ( strlen(ciag) <= 2 )
		return GOC_WHITE;
	if ( strstr(ciag,"bia³y") )
		tmp = GOC_WHITE;
	else if ( strstr(ciag,"czerwony") )
		tmp = GOC_RED;
	else if ( strstr(ciag,"niebieski") )
		tmp = GOC_BLUE;
	else if ( strstr(ciag,"zielony") )
		tmp = GOC_GREEN;
	else if ( strstr(ciag,"cyjankowy") )
		tmp = GOC_CYAN;
	else if ( strstr(ciag,"¿ó³ty") )
		tmp = GOC_YELLOW;
	if ( strstr(ciag,"jasny") )
		tmp |= GOC_FBOLD;
	
	return tmp;
}

void planNadajWartosc( const char *klucz, unsigned int pozycja, char znak, GOC_COLOR color )
{
	const char *tmp = konfplikDajParametr( _scena->konf, klucz );
	if ( strlen(tmp) )
		goc_maskSetValue(_plan->gra, pozycja, tmp[0], planDoKolor(tmp) );
	else
		goc_maskSetValue( _plan->gra, pozycja, znak, color );
}

int pokazOknoNasluch(GOC_HANDLER u, GOC_StMessage* msg)
{
	if ( u == _plan->gra )
	{
		if ( msg->id == GOC_MSG_CHAR_ID )
		{
			GOC_StMsgChar* msgchar = (GOC_StMsgChar*)msg;
			switch ( msgchar->charcode )
			{
				case 'u': // cofnij ruch
				{
					// Zawsze jest porzucane najpierw po³o¿enie go¶cia, a potem innych
					// elementów przemieszczanych
					unsigned char w;
					GOC_BOOL czyPodloga = GOC_FALSE;
					if ( cofRuchCzyJest() == GOC_FALSE )
						return GOC_ERR_OK;
					do {
						w = cofRuchDajw();
						if ( w == ZNAK_PODLOGA )
							czyPodloga = GOC_TRUE;
						goc_maskSet(_plan->gra, cofRuchDajx(), cofRuchDajy(), w);
						goc_maskPaintPoint(_plan->gra, (_plan->graczX = cofRuchDajx()), (_plan->graczY = cofRuchDajy()));
						if ( w == ZNAK_BALON || w == ZNAK_BALON_MAGAZYN )
						{
							_plan->liczPosow--;
							if ( w == ZNAK_BALON && czyPodloga == GOC_FALSE )
								_plan->liczPustki++;
							if ( w == ZNAK_BALON_MAGAZYN && czyPodloga == GOC_TRUE )
								_plan->liczPustki--;
						}
						cofRuchCofnij();
					} while ( w != ZNAK_LUDEK && w != ZNAK_LUDEK_MAGAZYN );
					_plan->liczRuch--;
					GOC_MSG_PAINT( msgpaint );
					goc_systemSendMsg(_plan->ruchInfo, msgpaint);
					goc_systemSendMsg(_plan->pychInfo, msgpaint);
#ifdef _TESTING_
					goc_systemSendMsg(_plan->pustakInfo, msgpaint);
#endif
					fflush( stdout );
					return GOC_ERR_OK;
				}
				case 'r': // zresetuj do poczatku
				{
					planZaladujGre( konfplikDajParametr( _scena->konf, KONFKLUCZ_FOLDER ), _scena->poziom );
					planBudujOtoczenie();
					GOC_MSG_PAINT( msgpaint );
					goc_systemSendMsg(_plan->forma, msgpaint);
					return GOC_ERR_OK;
				}
				case 0x603: // gora
				planRuch(0, -1);
				return GOC_ERR_OK;
				case 0x600: // dol
				planRuch(0, 1);
				return GOC_ERR_OK;
				case 0x602: // prawo
				planRuch(1, 0);
				return GOC_ERR_OK;
				case 0x601:
				planRuch(-1, 0);
				return GOC_ERR_OK;
				default:
				return goc_systemDefaultAction(u, msg);
			}
		}
	}
	return goc_systemDefaultAction(u, msg);
}

void pokazOknoPlanszy()
{
	GOC_HANDLER maparaw;
	char *tmp = NULL;
	_plan = (StOknoPlansza*)malloc(sizeof(StOknoPlansza));
	memset(_plan, 0, sizeof(StOknoPlansza));
	_plan->forma = goc_elementCreate( GOC_ELEMENT_FORM, 1, 1, 0, 0, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	_plan->gra = goc_elementCreate( GOC_ELEMENT_MASK, 1, 1, 1, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE,
			GOC_WHITE, _plan->forma );
//	maskaUstRodzajWartosci(_plan->gra, 3, 1);
	goc_maskAddMap(_plan->gra, maparaw = goc_elementCreate(GOC_ELEMENT_RAWMAP, 1, 1, 1, 1, GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE, GOC_WHITE, _plan->gra) );
	goc_maparawSetBPV(maparaw, 3);
	planNadajWartosc( "PODLOGA", ZNAK_PODLOGA, ' ', GOC_WHITE );
	planNadajWartosc( "LUDEK", ZNAK_LUDEK, '@', GOC_YELLOW | GOC_FBOLD );
	planNadajWartosc( "LUDEK_MAGAZYN", ZNAK_LUDEK_MAGAZYN, '@', GOC_CYAN | GOC_FBOLD );
	planNadajWartosc( "BALON", ZNAK_BALON, 'o', GOC_RED );
	planNadajWartosc( "BALON_MAGAZYN", ZNAK_BALON_MAGAZYN, 'o', GOC_RED | GOC_FBOLD );
	planNadajWartosc( "SCIANA", ZNAK_SCIANA, '#', GOC_GREEN );
	planNadajWartosc( "MAGAZYN", ZNAK_MAGAZYN, 'x', GOC_BLUE | GOC_FBOLD );
	_plan->status = goc_elementCreate( GOC_ELEMENT_LABEL, 1, 1, 0, 1,
		  	GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER,
			GOC_BCYAN | GOC_YELLOW | GOC_FBOLD, _plan->forma );
	tmp = goc_stringAdd( tmp, konfplikDajUstParametr(_scena->konf, "T4", "Autor") );
	tmp = goc_stringAdd( tmp, ": " );
	tmp = goc_stringAdd( tmp, PROGRAM_AUTHOR );
	tmp = goc_stringAdd( tmp, "    ");
	tmp = goc_stringAdd( tmp, konfplikDajUstParametr(_scena->konf, "T5", "Tytu³") );
	tmp = goc_stringAdd( tmp, ": ");
	tmp = goc_stringAdd(tmp, PROGRAM_TITLE );
	goc_labelSetText(_plan->status, tmp, GOC_FALSE);
	tmp = goc_stringFree(tmp);
	_plan->inform = goc_elementCreate( GOC_ELEMENT_LABEL, 1, 0, 0, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER, GOC_BCYAN | GOC_YELLOW | GOC_FBOLD,
		  	_plan->forma );

	// level info
	_plan->levelInfo = goc_elementCreate( GOC_ELEMENT_LABEL, 1, 2, 4, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER, GOC_WHITE, _plan->forma );
	tmp = goc_stringCopy(tmp, "L:");
	tmp = goc_stringAddInt(tmp, _scena->poziom);
	goc_labelSetText(_plan->levelInfo, tmp, GOC_FALSE);
	tmp = goc_stringFree(tmp);
	// move statistics
	_plan->ruchInfo = goc_elementCreate( GOC_ELEMENT_LABEL, 10, 2, 18, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER, GOC_WHITE, _plan->forma );
	budujNapisInt(_plan->ruchInfo, konfplikDajUstParametr(_scena->konf, "T8", "Ruch"), _plan->liczRuch, _plan->minRuch);
	_plan->pychInfo = goc_elementCreate( GOC_ELEMENT_LABEL, 30, 2, 18, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER, GOC_WHITE, _plan->forma );
	budujNapisInt(_plan->pychInfo, konfplikDajUstParametr(_scena->konf, "T9", "Pych"), _plan->liczPosow, _plan->minPosow);
#ifdef _TESTING_
	_plan->pustakInfo = goc_elementCreate( GOC_ELEMENT_LABEL, -19, 2, 18, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER, GOC_WHITE, _plan->forma );
	budujNapisInt(_plan->pustakInfo, "Pustaki", _plan->liczPustki, _plan->liczPustki);
#endif
	planZaladujGre( konfplikDajParametr( _scena->konf, KONFKLUCZ_FOLDER ), _scena->poziom );
	planBudujOtoczenie();
	goc_elementSetFunc(_plan->gra, GOC_FUNID_LISTENER, &pokazOknoNasluch);
	goc_formShow(_plan->forma);
}

const char *liczPoziomy(const char *path)
{
	static char buftmp[4];
	int i=0;
	char tmp[1024];
	
	do {
		i++;
		memset( buftmp, 0, 4 );
		sprintf( buftmp, "%03d", i );
		memset(tmp, 0, 1024);
		sprintf(tmp, "%s/%s/level%s", GAME_FOLDER, path, buftmp);
	} while ( goc_isFileExists( tmp ) );

	i--;
	sprintf( buftmp, "%03d", i );
	return buftmp;
}

int bazowaNasluch(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( uchwyt == GOC_HANDLER_SYSTEM )
	{
		if ( msg->id == GOC_MSG_CHAR_ID )
		{
			GOC_StMsgChar* msgchar = (GOC_StMsgChar*)msg;
			if ( msgchar->charcode == 13 ) // Enter
			{
				switch ( goc_listGetCursor( _scena->lista ) )
				{
					case 0: //start
						// kreuj i startuj okno planszy
						pokazOknoPlanszy();
						break;
					case 1: // poziom
						pokazOknoWybieraniaPoziomu();
						// kreuj i startuj okno poziomu
						break;
					case 2: {
						// koniec
						GOC_MSG_FINISH( msgfinish, 0 );
						goc_systemSendMsg(GOC_HANDLER_SYSTEM, msgfinish);
						break;
					}
					default:
						return GOC_ERR_UNKNOWNMSG;
				}
				return GOC_ERR_OK;
			}
			else
				return GOC_ERR_UNKNOWNMSG;
		}
	}
	return goc_systemDefaultAction(uchwyt, msg);
}

StBazowaScena *bazowaKreuj( StKonfPlik *konf )
{
	StBazowaScena *tmp = (StBazowaScena*)malloc(sizeof(StBazowaScena));
	StTabPlik *tp = NULL;
	char *globkonf = NULL;
	const char *folder;

	memset( tmp, 0, sizeof(StBazowaScena) );
	tmp->konf = konf;
	tmp->aktokno = 0;
	tmp->poziom = 1;

	folder = konfplikDajParametr( tmp->konf, KONFKLUCZ_FOLDER );

	globkonf = goc_stringCopy(NULL, GAME_FOLDER);
	globkonf = goc_stringAdd(globkonf, "/");
	globkonf = goc_stringAdd(globkonf, folder);

	if ( !goc_isFolder( globkonf ) )
	{
		GOC_FERROR( goc_stringAdd( goc_stringCopy(NULL, "Brak katalogu z poziomami: "), globkonf) );
		exit(-1);
	}
	globkonf = goc_stringAdd( globkonf, "/global.desc" );
	if ( !goc_isFileExists( globkonf ) )
	{
		GOC_FERROR( goc_stringAdd( goc_stringCopy(NULL, "Brak pliku konfiguracji: "), globkonf) );
		free(globkonf);
		exit(-1);
	}
	{
		char pBuf[80];
		tp = tabplikKreuj( dajRekordPlik(pBuf, 80), "|", "#" );
	}
	while ( tabplikCzytajWiersz( tp ) )
	{
		if ( (atoi( tabplikDajKolumna( tp, 0 ) ) == tmp->poziom)
				&& (strcmp(konfplikDajParametr( tmp->konf, KONFKLUCZ_FOLDER ),
						tabplikDajKolumna( tp, 5 )) == 0) )
		{
			tabplikSkokPoczatek( tp );
			(tmp->poziom)++;
		}

	}
	tabplikUsun(tp);
	// Czytaj konfiguracje - Autor, Tytul
	tmp->globus = konfplikKreuj(globkonf, ":");
	// Ustal liczbe poziomow
	konfplikUstParametr( tmp->globus, GLOBUS_MAKS,
		  	liczPoziomy(konfplikDajParametr(tmp->konf, KONFKLUCZ_FOLDER)) );
	goc_systemSetListenerFunc( &bazowaNasluch );
	tmp->lista = goc_elementCreate(GOC_ELEMENT_LIST, (goc_screenGetWidth()>>1)-15, 7, 30, 5,
		  	GOC_EFLAGA_ENABLE | GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER, GOC_WHITE, GOC_HANDLER_SYSTEM);
	((GOC_StList*)tmp->lista)->kolorPoleKursor = GOC_BGREEN;
	goc_listAddText( tmp->lista, konfplikDajUstParametr(tmp->konf, "T1", "S T A R T") );
	goc_listAddText( tmp->lista, konfplikDajUstParametr(tmp->konf, "T2", "I N N Y   P O Z I O M") );
	goc_listAddText( tmp->lista, konfplikDajUstParametr(tmp->konf, "T3", "K O N I E C") );
	// Dodaj linie statusu
	tmp->status = goc_elementCreate(GOC_ELEMENT_LABEL, 1, 0, 0, 1,
		  	GOC_EFLAGA_PAINTED | GOC_EFLAGA_CENTER, GOC_BCYAN | GOC_YELLOW | GOC_FBOLD,
		  	GOC_HANDLER_SYSTEM);
	{
		char pBuf[80];
		sprintf(pBuf, "%s: %s    %s: %s",
				konfplikDajUstParametr(tmp->konf, "T4", "Autor"),
				PROGRAM_AUTHOR,
				konfplikDajUstParametr(tmp->konf, "T5", "Tytu³"),
				PROGRAM_TITLE
				);
		goc_labelSetText(tmp->status, pBuf, GOC_FALSE);
	}

	GOC_MSG_PAINT( msgpaint );
	goc_systemSendMsg(GOC_HANDLER_SYSTEM, msgpaint);
	goc_systemFocusOn(tmp->lista);

	free(globkonf);
	return tmp;
}

// Program g³ówny
int main( int argc, char **argv )
{
	GOC_StMessage wiesc;
	StKonfPlik *x = konfplikKreuj("plik.cnf", "|:");
	if ( argc == 2 )
		konfplikUstParametr( x, KONFKLUCZ_FOLDER, argv[1] );
	_scena = bazowaKreuj(x);
	while (goc_systemCheckMsg( &wiesc ));
	konfplikUsun( x );
	konfplikUsun( _scena->globus );
	free( _scena );

	return 0;
}

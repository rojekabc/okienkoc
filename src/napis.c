#include "global-inc.h"
#ifndef _DEBUG
#	include <string.h>
#endif
#include <stdio.h>
#include "napis.h"
#include "tablica.h"
#include "screen.h"
#include "mystr.h"
#include "system.h"

const char* GOC_ELEMENT_LABEL = "GOC_Label";

int goc_labelSetColor(GOC_HANDLER u, unsigned short line, GOC_COLOR color)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	if ( line == (unsigned short)~0 )
	{
		/* wykonaj ustawienie dla wszystkich elementów */
		int i;
		unsigned int n = napis->nText;
		for ( i=0; i<n; i++ )
			napis->tekst[i].color = color;
	}
	else
	{
		/* wykonaj ustawienie dla wybranego elementu */
		if ( line >= napis->nText )
			return GOC_ERR_END;
		napis->tekst[line].color = color;
	}
	// TODO: Paint only changed line
	if ( napis->flag & GOC_EFLAGA_SHOWN ) {
		GOC_MSG_PAINT( msg );
		goc_systemSendMsg(u, msg);
	}
	return GOC_ERR_OK;
}

/*
 * read a color of line in napis
 */
GOC_COLOR goc_labelGetColor(GOC_HANDLER u, unsigned short line)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	if ( line >= napis->nText )
		return GOC_WHITE;
	return napis->tekst[line].color;
}

int goc_labelSetFlag(GOC_HANDLER u, unsigned short line, GOC_FLAGS flag)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	flag &= (GOC_FLAGS)COC_EFLAGA_G_JUSTIFY;
	if ( line == (unsigned short)~0 )
		/* wykonaj ustawienie dla wszystkich elementów */
	{
		int i;
		GOC_FLAGS *f = &(napis->flag);
		unsigned int n = napis->nText;
		*f &= (GOC_FLAGS)~COC_EFLAGA_G_JUSTIFY;
		*f |= flag;
		for ( i=0; i<n; i++ )
		{
			napis->tekst[i].flag &= (GOC_FLAGS)~COC_EFLAGA_G_JUSTIFY;
			napis->tekst[i].flag |= flag;
		}
	}
	else
		/* wykonaj ustawienie dla wybranego elementu */
	{
		if ( line >= napis->nText )
			return GOC_ERR_END;
		napis->tekst[line].flag &= (GOC_FLAGS)~COC_EFLAGA_G_JUSTIFY;
		napis->tekst[line].flag |= flag;
	}
	if ( napis->flag & GOC_EFLAGA_SHOWN ) {
		GOC_MSG_PAINT( msg );
		goc_systemSendMsg(u, msg);
	}
	return GOC_ERR_OK;
}

GOC_BOOL goc_labelIsFlag(GOC_HANDLER u, unsigned short line, GOC_FLAGS f)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	f &= (GOC_FLAGS)~COC_EFLAGA_G_JUSTIFY;
	if ( line == (unsigned short)~0 )
	{
		if ( (napis->flag & f) == f )
			return GOC_TRUE;
		else
			return GOC_FALSE;
	}
	else
	{
		if ( line >= napis->nText )
			return GOC_FALSE;
		if ( (napis->tekst[line].flag & f) == f )
			return GOC_TRUE;
		else
			return GOC_FALSE;
	}
}

int goc_labelGetText(
		GOC_HANDLER u, unsigned short line, char *pBuf, unsigned int nBuf)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	if ( line >= napis->nText )
		return GOC_ERR_END;
	strncpy(pBuf, napis->tekst[line].line, nBuf);
	return GOC_ERR_OK;
}

int goc_labelGetLinesAmmount(GOC_HANDLER u)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	return napis->nText;
}

int goc_labelSetStart(GOC_HANDLER u, unsigned short s)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	if ( s >= napis->nText )
		return GOC_ERR_END;
	napis->start = s;
	return GOC_ERR_OK;
}

int goc_labelGetStart(GOC_HANDLER u)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	return napis->start;
}

int goc_labelRemLines(GOC_HANDLER u)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	int i;
	for ( i = 0; i<napis->nText; i++ )
		goc_stringFree(napis->tekst[i].line);
	napis->tekst = goc_tableClear(napis->tekst, &napis->nText);
	return GOC_ERR_OK;
}

int goc_labelRemLine(GOC_HANDLER u, unsigned int numer)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	
	if ( numer < napis->nText )
  	{
		goc_stringFree(napis->tekst[numer].line);
		napis->tekst = goc_tableRemove(napis->tekst, &napis->nText, sizeof(GOC_StColorLine), numer);
	}
	return GOC_ERR_OK;
}

int goc_labelAddLine(GOC_HANDLER u, const char *Tekst)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	if ( !Tekst )
		return GOC_ERR_OK; // ??
	napis->tekst = goc_tableAdd(napis->tekst, &napis->nText, sizeof(GOC_StColorLine));
	napis->tekst[napis->nText-1].line = goc_stringCopy(NULL, Tekst);
	napis->tekst[napis->nText-1].color = napis->color;
	napis->tekst[napis->nText-1].flag = napis->flag;
	if ( napis->flag & GOC_EFLAGA_SHOWN ) {
		GOC_MSG_PAINT( msg );
		goc_systemSendMsg(u, msg);
	}
	return GOC_ERR_OK;
}

int goc_labelSetText(GOC_HANDLER u, char *tekst, GOC_BOOL licz)
{
	GOC_StLabel *napis = (GOC_StLabel*)u;
	goc_labelRemLines(u);
	goc_labelAddLine(u, tekst);
	if ( licz == GOC_TRUE )
		goc_elementSetWidth(u, strlen(tekst));
	else if ( napis->flag & GOC_EFLAGA_SHOWN ) {
		GOC_MSG_PAINT( msg );
		goc_systemSendMsg(u, msg);
	}
	return GOC_ERR_OK;
}

int goc_labelDrawer(GOC_StElement *elem, const char *tekst)
{
	char *line; // line do wypisania
	unsigned int d; // d³ugo¶æ do kopiowania dla linii do wypisania
	unsigned int p = 0; // przesuniêcie wzglêdem pocz±tku punktu pisania
	GOC_POSITION w = goc_elementGetWidth((GOC_HANDLER)elem);
	line = (char *)malloc(w+1);
	memset(line, 0, w+1);
	memset(line, ' ', w);
	if ( tekst )
		d = strlen(tekst) > w ? w : strlen(tekst);
	else
		d = w;
	switch ( elem->flag & COC_EFLAGA_G_JUSTIFY )
	{
		case GOC_EFLAGA_RIGHT:
			p = w - d;
			break;
		case GOC_EFLAGA_LEFT:
			p = 0;
			break;
		case GOC_EFLAGA_CENTER:
			p = w - d;
			p >>= 1;
			break;
	}
	if ( tekst )
		memcpy( line + p, tekst, d );
	goc_gotoxy( 
		goc_elementGetScreenX( (GOC_HANDLER)elem ),
		goc_elementGetScreenY( (GOC_HANDLER)elem ) );
	goc_textallcolor( elem->color );
	fputs( line, stdout );
	free(line);
	return GOC_ERR_OK;
}

static int labelPaint(GOC_StLabel *napis)
{
//	char *line; // line do wypisania
//	unsigned int d; // d³ugo¶æ do kopiowania dla linii do wypisania
//	unsigned int p = 0; // przesuniêcie wzglêdem pocz±tku punktu pisania
	unsigned int i; // licznik
	GOC_POSITION height;
	if ( napis->nText == 0 )
		return GOC_ERR_OK;
	if ( !(napis->flag & GOC_EFLAGA_PAINTED) )
		return GOC_ERR_OK;

//	line = (char *)malloc(napis->width+1);
//	memset(line, 0, napis->width+1);
	
	height = goc_elementGetHeight((GOC_HANDLER)napis);
	for ( i = 0; i < height; i++ )
  	{
		GOC_StElement elem;
		if ( i + napis->start >= napis->nText )
			break;
		memcpy( &elem,  napis, sizeof(GOC_StElement) );
//		memset( line, ' ', napis->width );
//		d = strlen(napis->tekst[i+napis->start].line) > napis->width ? napis->width :
//			strlen(napis->tekst[i+napis->start].line);
//		switch ( napis->tekst[i+napis->start].flag & COC_EFLAGA_G_JUSTIFY )
//	  	{
//			case GOC_EFLAGA_RIGHT:
//				p = napis->width - d;
//				break;
//			case GOC_EFLAGA_LEFT:
//				p = 0;
//				break;
//			case GOC_EFLAGA_CENTER:
//				p = napis->width - d;
//				p >>= 1;
//				break;
//		}
		elem.flag = napis->tekst[i+napis->start].flag;
//		memcpy( line + p, napis->tekst[i+napis->start].line, d );
		elem.y += i;
		elem.color = napis->tekst[i+napis->start].color;
		goc_elementGetLabelDrawer( (GOC_HANDLER)napis )(
			&elem, napis->tekst[i+napis->start].line );
//		goc_textallcolor( napis->tekst[i+napis->start].color );
//		fputs( line, stdout );
	}
	napis->flag |= GOC_EFLAGA_SHOWN;
	fflush(stdout);
//	free(line);
	return GOC_ERR_OK;
}

int goc_labelListener(GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_CREATE_ID )
	{
		GOC_CREATE_ELEMENT(GOC_StLabel, napis, msg);
		napis->tekst = NULL;
		napis->nText = 0;
		napis->start = 0;
		return GOC_ERR_OK;
	}
	else if ( msg->id == GOC_MSG_PAINT_ID )
	{
		return labelPaint((GOC_StLabel*)uchwyt);
	}
	else if ( msg->id == GOC_MSG_DESTROY_ID )
	{
		goc_labelRemLines(uchwyt);
		return goc_elementDestroy(uchwyt);
	}
	return GOC_ERR_UNKNOWNMSG;
}

// zwróæ ustawion± funkcjê rysuj±c± napis, a je¶li nie ma to domy¶lnie
// stosowan± do rysowania napisów
GOC_FUN_LABELDRAWER *goc_elementGetLabelDrawer(GOC_HANDLER uchwyt)
{
	void *tmp = goc_elementGetFunc(uchwyt, GOC_FUNID_LABELDRAWER);
	if ( tmp )
		return tmp;
	return &goc_labelDrawer;
}

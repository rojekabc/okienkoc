#include <okienkoc/okienkoc.h>
#include <stdio.h>

GOC_HANDLER forma;
GOC_HANDLER napis;

static int nasluchOk(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_ACTION_ID )
	{
		goc_labelRemLines( napis );
		goc_labelAddLine( napis, "Formularz wype³niono" );
		goc_formHide( forma );
		return GOC_ERR_OK;
	}
	return goc_systemDefaultAction(uchwyt, msg);
}

static int nasluchCancel(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_ACTION_ID )
	{
		goc_labelRemLines( napis );
		goc_labelAddLine( napis, "Formularz nie zosta³ wype³niony" );
		goc_formHide( forma );
		return GOC_ERR_OK;
	}
	return goc_systemDefaultAction(uchwyt, msg);
}

static int nasluchForm(
	GOC_HANDLER uchwyt, GOC_StMessage* msg)
{
	if ( msg->id == GOC_MSG_ACTION_ID )
	{
		goc_formShow( forma );
		return GOC_ERR_OK;
	}
	return goc_systemDefaultAction(uchwyt, msg);
}

int main()
{
	GOC_StMessage wiesc;
	GOC_HANDLER przycisk;

	przycisk = goc_elementCreate(
			GOC_ELEMENT_BUTTON, 10, 12, 15, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE | GOC_EFLAGA_CENTER,
			GOC_WHITE | GOC_FBOLD, GOC_HANDLER_SYSTEM );
	goc_buttonSetText( przycisk, "Formularz" );
	goc_elementSetFunc( przycisk, GOC_FUNID_LISTENER, nasluchForm );
	napis = goc_elementCreate(
			GOC_ELEMENT_LABEL, 10, 4, 32, 1,
			GOC_EFLAGA_PAINTED,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	goc_labelAddLine( napis, "Formularz nie zosta³ wype³niony" );

	forma = goc_elementCreate(
			GOC_ELEMENT_FORM, 10, 2, 24, 10,
			0,
			GOC_WHITE, GOC_HANDLER_SYSTEM );
	goc_elementCreate(
			GOC_ELEMENT_FRAME, 1, 1, 24, 10,
			GOC_EFLAGA_PAINTED,
			GOC_WHITE | GOC_FBOLD, forma );
	przycisk = goc_elementCreate(
			GOC_ELEMENT_BUTTON, 3, 7, 10, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE | GOC_EFLAGA_CENTER,
			GOC_WHITE | GOC_FBOLD, forma );
	goc_buttonSetText( przycisk, "Ok" );
	goc_elementSetFunc( przycisk, GOC_FUNID_LISTENER, nasluchOk );
	przycisk = goc_elementCreate(
			GOC_ELEMENT_BUTTON, 13, 7, 10, 1,
			GOC_EFLAGA_PAINTED | GOC_EFLAGA_ENABLE | GOC_EFLAGA_CENTER,
			GOC_WHITE | GOC_FBOLD, forma );
	goc_buttonSetText( przycisk, "Cancel" );
	goc_elementSetFunc( przycisk, GOC_FUNID_LISTENER, nasluchCancel );
	while (goc_systemCheckMsg( &wiesc ));
	return 0;
}

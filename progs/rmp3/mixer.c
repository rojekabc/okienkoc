#include "mixer.h"

#include <tools/mystr.h>
#include <tools/malloc.h>

enum MixerCode mixerGetCardsAmount(MixerHandler handler, int* amount) {
	MIXER_ASSERT_HANDLER( handler );
	MixerSystem* system = (MixerSystem*)handler;
	MIXER_ASSERT_OPERATION( system->mixerGetCardsAmount );
	MIXER_ASSERT_ARGUMENT( amount );

	return system->mixerGetCardsAmount( system, amount );
}

enum MixerCode mixerGetCardInfo(MixerHandler handler, int cardId, MixerCard** cardInfo) {
	MIXER_ASSERT_HANDLER( handler );
	MixerSystem* mixer = (MixerSystem*)handler;
	MIXER_ASSERT_OPERATION( mixer->mixerGetCardInfo );
	MIXER_ASSERT_ARGUMENT( cardInfo );

	return mixer->mixerGetCardInfo( mixer, cardId, cardInfo );
}

enum MixerCode mixerSelectCard(MixerHandler handler, int cardId) {
	MIXER_ASSERT_HANDLER( handler );
	MixerSystem* mixer = (MixerSystem*)handler;
	MIXER_ASSERT_OPERATION( mixer->mixerSelectCard );

	return mixer->mixerSelectCard( mixer, cardId );
}

enum MixerCode mixerGetElementsAmount(MixerHandler handler, int* amount) {
	MIXER_ASSERT_HANDLER( handler );
	MixerSystem* mixer = (MixerSystem*)handler;
	MIXER_ASSERT_OPERATION( mixer->mixerGetElementsAmount );
	MIXER_ASSERT_ARGUMENT( amount );

	return mixer->mixerGetElementsAmount( mixer, amount );
}

enum MixerCode mixerGetElementInfo(MixerHandler handler, int elementId, MixerElement** elementInfo) {
	MIXER_ASSERT_HANDLER( handler );
	MixerSystem* mixer = (MixerSystem*)handler;
	MIXER_ASSERT_OPERATION( mixer->mixerGetCardInfo );
	MIXER_ASSERT_ARGUMENT( elementInfo );

	return mixer->mixerGetElementInfo( mixer, elementId, elementInfo );
}

enum MixerCode mixerSelectElement(MixerHandler handler, int elementId) {
	MIXER_ASSERT_HANDLER( handler );
	MixerSystem* mixer = (MixerSystem*)handler;
	MIXER_ASSERT_OPERATION( mixer->mixerSelectElement );

	return mixer->mixerSelectElement( mixer, elementId );
}

enum MixerCode mixerChangeVolume(MixerHandler handler, int change, int* result) {
	MIXER_ASSERT_HANDLER( handler );
	MixerSystem* mixer = (MixerSystem*)handler;
	MIXER_ASSERT_OPERATION( mixer->mixerChangeVolume );

	return mixer->mixerChangeVolume( mixer, change, result );
}

enum MixerCode mixerGetSelectedElement(MixerHandler handler, MixerElement** element) {
	MIXER_ASSERT_HANDLER( handler );
	MixerSystem* mixer = (MixerSystem*)handler;
	MixerCard* card = mixer->active;
	if ( !card ) {
		return MIXER_CODE_NO_ACTIVE_CARD;
	}
	*element = card->active;
	return MIXER_CODE_OK;
}

MixerHandler mixerFree(MixerHandler handler) {
	MixerSystem* mixer = (MixerSystem*)handler;
	if ( mixer ) {
		if ( mixer->mixerCardClose ) {
			mixer->mixerCardClose( mixer );
		}
		mixer->mixerCard = goc_arrayClear( mixer->mixerCard );
		free( mixer );
	}
	return 0;
}


struct MixerSystem* mixerAlloc() {
	ALLOC(struct MixerSystem, mixer);
	mixer->mixerCard = goc_arrayAlloc();
	mixer->mixerCard->freeElement = (MIXER_FREE_TYPE)&mixerCardFree;

	return mixer;
}

struct MixerCard* mixerCardAlloc() {
	ALLOC(struct MixerCard, card);
	card->mixerElement = goc_arrayAlloc();
	card->mixerElement->freeElement = (MIXER_FREE_TYPE)&mixerElementFree;
	return card;
}

struct MixerCard* mixerCardFree(struct MixerCard* mixerCard) {
	if ( mixerCard ) {
		mixerCard->mixerElement = goc_arrayFree(mixerCard->mixerElement);
		mixerCard->name = goc_stringFree( mixerCard->name );
		mixerCard->physicalName = goc_stringFree( mixerCard->physicalName );
		free( mixerCard );
	}
	return NULL;
}

struct MixerElement* mixerElementAlloc() {
	ALLOC(struct MixerElement, element);
	return element;
}

struct MixerElement* mixerElementFree(struct MixerElement* mixerElement) {
	if ( mixerElement ) {
		goc_stringFree( mixerElement->name );
		free(mixerElement);
	}
	return NULL;
}


#include "common-mixer.h"

struct MixerSystem* commonMixerAlloc() {
	struct MixerSystem* system = mixerAlloc();

	system->mixerGetCardsAmount = commonMixerGetCardsAmount;
	system->mixerGetCardInfo = commonMixerGetCardInfo;
	system->mixerSelectCard = commonMixerSelectCard;
	system->mixerGetElementsAmount = commonMixerGetElementsAmount;
	system->mixerGetElementInfo = commonMixerGetElementInfo;
	system->mixerSelectElement = commonMixerSelectElement;

	return system;
}

enum MixerCode commonMixerGetCardsAmount(struct MixerSystem* system, int* amount) {
	*amount = system->mixerCard->nElement;
	return MIXER_CODE_OK;
}

enum MixerCode commonMixerGetCardInfo(struct MixerSystem* system, int mixerId, MixerCard** mixerCard) {
	if ( mixerId >= system->mixerCard->nElement ) {
		return MIXER_CODE_WRONG_ID;
	}
	*mixerCard = (MixerCard*)system->mixerCard->pElement[mixerId];
	return MIXER_CODE_OK;
}

enum MixerCode commonMixerSelectCard(struct MixerSystem* system, int mixerId) {
	return commonMixerGetCardInfo(system, mixerId, &system->active);
}

enum MixerCode commonMixerGetElementsAmount(struct MixerSystem* system, int* amount) {
	if ( !system->active ) {
		return MIXER_CODE_NO_ACTIVE_CARD;
	}
	*amount = system->active->mixerElement->nElement;
	return MIXER_CODE_OK;
}

enum MixerCode commonMixerGetElementInfo(struct MixerSystem* system, int elementId, MixerElement** mixerElement) {
	if ( !system->active ) {
		return MIXER_CODE_NO_ACTIVE_CARD;
	}
	if ( elementId >= system->active->mixerElement->nElement ) {
		return MIXER_CODE_WRONG_ID;
	}
	*mixerElement = (MixerElement*)system->active->mixerElement->pElement[elementId];
	return MIXER_CODE_OK;
}

enum MixerCode commonMixerSelectElement(struct MixerSystem* system, int elementId) {
	return commonMixerGetElementInfo(system, elementId, &system->active->active);
}

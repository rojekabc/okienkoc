#include "alsa-mixer.h"

#include <alsa/asoundlib.h>
#include <tools/mystr.h>
#define GOC_PRINTERROR
#include <tools/log.h>
#include <tools/malloc.h>

#include "common-mixer.h"

//--- INTERNAL TYPE DEFINITIONS ---
typedef struct AlsaMixerElement {
	MIXER_ELEMENT_TYPE;
	snd_mixer_elem_t* sndelement;
} AlsaMixerElement;

typedef struct AlsaMixerCard {
	MIXER_CARD_TYPE;
	snd_mixer_t* sndmixer;
} AlsaMixerCard;

//--- FUNCTIONS ---
static enum MixerCode alsaMixerCardClose(struct MixerSystem* system) {
	AlsaMixerCard* card = (AlsaMixerCard*)system->active;
	if ( !card ) {
		return MIXER_CODE_NO_ACTIVE_CARD;
	}
	if ( card->sndmixer ) {
		if ( snd_mixer_close( card->sndmixer ) < 0 ) {
			GOC_ERROR("Cannot close active mixer");
		}
		card->sndmixer = NULL;
		card->mixerElement = goc_arrayFree( card->mixerElement );
	}
	system->active = NULL;
	return MIXER_CODE_OK;
}

static AlsaMixerElement* alsaMixerElementAlloc() {
	ALLOC(struct AlsaMixerElement, element);
	return element;
}

static enum MixerCode alsaMixerSelectCard(struct MixerSystem* system, int mixerId) {
	enum MixerCode code;
	snd_mixer_elem_t* mixerElem = NULL;
	AlsaMixerCard* card = (AlsaMixerCard*)system->active;
	if ( !card  ) {
		if ( card == system->mixerCard->pElement[mixerId] ) {
			return MIXER_CODE_OK;
		}
		alsaMixerCardClose(system);
	}

	code = commonMixerSelectCard(system, mixerId);
	if ( code != MIXER_CODE_OK ) {
		GOC_DEBUG("Cannot select card by commonMixerSelectCard");
		return code;
	}
	card = (AlsaMixerCard*)system->active;

	GOC_CHEOP_DEBUG(snd_mixer_open( &card->sndmixer, 0 ) == 0, return MIXER_CODE_CANNOT_OPEN_CARD);
	GOC_CHEOP_DEBUG(snd_mixer_attach( card->sndmixer, card->physicalName ) == 0, return MIXER_CODE_CANNOT_OPEN_CARD);
	GOC_CHEOP_DEBUG(snd_mixer_selem_register( card->sndmixer, NULL, NULL ) == 0, return MIXER_CODE_CANNOT_OPEN_CARD);
	GOC_CHEOP_DEBUG(snd_mixer_load( card->sndmixer ) == 0, return MIXER_CODE_CANNOT_OPEN_CARD);

	mixerElem = snd_mixer_first_elem( card->sndmixer );
	while ( mixerElem ) {
		AlsaMixerElement* pMixerElement = alsaMixerElementAlloc();
		for (int channel = 0; channel <= SND_MIXER_SCHN_LAST; channel++ ) {
			if ( snd_mixer_selem_has_playback_channel(mixerElem, channel)
				&& snd_mixer_selem_get_playback_volume_range(mixerElem, &pMixerElement->rangemin, &pMixerElement->rangemax) == 0
				&& snd_mixer_selem_get_playback_volume(mixerElem, channel, &pMixerElement->volume) == 0 ) {
					pMixerElement->name = goc_stringCopy(NULL, snd_mixer_selem_get_name(mixerElem));
					pMixerElement->type = MIXER_ELEMENT_PLAYBACK;
					pMixerElement->sndelement = mixerElem;
					card->mixerElement = goc_arrayAdd( card->mixerElement, pMixerElement );
					break;
			} else if ( snd_mixer_selem_has_capture_channel(mixerElem, channel)
				&& snd_mixer_selem_get_capture_volume_range(mixerElem, &pMixerElement->rangemin, &pMixerElement->rangemax) == 0
				&& snd_mixer_selem_get_capture_volume(mixerElem, channel, &pMixerElement->volume) == 0 ) {
					pMixerElement->name = goc_stringCopy(NULL, snd_mixer_selem_get_name(mixerElem));
					pMixerElement->type = MIXER_ELEMENT_CAPTURE;
					pMixerElement->sndelement = mixerElem;
					card->mixerElement = goc_arrayAdd( card->mixerElement, pMixerElement );
					break;
			}
		}
		mixerElem = snd_mixer_elem_next( mixerElem );
	}
	return MIXER_CODE_OK;
}

static enum MixerCode alsaMixerChangeVolume(struct MixerSystem* system, int change, int* result) {
	struct AlsaMixerCard* card = (struct AlsaMixerCard*)system->active;
	if ( !card ) {
		return MIXER_CODE_NO_ACTIVE_CARD;
	}
	struct AlsaMixerElement* element = (struct AlsaMixerElement*)card->active;
	if ( !element ) {
		return MIXER_CODE_NO_ACTIVE_ELEMENT;
	}
	long range = element->rangemax - element->rangemin;
	long precentage = (element->volume - element->rangemin) / range;
	if ( range <= 100 ) {
		element->volume += change;
		if ( element->volume < element->rangemin ) {
			element->volume = element->rangemin;
		} else if ( element->volume > element->rangemax ) {
			element->volume = element->rangemax;
		}
		precentage = (element->volume - element->rangemin) / range;
	} else {
		precentage += change;
		if ( precentage < 0 ) {
			precentage = 0;
		} else if ( precentage > 100 ) {
			precentage = 100;
		}
		element->volume = element->rangemin + (range * precentage / 100);
	}
	if ( result ) {
		*result = (int)precentage;
	}

	if ( element->type == MIXER_ELEMENT_PLAYBACK
		&& snd_mixer_selem_set_playback_volume_all(element->sndelement, element->volume) < 0 ) {
			return MIXER_CODE_CANNOT_SET_VOLUME;
	} else if ( element->type == MIXER_ELEMENT_CAPTURE 
		&& snd_mixer_selem_set_capture_volume_all(element->sndelement, element->volume) < 0 ) {
			return MIXER_CODE_CANNOT_SET_VOLUME;
	}

	return MIXER_CODE_OK;
}

static struct AlsaMixerCard* alsaMixerCardAlloc() {
	ALLOC(struct AlsaMixerCard, card);
	card->mixerElement = goc_arrayAlloc();
	card->mixerElement->freeElement = (MIXER_FREE_TYPE)&mixerElementFree;
	return card;
}

static struct AlsaMixerCard* createMixerCard(int cardId) {
	struct AlsaMixerCard* pMixerCard = alsaMixerCardAlloc();

	snd_card_get_name( cardId, &pMixerCard->name );
	pMixerCard->physicalName = goc_stringAdd(pMixerCard->physicalName, "hw:");
	pMixerCard->physicalName = goc_stringAddInt(pMixerCard->physicalName, cardId);
	return pMixerCard;
}

MixerHandler alsaMixerAlloc() {
	int card = -1;
	int result = 0;
	MixerSystem* mixer = commonMixerAlloc();

	// setup functions
	mixer->mixerChangeVolume = alsaMixerChangeVolume;
	mixer->mixerSelectCard = alsaMixerSelectCard;
	mixer->mixerCardClose = alsaMixerCardClose;
	// setup card information
	while ( !result ) {
		result = snd_card_next( &card );
		if ( result || card == -1 ) {
			break;
		}
		struct AlsaMixerCard* mixerCard = createMixerCard( card );
		if ( mixerCard ) {
			mixer->mixerCard = goc_arrayAdd( mixer->mixerCard, mixerCard );
		}

	}

	return (MixerHandler)mixer;
}

// On base of page: www.4front-tech.com/pguide/mixer.html
#include "oss-mixer.h"

#ifdef _DEBUG
#	include <mpatrol.h>
#endif // ifdef _DEBUG
#include <stdlib.h>
#include <string.h>
#include <tools/plik.h>
#include <tools/mystr.h>
#include <tools/array.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/types.h>
#include <fcntl.h>

#include "common-mixer.h"

#define SOUND_IOCTL(a,b,c)	ioctl(a,b,c)
#define MAXLEVEL 100		/* highest level permitted by OSS drivers */

#define CARD_SIZE 4
static const char* cardDevices[CARD_SIZE] = {
	"/dev/mixer",
	"/dev/mixer0",
	"/dev/mixer1",
	"/dev/dsp"
};

const char *dev_label[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_NAMES;

typedef struct OssMixerElement {
	MIXER_ELEMENT_TYPE;
	int id;
} OssMixerElement;

typedef struct OssMixerCard {
	MIXER_CARD_TYPE;
	int fd;
} OssMixerCard;

static enum MixerCode ossMixerChangeVolume(struct MixerSystem* system, int change, int* result) {
	OssMixerCard* card = (OssMixerCard*)system->active;
	if ( !card ) {
		return MIXER_CODE_NO_ACTIVE_CARD;
	}
	OssMixerElement* element = (OssMixerElement*)card->active;
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
	element->volume &= 0x7F;
	int leftright = element->volume + (element->volume << 8);
	if (SOUND_IOCTL(card->fd,
		MIXER_WRITE(element->id),
	       	&leftright) == -1) {
		return MIXER_CODE_ERROR_SET_VOLUME;
	}
	return MIXER_CODE_OK;
}

static struct OssMixerElement* ossMixerElementAlloc() {
	OssMixerElement* element = malloc(sizeof(struct OssMixerElement));
	memset(element, 0, sizeof(struct OssMixerElement));
	return element;
}

static enum MixerCode ossMixerCardClose(MixerSystem* system) {
	struct OssMixerCard* card = (struct OssMixerCard*)system->active;
	if ( !card ) {
		return MIXER_CODE_NO_ACTIVE_CARD;
	}
	// close and clear last informations
	close( card->fd );
	card->fd = 0;
	card->mixerElement = goc_arrayClear( card->mixerElement );
	system->active = NULL;
	return MIXER_CODE_OK;
}

static enum MixerCode ossMixerSelectCard(struct MixerSystem* system, int cardId) {
	enum MixerCode code;
	struct OssMixerCard* card = (OssMixerCard*)system->active;
	if ( card ) {
		if ( card == system->mixerCard->pElement[cardId] ) {
			return MIXER_CODE_OK;
		} else {
			ossMixerCardClose(system);
		}
	}

	code = commonMixerSelectCard( system, cardId );
	if ( code != MIXER_CODE_OK ) {
		return code;
	}
	card = (OssMixerCard*)system->active;

	card->fd = open(card->physicalName, O_RDWR);
	if ( card->fd < 0 ) {
		return MIXER_CODE_CANNOT_OPEN_CARD;
	}

	// playback elements
	int mask;
	if (SOUND_IOCTL(card->fd, SOUND_MIXER_READ_DEVMASK, &mask) == 0) {
		for (int i=0; i<SOUND_MIXER_NRDEVICES; i++) {
			if ( (1<<i) & mask ) {
				int level;
				OssMixerElement* element = ossMixerElementAlloc();
				element->name = goc_stringCopy(
					element->name,
				       	dev_label[i]);
				element->type = MIXER_ELEMENT_PLAYBACK;
				element->rangemin = 0;
				element->rangemax = MAXLEVEL;
				element->id = i;
				if (SOUND_IOCTL(card->fd, MIXER_READ(i), &level) == 0) {
					int left = level & 0x7F;
					int right = (level >> 8) & 0x7F;
					element->volume = left > right ? left : right;
				}
				card->mixerElement = goc_arrayAdd( card->mixerElement, element );
			}
		}
	}

	// capture elements
	if (SOUND_IOCTL(card->fd, SOUND_MIXER_READ_RECMASK, &mask) == 0) {
		for (int i=0; i<SOUND_MIXER_NRDEVICES; i++) {
			if ( (1<<i) & mask ) {
				int level;
				OssMixerElement* element = ossMixerElementAlloc();
				element->name = goc_stringCopy(
					element->name,
				       	dev_label[i]);
				element->type = MIXER_ELEMENT_CAPTURE;
				element->rangemin = 0;
				element->rangemax = MAXLEVEL;
				element->id = i;
				if (SOUND_IOCTL(card->fd, MIXER_READ(i), &level) == 0) {
					int left = level & 0x7F;
					int right = (level >> 8) & 0x7F;
					element->volume = left > right ? left : right;
				}
				card->mixerElement = goc_arrayAdd( card->mixerElement, element );
			}
		}
	}

	return MIXER_CODE_OK;
}

static struct OssMixerCard* ossMixerCardAlloc() {
	OssMixerCard* card = malloc(sizeof(struct OssMixerCard));
	memset(card, 0, sizeof(struct OssMixerCard));
	card->mixerElement->freeElement = (MIXER_FREE_TYPE)&mixerElementFree;
	return card;
}

static struct OssMixerCard* createMixerCard(const char* devName) {
	int fd = open(devName, O_RDWR);
	if ( fd <= 0 ) {
		return NULL;
	}
	close( fd );

	struct OssMixerCard* pMixerCard = ossMixerCardAlloc();
	pMixerCard->name = goc_stringCopy(pMixerCard->name, devName);
	pMixerCard->physicalName = goc_stringCopy(pMixerCard->physicalName, devName);

	return pMixerCard;
}

// Currently not used IOCTL, but maybe in future
// SOUND_IOCTL(mixer_fd, SOUND_MIXER_READ_STEREODEVS, &stereodevs)
// SOUND_IOCTL(mixer_fd, SOUND_MIXER_WRITE_RECSRC, &recsrc)
// SOUND_IOCTL(mixer_fd, SOUND_MIXER_READ_RECSRC, &recsrc)

MixerHandler ossMixerAlloc() {
	MixerSystem* mixer = commonMixerAlloc();
	// setup functions
	mixer->mixerChangeVolume = ossMixerChangeVolume;
	mixer->mixerSelectCard = ossMixerSelectCard;
	mixer->mixerCardClose = ossMixerCardClose;
	// setup cards information
	for (int i=0; i<CARD_SIZE; i++) {
		if ( goc_isFileExists(cardDevices[i]) ) {
			struct OssMixerCard* mixerCard = createMixerCard( cardDevices[i] );
			if ( mixerCard ) {
				mixer->mixerCard = goc_arrayAdd( mixer->mixerCard, mixerCard );
			}
		}
	}

	return (MixerHandler)mixer;
}

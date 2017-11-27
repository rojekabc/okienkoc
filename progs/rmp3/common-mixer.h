#ifndef _COMMON_MIXER_H_
#	define _COMMON_MIXER_H_
#	include "mixer.h"

struct MixerSystem* commonMixerAlloc();

enum MixerCode commonMixerGetCardsAmount(struct MixerSystem* system, int* amount);
enum MixerCode commonMixerGetCardInfo(struct MixerSystem* system, int mixerId, MixerCard** mixerCard);
enum MixerCode commonMixerSelectCard(struct MixerSystem* system, int mixerId);
enum MixerCode commonMixerGetElementsAmount(struct MixerSystem* system, int* amount);
enum MixerCode commonMixerGetElementInfo(struct MixerSystem* system, int elementId, MixerElement** mixerElement);
enum MixerCode commonMixerSelectElement(struct MixerSystem* system, int elementId);

#endif

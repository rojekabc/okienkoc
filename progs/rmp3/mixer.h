#ifndef _MIXER_H_
#	define _MIXER_H_
#	include <stdint.h>
#	include <tools/array.h>

typedef uintptr_t MixerHandler;

typedef enum MixerElementType {
	MIXER_ELEMENT_UNKNOWN = -1,
       	MIXER_ELEMENT_PLAYBACK,
       	MIXER_ELEMENT_CAPTURE
} MixerElementType;

typedef enum MixerCode {
	MIXER_CODE_OK = 0,
	MIXER_CODE_WRONG_HANDLER,
	MIXER_CODE_WRONG_ARGUMENT,
	MIXER_CODE_OPERATION_NOT_ASSIGNED,
	MIXER_CODE_WRONG_ID,
	MIXER_CODE_NO_ACTIVE_CARD,
	MIXER_CODE_NO_ACTIVE_ELEMENT,
	MIXER_CODE_ERROR_SET_VOLUME,
	MIXER_CODE_CANNOT_OPEN_CARD,
	MIXER_CODE_CANNOT_SET_VOLUME
} MixerCode;

#define MIXER_FREE_TYPE void(*)(void*)

#define MIXER_ELEMENT_TYPE \
	char* name; \
	long rangemin; \
	long rangemax; \
	long volume; \
	enum MixerElementType type

typedef struct MixerElement {
	MIXER_ELEMENT_TYPE;
} MixerElement;

#define MIXER_CARD_TYPE \
	char* name; \
	char* physicalName; \
	GOC_Array* mixerElement; \
	MixerElement* active

typedef struct MixerCard {
	MIXER_CARD_TYPE;
} MixerCard;

#define MIXER_SYSTEM_TYPE \
	GOC_Array* mixerCard; \
	MixerCard* active; \
	enum MixerCode (*mixerGetCardsAmount)(struct MixerSystem*, int*); \
	enum MixerCode (*mixerGetCardInfo)(struct MixerSystem*, int, MixerCard**); \
	enum MixerCode (*mixerSelectCard)(struct MixerSystem*, int); \
	enum MixerCode (*mixerGetElementsAmount)(struct MixerSystem*, int*); \
	enum MixerCode (*mixerGetElementInfo)(struct MixerSystem*, int, MixerElement**); \
	enum MixerCode (*mixerSelectElement)(struct MixerSystem*, int); \
	enum MixerCode (*mixerChangeVolume)(struct MixerSystem*, int change, int* result); \
	enum MixerCode (*mixerCardClose)(struct MixerSystem*)

typedef struct MixerSystem {
	MIXER_SYSTEM_TYPE;
} MixerSystem;

#	define MIXER_ASSERT_NOT_NULL( pointer, code ) if ( pointer == NULL ) return code;
#	define MIXER_ASSERT_HANDLER( handler ) MIXER_ASSERT_NOT_NULL( (void*)handler, MIXER_CODE_WRONG_HANDLER )
#	define MIXER_ASSERT_OPERATION( operation ) MIXER_ASSERT_NOT_NULL( operation, MIXER_CODE_OPERATION_NOT_ASSIGNED )
#	define MIXER_ASSERT_ARGUMENT( argument ) MIXER_ASSERT_NOT_NULL( argument, MIXER_CODE_WRONG_ARGUMENT )

enum MixerCode mixerGetCardsAmount(MixerHandler handler, int* amount);
enum MixerCode mixerGetCardInfo(MixerHandler handler, int cardId, MixerCard** cardInfo);
enum MixerCode mixerSelectCard(MixerHandler handler, int cardId);
enum MixerCode mixerGetElementsAmount(MixerHandler handler, int* amount);
enum MixerCode mixerGetElementInfo(MixerHandler handler, int elementId, MixerElement** elementInfo);
enum MixerCode mixerSelectElement(MixerHandler handler, int elementId);
enum MixerCode mixerChangeVolume(MixerHandler handler, int change, int* result);

enum MixerCode mixerGetSelectedElement(MixerHandler handler, MixerElement** element);

MixerHandler mixerFree(MixerHandler mixer);

struct MixerSystem* mixerAlloc();
struct MixerCard* mixerCardAlloc();
struct MixerCard* mixerCardFree(struct MixerCard* mixerCard);
struct MixerElement* mixerElementAlloc();
struct MixerElement* mixerElementFree(struct MixerElement* mixerElement);

#endif

#ifndef _PLAYER_H_
#	define _PLAYER_H_
#	include <stdlib.h>

typedef enum PlayerCode {
	PLAYER_CODE_OK = 0,
	PLAYER_CODE_FAIL = 1
} PlayerCode;

PlayerCode playerInitialize();
PlayerCode playerOpenOutput();
PlayerCode playerCloseOutput();
PlayerCode playerPlay(int16_t* pBuf, size_t nBuf);

#endif

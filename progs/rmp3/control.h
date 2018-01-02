#ifndef _CONTROL_H_
#	define _CONTROL_H_

int controlInit(int (*)(unsigned int, const void*));
void controlCleanup();
int controlStart();
int controlEvent(unsigned int);

#endif

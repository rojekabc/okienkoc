#ifndef _GOC_TERM_H_
#define _GOC_TERM_H_
int goc_termInit(void);
int goc_termRestore(void);
int goc_getch();
int goc_isZnak();
int goc_getchar();
int goc_charWait();
#ifndef __MACH__
int goc_kbdRestoreMode(void);
int goc_scanToKey(int scan);
int goc_keyToEntry(int keyCode, int kbdTable);
int goc_keyToAscii(int keyCode);
void goc_ledTurnOn(int led);
void goc_ledTurnOff(int led);
int goc_isLedOn(int led);
void goc_ledSetFlag(int led);
void goc_clrLedFlag(int led);
int goc_isLedFlag(int led);
void goc_termSwitchTo(int num);
#endif
int goc_kbdScanMode(void);
int goc_screenGetWidth();
int goc_screenGetHeight();
int goc_kbdCanReadKbdMode();

#define goc_isKeyPressed(scan) (!(scan & 0x80))
#define goc_isKeyReleased(scan) (scan & 0x80)
#endif

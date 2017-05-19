#ifndef __MENU_H
#define __MENU_H
#include "hw_button.h"

extern const char* LcdOnStr;
extern const char* LcdOffStr;

void Menu_process(void);

uint8_t Menu_getOnFlag(void);
void Menu_initPara(void);

#endif

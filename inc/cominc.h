#ifndef __COMMINCV2_H
#define __COMMINCV2_H

#include <stdint.h>

#ifdef __dsb_vs
void __dsb(uint8_t);
#endif // __dsb_vs

#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_it.h"

#include "task.h"
#include "stm32_ub_led.h"
#include "st7587.h"
#include "Tsystick.h"
#include "hw_button.h"
#include "queue.h"
#include "uart1.h"
#include "graphics.h"
#include "menu.h"
#include "ringbuff.h"

#endif


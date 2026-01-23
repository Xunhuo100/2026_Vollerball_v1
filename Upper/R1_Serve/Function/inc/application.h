#ifndef __APPLICATION_H
#define __APPLICATION_H

#include "stm32f405xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "usart.h"

#include "PID.h"
#include "gom_protocol.h"
#include "crc_ccitt.h"

#include "upper.h"
#include "mygo.h"
#include "math.h"

#include "stdio.h"
#include "projdefs.h"

enum UpperState{
	Free = 0, Busy
};

extern float init_degree;

extern uint8_t hit_state;

extern uint8_t bow_state;

extern uint8_t hit_cmd;

extern uint8_t bow_cmd;

void _tim2_callback();

#endif
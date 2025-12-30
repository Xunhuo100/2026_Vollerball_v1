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

enum TaskMode
{
    Wait_Cmd = 0,
    Speed_Hit,     
    Accel_Hit      
}; 

extern float init_degree;
void _tim2_callback();

#endif
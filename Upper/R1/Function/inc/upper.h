#ifndef __UPPER_H
#define __UPPER_H

#include "stm32f405xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "usart.h"

#include "PID.h"
#include "gom_protocol.h"
#include "crc_ccitt.h"
#include "application.h"
#include "mygo.h"

extern PIDStructTypedef GoSpeedPID;
extern float resey_degree;
extern HAL_StatusTypeDef tx_res;
extern HAL_StatusTypeDef rx_res;
extern MotorCmd_t RiseUp;
extern MotorCmd_t FallDown;

float upper_go_reset();
void upper_go_init();

#endif
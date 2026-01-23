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

#include "freertos.h"
#include "projdefs.h"

#define UP 0
#define DOWN 1

#define CIRCLE 40 //6.33*2*pai

extern PIDStructTypedef GoSpeedPID;
extern HAL_StatusTypeDef tx_res;
extern HAL_StatusTypeDef rx_res;

extern uint8_t hit_state;
extern uint8_t isSpeed;
//extern uint8_t bow_state;

//extern float targetDegree;
extern float targetSpeed;

extern uint8_t hit_cmd;

extern MotorCmd_t Calmdown;
extern MotorCmd_t RiseUp;
extern MotorCmd_t FallDown;

void upper_go_init();
void upper_hit_work(float speed);
//void upper_bow_work(float degree);
uint8_t upper_hit_execute(float speed);
#endif
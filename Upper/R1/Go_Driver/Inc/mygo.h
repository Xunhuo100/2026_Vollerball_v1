#ifndef __MYGO_H
#define __MYGO_H

#include "stm32f405xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "usart.h"

#include "PID.h"
#include "gom_protocol.h"
#include "crc_ccitt.h"

#include "upper.h"
#include "application.h"

typedef struct
{
	uint8_t id;
	MotorCmd_t cmd;
	MotorData_t data;
	PIDStructTypedef speed_pid;
	uint8_t tx_res;
	uint8_t rx_res;
} myGo;


void mygo_init(myGo* m,uint8_t id,float speed_kp,float speed_ki,float speed_kd,float limit_output);

void mygo_output(myGo*m);

void mygo_speed_output(myGo*m,float tgt);

#endif
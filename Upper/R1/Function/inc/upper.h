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

#define USE_CAN_1

#include "CAN_Basic.h"
#include "DJ_Motor.h"


extern PIDStructTypedef GoSpeedPID;
extern HAL_StatusTypeDef tx_res;
extern HAL_StatusTypeDef rx_res;
extern MotorCmd_t RiseUp;
extern MotorCmd_t FallDown;

extern MotorTypeDef BowMotor;
extern MotorTypeDef SlewMotor;
extern MOTORPIDstructTypedef BowMotorPID;
extern MOTORPIDstructTypedef SlewMotorPID;

extern uint8_t hit_state;
//extern uint8_t bow_state;

//extern float targetDegree;
extern float targetSpeed;

extern uint8_t hit_cmd;
extern uint8_t bow_cmd;

float upper_go_reset();
void upper_go_init();
void upper_3508_init();
void upper_hit_work(float speed);
//void upper_bow_work(float degree);
uint8_t upper_hit_execute(float speed);
void upper_bow_slew_execute(float bow_deg,float slew_deg);
void slew_motor_PID_init(MOTORPIDstructTypedef* MotorPID);
void bow_motor_PID_init(MOTORPIDstructTypedef* MotorPID);

#endif
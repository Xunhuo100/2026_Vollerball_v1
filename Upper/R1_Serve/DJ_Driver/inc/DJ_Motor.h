#ifndef _DM_CAN_H
#define _DM_CAN_H

#include "stm32f4xx_hal.h"
#include "CAN_Basic.h"

#define MyMotor_CAN						MYCAN1 //修改此处后仍需在CAN_Basic修改
#define MyMotor_CAN_ID					0x03

typedef enum
{
	IDLE,//闲置
	MOTOR_POSITION,//位置环
	MOTOR_SPEED,//速度环
	MOTOR_CURRENT,//电流环
	MOTOR_ERROR,
}DJMotor_mode;//大疆电机运行模式

typedef struct
{
	uint8_t enable;
	DJMotor_mode mode;
	
	short encoder;					//电调读数，角度值
	short encoderPre;				// 上一时刻角度值
	float PrePosition;			// 上一时刻位置
	
	float SpeedTorlent;
	float StartPosition;
	
	float SpeedMax;					// 可用来记录速度峰值
	float CurrentMAX;				// 用来记录电流峰值
	
	uint32_t ID;						//电机ID
	int16_t DIR;						//电机运行方向
	
	float PositionExpected;
	float PositionMeasure;
	float SpeedExpected;
	float SpeedMeasure;
	float CurrentExpected;
	float CurrentMeasure;
	
} MotorTypeDef;


extern MotorTypeDef MyMotor;

void dj_can_transmit_eid(uint16_t id, uint8_t bus_id, uint8_t *data, uint8_t len);
void dj_can_set(uint16_t, uint8_t,int16_t, int16_t, int16_t, int16_t);
void dj_can_set_can2(uint16_t controller_id, int16_t current1, int16_t current2, int16_t current3, int16_t current4);
void Process_DJFrame_CAN(CANFrame *Frame_Process);
uint8_t motor_ready(MotorTypeDef *motor,uint8_t *count,uint8_t boundary);
#endif

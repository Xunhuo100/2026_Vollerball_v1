#include "DJ_Motor.h"
#include "can.h"
#include "cmsis_os2.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "upper.h"

#define USE_CAN_1

MotorTypeDef MyMotor;

/***********************************Dj消息发送函数***********************************/
void dj_can_transmit_eid(uint16_t id, uint8_t bus_id, uint8_t *data, uint8_t len)
{
  uint32_t Tx_MailBox;
	uint16_t can_timeout = 0;
	uint32_t status = HAL_ERROR;

	if (len > 8)
	{
			len = 8;
	}

	CAN_TxHeaderTypeDef TxMessage;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.StdId = id;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = len;

	#ifdef USE_CAN_1
	if (bus_id == MYCAN1)
	{
		status =  HAL_CAN_AddTxMessage(&hcan1, &TxMessage,data, &Tx_MailBox);
    while (status != HAL_OK && (can_timeout <= 1500)) //一定要加超时
		{
			uint32_t can_error = HAL_CAN_GetError(&hcan1);
			if (can_error & (HAL_CAN_ERROR_TX_TERR0 | HAL_CAN_ERROR_TX_TERR1 | HAL_CAN_ERROR_TX_TERR2))
      {
        // 确定具体哪个邮箱失败
        uint32_t failed_mb = 0;
				if (can_error & HAL_CAN_ERROR_TX_TERR0) failed_mb = CAN_TX_MAILBOX0;
				else if (can_error & HAL_CAN_ERROR_TX_TERR1) failed_mb = CAN_TX_MAILBOX1;
				else if (can_error & HAL_CAN_ERROR_TX_TERR2) failed_mb = CAN_TX_MAILBOX2;

				HAL_CAN_AbortTxRequest(&hcan1, failed_mb);
			}
			if(can_timeout > 1000)
			{
				HAL_CAN_AbortTxRequest(&hcan1, CAN_TX_MAILBOX0);
				HAL_CAN_AbortTxRequest(&hcan1, CAN_TX_MAILBOX1);
				HAL_CAN_AbortTxRequest(&hcan1, CAN_TX_MAILBOX2);
			}
			status = HAL_CAN_AddTxMessage(&hcan1, &TxMessage,data, &Tx_MailBox);
			can_timeout++;
		}
	}
	#endif
	
	#ifdef USE_CAN_2
	if (bus_id == MYCAN2)
	{
		status =  HAL_CAN_AddTxMessage(&hcan2, &TxMessage,data, &Tx_MailBox);
		while (status != HAL_OK && (can_timeout <= 1500)) //一定要加超时
		{
			uint32_t can_error = HAL_CAN_GetError(&hcan2);
			if (can_error & (HAL_CAN_ERROR_TX_TERR0 | HAL_CAN_ERROR_TX_TERR1 | HAL_CAN_ERROR_TX_TERR2))
			{
				// 确定具体哪个邮箱失败
				uint32_t failed_mb = 0;

				if (can_error & HAL_CAN_ERROR_TX_TERR0) failed_mb = CAN_TX_MAILBOX0;
				else if (can_error & HAL_CAN_ERROR_TX_TERR1) failed_mb = CAN_TX_MAILBOX1;
				else if (can_error & HAL_CAN_ERROR_TX_TERR2) failed_mb = CAN_TX_MAILBOX2;

				HAL_CAN_AbortTxRequest(&hcan2, failed_mb);
			}
			if(can_timeout > 1000)
			{
				HAL_CAN_AbortTxRequest(&hcan2, CAN_TX_MAILBOX0);
				HAL_CAN_AbortTxRequest(&hcan2, CAN_TX_MAILBOX1);
				HAL_CAN_AbortTxRequest(&hcan2, CAN_TX_MAILBOX2);
			}
			status =  HAL_CAN_AddTxMessage(&hcan2, &TxMessage,data, &Tx_MailBox);
			can_timeout++;
		}
	}
	#endif
	
}

void dj_can_set(uint16_t controller_id, uint8_t bus_id, int16_t current1, int16_t current2, int16_t current3, int16_t current4)
{

	uint8_t buffer[8];
	uint16_t id = 0;

	if (controller_id <= 0x08)
	{
		if (controller_id >= 0x05)
		{
			id = 0x1FF;
		}
		else
		{
			id = 0x200;
		}
	}

	buffer[0] = (current1>>8)&0xFF;
	buffer[1] = current1&0xFF;
	buffer[2] = (current2>>8)&0xFF;
	buffer[3] = current2&0xFF;
	buffer[4] = (current3>>8)&0xFF;
	buffer[5] = current3&0xFF;
	buffer[6] = (current4>>8)&0xFF;
	buffer[7] = current4&0xFF;

	dj_can_transmit_eid(id, bus_id, buffer, 8);
}

/***********************************Dj消息接收函数***********************************/
float Get_RM2006_Distance(MotorTypeDef Motor)
{
	int Distance = Motor.encoder - Motor.encoderPre;

	if (abs(Distance) > 4000)
	{
		Distance = Distance - Distance/abs(Distance) * 8192;
	}

	return ((float)Distance*360.0f/36.0f/8192.0f);
}

float Get_RM3508_Distance(MotorTypeDef Motor)
{
	int Distance = Motor.encoder - Motor.encoderPre;

	if (abs(Distance) > 4000)
	{
		Distance = Distance - Distance/abs(Distance) * 8192;
	}

	return ((float)Distance*360.0f/(3591.0f/187.0f)/8192.0f);
}

short temp_angle = 0;
short temp_speed = 0;
short temp_current = 0;
void Process_DJFrame_CAN(CANFrame *Frame_Process)
{
	uint32_t id = Frame_Process->Id.StdID & 0x00F;
	temp_angle      = (short)(Frame_Process ->Data.uchars[0]<<8 | Frame_Process ->Data.uchars[1]);
	temp_speed      = (short)(Frame_Process ->Data.uchars[2]<<8 | Frame_Process ->Data.uchars[3]);
	temp_current    = (short)(Frame_Process ->Data.uchars[4]<<8 | Frame_Process ->Data.uchars[5]);
	
	switch (id)
	{
		case MyMotor_CAN_ID:
			
			BowMotor.ID = MyMotor_CAN_ID;
			if (BowMotor.enable == 0)
			{
					BowMotor.enable = 1;
			}

			if (BowMotor.encoderPre == 0 && BowMotor.encoder == 0)
			{
					BowMotor.encoderPre = BowMotor.encoder = temp_angle;
			}
			else
			{
					BowMotor.encoderPre = BowMotor.encoder;
					BowMotor.encoder = temp_angle;
			}
			BowMotor.PrePosition = BowMotor.PositionMeasure;
			BowMotor.PositionMeasure += Get_RM3508_Distance(BowMotor);
			BowMotor.SpeedMeasure = (float)(temp_speed) *187.f/1591.f;
			BowMotor.CurrentMeasure = (float)(temp_current);
			
			break;
			
		default:
				break;

	}
}

#define MOTOR_STABLE_THRESHOLD  1.0f
uint8_t motor_ready(MotorTypeDef *motor, uint8_t *cnt, uint8_t boundary)
{
	uint8_t temp = *cnt;

	// 判断位置是否稳定
	if (fabsf(motor->PositionMeasure - motor->PrePosition) < MOTOR_STABLE_THRESHOLD)
	{
		if (temp < 255)  // 防止溢出
				temp++;
	}
	else
	{
		temp = (temp < 2) ? 0 : (temp >> 1);  // 减半，快速衰减
	}

	// 判断是否达到稳定阈值
	if (temp >= boundary) 
	{
		*cnt = 0;
		return 1;  // 电机已就绪
	}
	else
	{
		*cnt = temp;
		return 0;  // 未就绪
	}
}

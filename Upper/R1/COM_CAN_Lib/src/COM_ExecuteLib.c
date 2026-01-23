#include "COM_Basic.h"
#include "COM_Proplist.h"
#include "HUST_Math_Lib.h"
#include "main.h"
#include "usart.h"
#include "upper.h"
#include "application.h"

float com_speed;
uint8_t isSpeedSet = 0;

int fputc(int ch, FILE *stream)
{
	HAL_UART_Transmit(&huart2,(uint8_t*)&ch,1,HAL_MAX_DELAY);
	return ch;
}
///*******************************MYUSART1���պ���***********************************/

//void setVelocity(COMFrame * Frame)
//{
//}

///*******************************MYUSART2���պ���***********************************/
//和底盘(蓝牙通讯或其他)
void com_upper_spd_set(COMFrame* frame)
{
	com_speed = frame->Data.floats_ts[0];
	isSpeedSet = 1;
}
	
void com_upper_hit(COMFrame* frame)
{
	if(isSpeedSet == 0)
		upper_hit_execute(40);
	else
		upper_hit_execute(com_speed);
	
	isSpeedSet = 0;
}
	
void com_test(COMFrame* frame)
{
	uint8_t t = frame->Data.uint8_ts[0];
	HAL_UART_Transmit(&huart2,(uint8_t*)&t,1,HAL_MAX_DELAY);

}

void com_upper_deg_set(COMFrame* frame)
{
	float bow_deg = (frame->Data.floats_ts[0] >45.0)?45.0:frame->Data.floats_ts[0];
	bow_deg = bow_deg*41.f/19.f;

	float slew_deg = frame->Data.floats_ts[1];
	if(slew_deg>45.f)slew_deg = 45.f;
	else if(slew_deg<-45.f)slew_deg = - 45.f; 
	slew_deg = slew_deg*87.f/19.f;
	
	upper_bow_slew_execute(bow_deg,slew_deg);
}

///*******************************MYUSART3���պ���***********************************/
//和PC通讯

///*******************************MYUSART4���պ���***********************************/



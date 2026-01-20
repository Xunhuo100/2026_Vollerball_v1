#include "COM_Basic.h"
#include "COM_Proplist.h"
#include "HUST_Math_Lib.h"
#include "main.h"

///*******************************MYUSART1���պ���***********************************/

//void setVelocity(COMFrame * Frame)
//{
//}

///*******************************MYUSART2���պ���***********************************/
//和底盘(蓝牙通讯或其他)
void com_upper_deg_set(COMFrame* frame){}
	
void com_upper_spd_set(COMFrame* frame){}
	
void com_upper_hit(COMFrame* frame){}
	
void com_test(COMFrame* frame)
{
	float test[4];
	test[0] = frame->Data.floats_ts[0];
	test[1] = frame->Data.floats_ts[1];
	test[2] = frame->Data.floats_ts[2];
	test[3] = frame->Data.floats_ts[3];
	
	if(test[0]-1.14<1e-6&&test[1]-5.14<1e-6&&test[2]-19.19<1e-6&&test[3]-8.10<1e-6)
	{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET);
	}
}

///*******************************MYUSART3���պ���***********************************/
//和PC通讯
///*******************************MYUSART4���պ���***********************************/



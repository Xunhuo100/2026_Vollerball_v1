#ifndef ROBOTCOM_PROPLIST_H_
#define ROBOTCOM_PROPLIST_H_

#include "COM_basic.h"

extern COMFunDict g_My_USART1_Prop_Array[];

extern COMFunDict g_My_USART2_Prop_Array[];

extern COMFunDict g_My_USART3_Prop_Array[];

extern COMFunDict g_My_USART4_Prop_Array[];

extern const uint8_t g_My_USART1_Prop_Count;

extern const uint8_t g_My_USART2_Prop_Count;

extern const uint8_t g_My_USART3_Prop_Count;

extern const uint8_t g_My_USART4_Prop_Count;

/* MYUSART1 part */
/******************************prop属性值,8位*********************************/
//#define SETVELOCITY 0x60


/******************************接收函数*********************************/
/******************************发送函数*********************************/


/******************************prop,属性值8位*********************************/
#define UPPER_DEG_SET 0x17 //设置俯仰和回转的角度
#define UPPER_SPD_SET 0x32 //设置下一次击打的速度
#define UPPER_HIT     0xA2 //击球
#define TEST          0xDA

/******************************接收函数*********************************/
void com_upper_deg_set(COMFrame* frame);
	
void com_upper_spd_set(COMFrame* frame);
	
void com_upper_hit(COMFrame* frame);
	
void com_test(COMFrame* frame);

/******************************发送函数*********************************/


/* MYUSART3 part */
/******************************prop,属性值8位*********************************/
#define LOOP_TO_PC		0xF2
//接收


//发送

/******************************接收函数*********************************/

/******************************发送函数*********************************/


#endif


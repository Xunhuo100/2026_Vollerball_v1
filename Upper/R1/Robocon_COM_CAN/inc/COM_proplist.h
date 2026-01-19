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
#define SETVELOCITY 0x60
#define LOOPTOPC		0xF2
/******************************接收函数*********************************/
void setVelocity(COMFrame *);
/******************************发送函数*********************************/

void ChassisToPc(void);

/* MYUSART3 part */
/******************************prop,属性值8位*********************************/
/* 手操功能命令 */


/******************************接收函数*********************************/
/* 手操功能函数 */



/******************************发送函数*********************************/


/* MYUSART4 part */
/******************************prop,属性值8位*********************************/
//接收


//发送

/******************************接收函数*********************************/

/******************************发送函数*********************************/


#endif


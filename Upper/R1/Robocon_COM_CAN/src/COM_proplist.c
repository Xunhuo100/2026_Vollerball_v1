#include "COM_proplist.h"

//pc
COMFunDict g_My_USART1_Prop_Array[] = {	
	{NULL,NULL},
	{SETVELOCITY,setVelocity},
};
//¶¨Î»°å
COMFunDict g_My_USART2_Prop_Array[] = {
	{NULL,NULL},
};

COMFunDict g_My_USART3_Prop_Array[] = {
	{NULL,NULL},
};
//Ê¾²¨Æ÷
COMFunDict g_My_USART4_Prop_Array[] = {
	{NULL,NULL},
};

const uint8_t g_My_USART1_Prop_Count = sizeof(g_My_USART1_Prop_Array) /
                                       sizeof(g_My_USART1_Prop_Array[0]);
const uint8_t g_My_USART2_Prop_Count = sizeof(g_My_USART2_Prop_Array) /
                                       sizeof(g_My_USART2_Prop_Array[0]);
const uint8_t g_My_USART3_Prop_Count = sizeof(g_My_USART3_Prop_Array) /
                                       sizeof(g_My_USART3_Prop_Array[0]);
const uint8_t g_My_USART4_Prop_Count = sizeof(g_My_USART4_Prop_Array) /
                                       sizeof(g_My_USART4_Prop_Array[0]);


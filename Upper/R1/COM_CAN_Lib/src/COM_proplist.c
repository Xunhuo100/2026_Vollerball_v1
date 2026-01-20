#include "COM_proplist.h"

COMFunDict g_My_USART1_Prop_Array[] = {	
	{NULL,NULL},
//	{SETVELOCITY,setVelocity},
};
COMFunDict g_My_USART2_Prop_Array[] = {
	{NULL,NULL},
	{TEST,com_test},
	{UPPER_DEG_SET,com_upper_deg_set},
	{UPPER_SPD_SET,com_upper_spd_set},
	{UPPER_HIT,com_upper_hit}
};

COMFunDict g_My_USART3_Prop_Array[] = {
	{NULL,NULL},
};

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


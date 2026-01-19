///**
// ******************************************************************************
// * @file    RobotCOM_ExecuteLib.c
// * @author  Robocon
// * @brief   �������ṩ�˴���Ϣ���������Ķ���:
// *           - �Զ����ŶӴ���Э��֡��Ϣ��������
// *  @verbatim
// *          ��������������Ӧ����Ϣ�������������Ӻ������Ӧ��proplist.c����
// *  @endverbatim
// ******************************************************************************
// */
//#include "COM_Basic.h"
//#include "COM_Proplist.h"
//#include "Task_Chassis.h"
//#include "HUST_Math_Lib.h"
//#include "main.h"

///*******************************MYUSART1���պ���***********************************/

//void setVelocity(COMFrame * Frame)
//{
//    float temp_x=Frame->Data.floats_ts[0];
//    float temp_y=Frame->Data.floats_ts[1];                                      //mm/s
//    float temp_alpha=Frame->Data.floats_ts[2];                              //DEG/s
//    uint8_t chassis_mode = Frame->Data.uint8_ts[12];
//	uint8_t temp_jump_flag = Frame->Data.uint8_ts[13];
//	uint8_t temp_dribble_flag = Frame->Data.uint8_ts[14];

//    g_Chassis_Instance.AimState.Vx  = temp_x ;
//    g_Chassis_Instance.AimState.Vy  = temp_y ;
//    g_Chassis_Instance.AimState.w   = temp_alpha ;
//    g_Chassis_Instance.AimState.V   = sqrtf(powf(g_Chassis_Instance.AimState.Vx, 2) + powf(g_Chassis_Instance.AimState.Vy, 2));
//    g_Chassis_Instance.mode         = chassis_mode;
//	
//	if(temp_jump_flag == 1)
//	{
//		my_jump_flag = 1;
//	}
//	if(temp_dribble_flag == 1)
//	{
//		dribble = 1;
//	}
//	
//}

///*******************************MYUSART2���պ���***********************************/

///*******************************MYUSART3���պ���***********************************/

///*******************************MYUSART4���պ���***********************************/



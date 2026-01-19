/**
 ******************************************************************************
 * @file    RobotCOM_CommandLib.c
 * @author  Robocon
 * @brief   本代码提供了利用串口通信发送消息功能的封装:
 *           - 根据每年机器人的具体需求，自行定义相应的发送函数
 *  @verbatim
 *
 *  @endverbatim
 ******************************************************************************
 */
#include "COM_Basic.h"
#include "COM_proplist.h"
#include "Task_Chassis.h"
#include "HUST_Math_Lib.h"
#include "usart.h"

// MYUSART1 : PC
// MYUSART4 : 手柄
/***************************************MYUSART1发送函数******************************************/
void ChassisToPc(void)
{
	COMFrame SendFrame;

	SendFrame.Length = 1;
	SendFrame.Prop = LOOPTOPC;	
	SendFrame.Data.uint8_ts[0] = 1;
	Send_Frame_COM(&SendFrame, MYUSART1);//发给PC
}
/***************************************MYUSART2发送函数******************************************/

/***************************************MYUSART3发送函数******************************************/

/***************************************MYUSART4发送函数******************************************/










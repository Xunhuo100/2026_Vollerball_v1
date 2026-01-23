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
//


///*******************************MYUSART3���պ���***********************************/
//和PC通讯

///*******************************MYUSART4���պ���***********************************/



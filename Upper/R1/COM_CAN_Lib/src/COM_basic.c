#include "COM_basic.h"
#include "stdio.h"
#include "usart.h"
#include "COM_proplist.h"

COMFrame        g_My_USART1_COM_Rec;
COMRecStatus    g_My_USART1_COM_Rec_Status;
COMFrame        g_My_USART2_COM_Rec;
COMRecStatus    g_My_USART2_COM_Rec_Status;
COMFrame        g_My_USART3_COM_Rec;
COMRecStatus    g_My_USART3_COM_Rec_Status;
COMFrame        g_My_USART4_COM_Rec;
COMRecStatus    g_My_USART4_COM_Rec_Status;


//int fputc(int ch,FILE* f)
//{
//	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
//	return ch;
//}

void (*Find_Method_COM(uint8_t Prop, COMFunDict* Props_Array, 
    uint8_t Props_Count))(COMFrame* Frm);

void Send_Frame_COM(COMFrame* Send_Frame,int USARTx) 
{
    uint8_t *Send_Buf_Temp;
    uint8_t Send_Buf[0xE0];
    uint8_t Length = Send_Frame->Length;
    uint8_t LengthTemp = Length;

    Send_Buf_Temp = &Send_Buf[0];

    *Send_Buf_Temp++ = 0XEE;
    *Send_Buf_Temp++ = Length;
    *Send_Buf_Temp++ = Send_Frame->Prop;

    for (int i = 0; i < Length; i++) 
    {
        uint8_t Temp = Send_Frame->Data.uint8_ts[i];
        if (Temp == 0XEE) 
        {
            *Send_Buf_Temp++ = 0XEE;
            LengthTemp++;
        }
        *Send_Buf_Temp++ = Temp;
    }
    *Send_Buf_Temp++  = 0XEE;
    LengthTemp       += 4;
    Send_Buf[1]       = LengthTemp;
		switch(USARTx)
		{
			case MYUSART1:
		    //HAL_UART_Transmit(&huart1,&Send_Buf[0],LengthTemp,10);
			  break;
		  case MYUSART2:                                 
		    HAL_UART_Transmit(&huart2,&Send_Buf[0],LengthTemp,10);
			  break;
			case MYUSART3:
		    HAL_UART_Transmit(&huart4,&Send_Buf[0],LengthTemp,10);
			  break;
			case MYUSART4:
		    HAL_UART_Transmit(&huart6,&Send_Buf[0],LengthTemp,10);
			  break;
		}
	}

void Process_Frame_COM(COMFrame* Rec_Frame, int USARTx) 
{
    void (*Fun)(COMFrame * Frm);
    COMFunDict* Props_Array;
    uint8_t Props_Count;

    if (USARTx == MYUSART1) 
    {
        Props_Array = g_My_USART1_Prop_Array;
        Props_Count = g_My_USART1_Prop_Count;
    } 
    else if (USARTx == MYUSART2) 
    {
        Props_Array = g_My_USART2_Prop_Array;
        Props_Count = g_My_USART2_Prop_Count;
    }
    else if (USARTx == MYUSART3) 
    {
        Props_Array = g_My_USART3_Prop_Array;
        Props_Count = g_My_USART3_Prop_Count;
    } 
    else if (USARTx == MYUSART4) 
    {
        Props_Array = g_My_USART4_Prop_Array;
        Props_Count = g_My_USART4_Prop_Count;
    } 
    else 
        return;

    if ((Fun = Find_Method_COM(Rec_Frame->Prop, Props_Array, Props_Count)) != 0) 
        Fun(Rec_Frame);
}

void (*Find_Method_COM(uint8_t Prop, COMFunDict* Props_Array, 
    uint8_t Props_Count))(COMFrame* Frm) 
{
    uint8_t i;

    for (i = 0; i < Props_Count; ++i) 
		{
        if (Props_Array[i].Prop == Prop) 
            return Props_Array[i].Fun;
    }
    return 0;
}
	COMFrame* pFrame;
void Get_Frame_COM(uint8_t Data, int USARTx) 
{
    COMRecStatus* pRecInfo;
    
    uint8_t Rec_Status, pre_Data;

    if (USARTx == MYUSART1) 
    {
        pRecInfo       = &g_My_USART1_COM_Rec_Status;
        pFrame         = &g_My_USART1_COM_Rec;
    } 
    else if (USARTx == MYUSART2) 
    {
        pRecInfo       = &g_My_USART2_COM_Rec_Status;
        pFrame         = &g_My_USART2_COM_Rec;
    }
    else if (USARTx == MYUSART3) 
    {
        pRecInfo       = &g_My_USART3_COM_Rec_Status;
        pFrame         = &g_My_USART3_COM_Rec;
    }
    else if (USARTx == MYUSART4) 
    {
        pRecInfo       = &g_My_USART4_COM_Rec_Status;
        pFrame         = &g_My_USART4_COM_Rec;
    }
    pre_Data   = pRecInfo->preData;
    Rec_Status = pRecInfo->RecStatus;

    if (Data == COM_FRAME_MARK && Rec_Status != TAIL_CONFIRM) 
    {
        if (pre_Data != COM_FRAME_ESCAPE) 
        {
            pRecInfo->RecStatus = HEAD_DETECT;
            Rec_Status = HEAD_DETECT;
        } 
        else 
        {
            if (pFrame->Length > 0)
                pFrame->Length--;
        }
    }

    switch (Rec_Status) 
    {
        case HEAD_DETECT:
            pRecInfo->RecStatus = LENGTH_ACQUIRE;
            break;

        case LENGTH_ACQUIRE:
            if (Data < 4) 
                pRecInfo->RecStatus = HEAD_DETECT;
            else 
            {
                pRecInfo->RecLength = Data;
                pFrame->Length = 0;
                pRecInfo->RecStatus = PROP_ACQUIRE;
            }
            break;

        case PROP_ACQUIRE:
            pFrame->Prop = Data;
            if (pRecInfo->RecLength == 4)
                pRecInfo->RecStatus = TAIL_CONFIRM;
            else 
                pRecInfo->RecStatus = DATA_ACQUIRE;
            break;

        case DATA_ACQUIRE:
            pRecInfo->RecLength--;
            pFrame->Data.uint8_ts[pFrame->Length++] = Data;
            if (pRecInfo->RecLength == 4)
                pRecInfo->RecStatus = TAIL_CONFIRM;
            break;

        case TAIL_CONFIRM:
            if (COM_FRAME_MARK == Data)
                Process_Frame_COM(pFrame,USARTx);
            pRecInfo->RecStatus = HEAD_DETECT;
            break;
        
        default:
            break;
    }
    pRecInfo->preData = Data;
}
	
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	if(huart==&huart1)
//	{
//		Get_Frame_COM(data_uart_rec,MYUSART1);
//	}
//	if(huart==&huart2)
//	{
//		Get_Frame_COM(data_uart_rec,MYUSART2);
//	}
//	if(huart==&huart4)
//	{
//		Get_Frame_COM(data_uart_rec,MYUSART3);
//	}
	if(huart==&huart6)
	{
		Get_Frame_COM(data_uart_rec,MYUSART4);
	}
	while(HAL_UART_Receive_IT(huart,&data_uart_rec,1)!=HAL_OK)
	{
		 huart->RxState = HAL_UART_STATE_READY;
      __HAL_UNLOCK(huart);
	}
	//HAL_UART_Receive_IT(&huart2,&data_uart_rec,1);
	HAL_UART_Receive_IT(&huart6,&data_uart_rec,1);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if(__HAL_UART_GET_FLAG(huart,UART_FLAG_ORE) != RESET) 
    {
        __HAL_UART_CLEAR_OREFLAG(huart);
        HAL_UART_Receive_IT(huart,&data_uart_rec,1);
    }
}


#include "CAN_Basic.h"
#include "DJ_Motor.h"

extern osMessageQueueId_t CANSendDataHandle;

#ifdef USE_CAN_1
/**
  * @brief  初始化 CAN1 滤波器（32位掩码模式，接收所有帧）
  */
void CAN1_Filter_Init(void)
{
	CAN_FilterTypeDef sFilterConfig;

	sFilterConfig.FilterBank = 0;                          // 滤波器编号 0
	sFilterConfig.SlaveStartFilterBank = 14;               // CAN2 占用滤波器起始编号（适用于双 CAN 主从结构）
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;      // 掩码模式
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;     // 32位尺度
	sFilterConfig.FilterIdHigh = 0x0000;                   // 期望的 ID 高位（此处不限制）
	sFilterConfig.FilterIdLow = 0x0000;                    // 期望的 ID 低位
	sFilterConfig.FilterMaskIdHigh = 0x0000;               // 掩码高位（0 表示不关心）
	sFilterConfig.FilterMaskIdLow = 0x0000;                // 掩码低位
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0; // 分配到 FIFO0
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;    // 启用该滤波器

	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
	{
			Error_Handler();
	}
	
}
/**
  * @brief  启动 CAN1 并启用相关中断
  */
void CAN1_Start(void)
{
	// 1. 配置滤波器
	CAN1_Filter_Init();

	// 2. 启动CAN1外设
	if ((HAL_CAN_Start(&hcan1))!=HAL_OK) 
	{
      Error_Handler();
	}
	
	// 3. 启用 FIFO0 消息挂起中断（有数据来时触发）
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
			Error_Handler();
	}

	// 4. 启用错误中断（可选，用于监控总线错误）
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_ERROR) != HAL_OK)
	{
			Error_Handler();
	}
}
#endif

#ifdef USE_CAN_2

/**
  * @brief  初始化 CAN2 滤波器（32位掩码模式，接收所有帧）
  */
void CAN2_Filter_Init(void)
{
    CAN_FilterTypeDef sFilterConfig;

    sFilterConfig.FilterBank = 14;                         // CAN2 使用滤波器编号 14
    sFilterConfig.SlaveStartFilterBank = 14;               // 双 CAN 架构中，CAN2 从滤波器 14 开始
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;      // 掩码模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;     // 32位滤波器尺度
    sFilterConfig.FilterIdHigh = 0x0000;                   // 期望 ID 高位（不限制）
    sFilterConfig.FilterIdLow = 0x0000;                    // 期望 ID 低位
    sFilterConfig.FilterMaskIdHigh = 0x0000;               // 掩码高位（0 表示不关心）
    sFilterConfig.FilterMaskIdLow = 0x0000;                // 掩码低位
    sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO1; // 分配到 FIFO1
    sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;    // 启用滤波器

    if (HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  启动 CAN2 并启用相关中断
  */
void CAN2_Start(void)
{
    // 1. 配置滤波器
    CAN2_Filter_Init();
		HAL_Delay(10);
    // 2. 启动 CAN2 外设
    if (HAL_CAN_Start(&hcan2) != HAL_OK)
    {
        Error_Handler();
    }

    // 3. 启用 FIFO1 消息挂起中断（有数据到达时触发）
    if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
    {
        Error_Handler();
    }

    // 4. 启用错误中断（用于监控总线错误）
    if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_ERROR) != HAL_OK)
    {
        Error_Handler();
    }
}

#endif

/**
	* @brief  发送函数，裸机/RTOS均可调用(一般情况下不使用)
  */
void Send_Frame_CAN(CANFrame* Frame_Send, int canx)
{
	CAN_TxHeaderTypeDef TxMessage;
	uint32_t  Transmit_Mailbox;
	uint8_t Transmit_Staus = HAL_ERROR;
	uint8_t *Data_Send;
	uint16_t Time_Out_Count   = 0;
	Data_Send  = &(Frame_Send->Data.uchars[0]);

	// 设置 ID
	if (Frame_Send->IDE == CAN_ID_EXT) 
	{
		Frame_Send->Id.all = Frame_Send->Id.ExtID.DesDeviceId + 
												(Frame_Send->Id.ExtID.Property<<8) + 
												(Frame_Send->Id.ExtID.SrcDeviceId<<16) + 
												(Frame_Send->Id.ExtID.Priority<<24) + 
												(Frame_Send->Id.ExtID.Permit<<28);
		
		TxMessage.ExtId = Frame_Send->Id.all & 0x0fffffff;  //扩展ID最多19位
		TxMessage.IDE   = CAN_ID_EXT;
	}
	else 
	{
		Frame_Send->Id.all =Frame_Send->Id.StdID;
		TxMessage.StdId = Frame_Send->Id.all & 0x000007ff;	// 标准ID最多11位
		TxMessage.IDE   = CAN_ID_STD;
	}
	
	// 限制数据长度
	if (Frame_Send->Length > 8)
	{
		Frame_Send->Length = 8;
	}
	
	// 设置 RTR 和 DLC
	if (Frame_Send->isRemote)
	{
		TxMessage.RTR = CAN_RTR_REMOTE;
		TxMessage.DLC = 0;
	}
	else
	{
		TxMessage.RTR = CAN_RTR_DATA;
		TxMessage.DLC = Frame_Send->Length;
	}

	// 不启用时间戳功能
	TxMessage.TransmitGlobalTime=DISABLE;

	while ((Transmit_Staus != HAL_OK)&&(Time_Out_Count++ != 0xFF))
	{
		#ifdef USE_CAN_1
		if (canx == MYCAN1)
		{
			Transmit_Staus = HAL_CAN_AddTxMessage(&hcan1, &TxMessage, Data_Send, &Transmit_Mailbox);
		}
		#endif
		
		#ifdef USE_CAN_2
		if (canx == MYCAN2)
		{
			Transmit_Staus = HAL_CAN_AddTxMessage(&hcan2, &TxMessage, Data_Send, &Transmit_Mailbox);
		}
		#endif
		
	}
}

/**
	* @brief  发送函数，采用队列形式RTOS仅可调用(VESC等使用此函数，大疆电机不使用)
  */
//void DeQueueCanMessage(uint16_t *cancnt)
//{
//	uint16_t can_timeout = 0;
//	CANFrame TempCANFrame;
//	uint32_t TxMailbox;
//	
//	if (osMessageQueueGet(CANSendDataHandle, &TempCANFrame, NULL, 0) == osOK)
//	{
//		CAN_TxHeaderTypeDef txHeader;
//		if (TempCANFrame.IDE == CAN_ID_EXT)
//		{
//			TempCANFrame.Id.all = TempCANFrame.Id.ExtID.DesDeviceId + 
//													(TempCANFrame.Id.ExtID.Property<<8) + 
//													(TempCANFrame.Id.ExtID.SrcDeviceId<<16) + 
//													(TempCANFrame.Id.ExtID.Priority<<24) + 
//													(TempCANFrame.Id.ExtID.Permit<<28);
//			txHeader.ExtId = TempCANFrame.Id.all & 0x0fffffff;  //29?ID?????28??
//			txHeader.IDE   = CAN_ID_EXT;
//		}
//		else
//		{
//			TempCANFrame.Id.all =TempCANFrame.Id.StdID;
//			txHeader.StdId = TempCANFrame.Id.all & 0x000007ff;
//			txHeader.IDE   = CAN_ID_STD;
//		}

//		// 限制数据长度
//		if (TempCANFrame.Length > 8)
//		{
//			TempCANFrame.Length = 8;
//		}
//	
//		// 设置 RTR 和 DLC
//		if (TempCANFrame.isRemote)
//		{
//			txHeader.RTR = CAN_RTR_REMOTE;
//			txHeader.DLC = 0;
//		}
//		else
//		{
//			txHeader.RTR = CAN_RTR_DATA;
//			txHeader.DLC = TempCANFrame.Length;
//		}
//		
//		// 不启用时间戳功能
//		txHeader.TransmitGlobalTime = DISABLE;

//		#ifdef USE_CAN_1
//		if (TempCANFrame.canbus_id == MYCAN1)
//		{
//			while (HAL_CAN_AddTxMessage(&hcan1, &txHeader, &(TempCANFrame.Data.uchars[0]), &TxMailbox) != HAL_OK && (can_timeout <= 1500)) // 一定要加超时
//			{	
//				if(can_timeout > 1000)
//				{
//					HAL_CAN_AbortTxRequest(&hcan1, CAN_TX_MAILBOX0);
//					HAL_CAN_AbortTxRequest(&hcan1, CAN_TX_MAILBOX1);
//					HAL_CAN_AbortTxRequest(&hcan1, CAN_TX_MAILBOX2);
//				}
//				can_timeout++;
//			}
//		}
//		#endif
//		
//		#ifdef USE_CAN_2
//		if (TempCANFrame.canbus_id == MYCAN2)
//		{
//			while (HAL_CAN_AddTxMessage(&hcan2, &txHeader, &(TempCANFrame.Data.uchars[0]), &TxMailbox) != HAL_OK && (can_timeout <= 1500))
//			{
//				if(can_timeout > 1000)
//				{
//					HAL_CAN_AbortTxRequest(&hcan2, CAN_TX_MAILBOX0);
//					HAL_CAN_AbortTxRequest(&hcan2, CAN_TX_MAILBOX1);
//					HAL_CAN_AbortTxRequest(&hcan2, CAN_TX_MAILBOX2);
//				}
//				can_timeout++;
//			}
//		}
//		#endif
//		
//	}
//	else
//	{
//		*cancnt = 0;
//	}
//}

/**
  * @brief  定义中断回调函数，判断帧类型并调用专用处理函数(均使用此接收函数)
  */
#ifdef USE_CAN_1
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *CanHandle)
{
	/* Get RX message */
	if (CanHandle==&hcan1)
	{
		CAN_RxHeaderTypeDef hCAN1_RxHeader;
		HAL_StatusTypeDef   HAL_RetVal;
		CANFrame Frame_rcv;
		uint8_t aRxData[8];
		HAL_RetVal = HAL_CAN_GetRxMessage(CanHandle, CAN_RX_FIFO0, &hCAN1_RxHeader, aRxData);

		if (HAL_RetVal == HAL_OK)
		{
//			if (hCAN1_RxHeader.IDE == CAN_ID_EXT)
//			{
//				Frame_rcv.Id.all   = hCAN1_RxHeader.ExtId;
//				Frame_rcv.isRemote = hCAN1_RxHeader.RTR == CAN_RTR_DATA ? 0 : 1;
//				Frame_rcv.Length   = hCAN1_RxHeader.DLC;
//				memset(Frame_rcv.Data.chars, 0, 8);  //先清空再copy
//				memcpy(Frame_rcv.Data.chars, aRxData, 8);

//				if ((hCAN1_RxHeader.ExtId &(0xFF<<16))!=0)
//				{
//					Cybergear_read_parameter(&hCAN1_RxHeader, Frame_rcv.Data.uchars);
//				}
//				else
//				{
//					ProcessVESCFrame(&Frame_rcv);
//				}

//			}
			if (hCAN1_RxHeader.IDE == CAN_ID_STD)
			{
				Frame_rcv.Id.all   = hCAN1_RxHeader.StdId;
				Frame_rcv.isRemote = hCAN1_RxHeader.RTR == CAN_RTR_DATA ? 0 : 1;
				Frame_rcv.Length   = hCAN1_RxHeader.DLC;
				memset(Frame_rcv.Data.chars, 0, 8);  //先清空再copy
				memcpy(Frame_rcv.Data.chars, aRxData, 8);
				
				Process_DJFrame_CAN(&Frame_rcv);
			}
		}
	}
}
#endif

#ifdef USE_CAN_2
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *CanHandle)
{
    /* Get RX message */
	if (CanHandle==&hcan2)
	{
		CAN_RxHeaderTypeDef hCAN2_RxHeader;
		HAL_StatusTypeDef   HAL_RetVal;
		CANFrame Frame_rcv;
		uint8_t aRxData[8];
		HAL_RetVal = HAL_CAN_GetRxMessage(CanHandle, CAN_RX_FIFO1, &hCAN2_RxHeader, aRxData);

		//这个地方必须要把返回进行赋值，不能直接加入到下面的判断，否则会接收不到，具体原因未知
		if (HAL_RetVal== HAL_OK)
		{
//			if (hCAN2_RxHeader.IDE == CAN_ID_EXT)
//			{
//				Frame_rcv.Id.all   = hCAN2_RxHeader.ExtId;
//				Frame_rcv.isRemote = hCAN2_RxHeader.RTR == CAN_RTR_DATA ? 0 : 1;
//				Frame_rcv.Length   = hCAN2_RxHeader.DLC;
//				memset(Frame_rcv.Data.chars, 0, 8);  //先清空再copy
//				memcpy(Frame_rcv.Data.chars, aRxData, 8);

//				ProcessVESCFrame(&Frame_rcv);
//			}

			if (hCAN2_RxHeader.IDE == CAN_ID_STD)
			{
				Frame_rcv.Id.all   = hCAN2_RxHeader.StdId;
				Frame_rcv.isRemote = hCAN2_RxHeader.RTR == CAN_RTR_DATA ? 0 : 1;
				Frame_rcv.Length   = hCAN2_RxHeader.DLC;
				memset(Frame_rcv.Data.chars, 0, 8);  //先清空再copy
				memcpy(Frame_rcv.Data.chars, aRxData, 8);
				
				Process_DJFrame_CAN(&Frame_rcv);
			}

		}
	}
}

#endif

/**
* @brief  错误处理与回调函数(消息堵塞时自动调用，不可缺少)
  */
void ResetCANErrorCounters(CAN_HandleTypeDef *hcan)
{
    HAL_CAN_Stop(hcan);

    hcan->Instance->MCR |= CAN_MCR_INRQ;

    while ((hcan->Instance->MSR & CAN_MSR_INAK) == 0) {}

    hcan->Instance->ESR = 0;

    hcan->Instance->MCR &= ~CAN_MCR_INRQ;

    while ((hcan->Instance->MSR & CAN_MSR_INAK) != 0) {}

    HAL_CAN_Start(hcan);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    uint32_t esr_value = hcan->Instance->ESR;

    if (esr_value & CAN_ESR_BOFF)
    {
        ResetCANErrorCounters(hcan);
    }
    else
    {
        if (((esr_value & CAN_ESR_TEC) >> 16) > 200)
        {
            ResetCANErrorCounters(hcan);
        }

        if (((esr_value & CAN_ESR_REC) >> 24) > 200)
        {
            ResetCANErrorCounters(hcan);
        }
    }
//		ResetCANErrorCounters(hcan);

    hcan->Instance->MSR = CAN_MSR_ERRI;
}

//void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
//{
//    uint32_t esr_value = hcan->Instance->ESR;
//    uint32_t tec = (esr_value & CAN_ESR_TEC) >> 16;
//    uint32_t rec = (esr_value & CAN_ESR_REC) >> 24;
//    uint32_t boff = esr_value & CAN_ESR_BOFF;

////    printf("CAN Error: TEC=%d, REC=%d, BOFF=%d, ESR=0x%08lX\n", tec, rec, boff, esr_value);

//    if (boff)
//    {
////        printf(">>> Bus-Off! Resetting...\n");
//        ResetCANErrorCounters(hcan);
//    }
//    else if (tec > 200)
//    {
////        printf(">>> TEC > 200! Resetting...\n");
//        ResetCANErrorCounters(hcan);
//    }
//    else if (rec > 200)
//    {
////        printf(">>> REC > 200! Resetting...\n");
//        ResetCANErrorCounters(hcan);
//    }
//    else
//    {
////        printf(">>> Minor error (e.g., EWG, EPV, LEC), no reset.\n");
//		boff = 0;
//    }

//    hcan->Instance->MSR = CAN_MSR_ERRI;  // 清除错误中断标志
//}

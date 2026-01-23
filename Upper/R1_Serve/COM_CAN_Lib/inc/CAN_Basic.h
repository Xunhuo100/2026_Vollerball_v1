#ifndef CAN_BASIC_H_
#define CAN_BASIC_H_

#include "stm32f4xx_hal.h"
#include "string.h"
#include "cmsis_os2.h"
#include "main.h"


#define USE_CAN_1

#define MYCAN1                      1
#define MYCAN2                      2

#define ID_1_4                      1
#define ID_5_8                      5

typedef struct
{
    uint32_t DesDeviceId    :8;     
    uint32_t Property       :8;       
    uint32_t SrcDeviceId    :8;     
    uint32_t Priority       :4;         
    uint32_t Permit         :1;        
} EXT_ID_Typedef;                                 

typedef union
{
    uint32_t     all;
    uint32_t     StdID      :11;        //??ID
    EXT_ID_Typedef  ExtID ;     //??ID
} ID;

typedef union
{
    int8_t    chars[8];                             
    int16_t   shorts[4];                            
    int32_t   ints[2];                              
    int64_t   longs[1];                             
    uint8_t   uchars[8];                            
    uint16_t  ushorts[4];                           
    uint32_t  uints[2];                             
    uint64_t  ulongs[1];                            
    float     floats[2];
} CAN_Data;

typedef struct
{
	uint8_t canbus_id;															// CAN总线编号
	uint8_t IDE;                                    // IDE(是标准帧还是扩展帧)
	char isRemote;                                  // 是否为远程帧
	uint8_t Length;                                 // 数据长度(最大为8)
	CAN_Data Data;                                  // 实际数据
	ID Id;                                          // 帧ID
} CANFrame;

typedef struct
{
	uint16_t Prop;                                    //属性名称
	void (*Fun)(CANFrame* Frm);                       //此属性对应的处理函数
} CANFunDict;

#ifdef USE_CAN_1
void CAN1_Start(void);
#endif

#ifdef USE_CAN_2
void CAN2_Start(void);
#endif

void Send_Frame_CAN(CANFrame* Frame_Send, int canx) ;
void DeQueueCanMessage(uint16_t*);

#endif


#ifndef UART_BASIC_H_
#define UART_BASIC_H_

#include "stm32f4xx_hal.h"
#define HEAD_DETECT                 0
#define LENGTH_ACQUIRE              1
#define PROP_ACQUIRE                2
#define DATA_ACQUIRE                3
#define TAIL_CONFIRM                4
#define COM_FRAME_MAX_LENGTH        0XE0
#define COM_FRAME_MARK              0XEE            
#define COM_FRAME_ESCAPE            0XFF            
#define Usart1                      1
#define Usart2                      2
#define Usart3                      3
#define Usart4                      4
typedef union {
    int8_t    int8_ts[128];
    int16_t   int16_ts[64];
    int32_t   int32_ts[32];
    uint8_t   uint8_ts[128];
    uint16_t  uint16_ts[64];
    uint32_t  uint32_ts[32];
    float     floats_ts[32];
} COMData;

/******************************?CAN????????*********************************/
typedef struct {
    uint8_t   Length;
    uint8_t   Prop;
    COMData   Data;
} COMFrame;
typedef struct {
    uint8_t RecStatus;
    uint8_t RecCounts;
    uint8_t preData;
    uint8_t RecLength;
} COMRecStatus;
typedef struct {
    uint16_t Prop;                                  //属性名称
    void (*Fun)(COMFrame* Frm);                     //此属性对应的处理函数
} COMFunDict;

void Send_Frame_COM(COMFrame* Send_Frame,int USARTx);
void Get_Frame_COM(uint8_t Data, int USARTx);
#endif


#include "upper.h"

PIDStructTypedef GoSpeedPID;
float resey_degree;
HAL_StatusTypeDef tx_res;
HAL_StatusTypeDef rx_res;
MotorCmd_t RiseUp;
MotorCmd_t FallDown;//两个终点位置

void test(){}

float upper_go_reset()//强制把GO拉到初始位置, 返回初始位置的实际角度值
{
    MotorData_t data = {0};
		
    modify_data(&FallDown);

    for(int i = 0;i<5;i++)
    {
      tx_res = HAL_UART_Transmit(&huart1, (uint8_t *)&FallDown.motor_send_data, sizeof(FallDown.motor_send_data), 1);
			rx_res = HAL_UART_Receive(&huart1, (uint8_t *)&data.motor_recv_data, sizeof(data.motor_recv_data), 1);
			if (rx_res == HAL_TIMEOUT)
			{
				data.timeout++;
			}
			osDelay(1);
    }
}

void upper_go_init()
{
    FallDown.mode = 1;
    FallDown.id   = 1;
    FallDown.K_P   = 0.5;
    FallDown.K_W   = 0.00;
    FallDown.Pos    = 4.2f;
    FallDown.W   = 0;
    FallDown.T  = 1.2f;

    RiseUp.mode = 1;
    RiseUp.id   = 1;
    RiseUp.K_P   = 1.2;;
    RiseUp.K_W   = 0.00;
    RiseUp.Pos    = 9.0f;
    RiseUp.W   = 0;
    RiseUp.T  =  0.5;

}

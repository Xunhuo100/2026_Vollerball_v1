#include "application.h"

float init_degree;
MotorCmd_t _cmd = {0};
MotorData_t _data = {0};
	myGo m;
int fputc(int ch, FILE *stream)
{
	HAL_UART_Transmit(&huart4,(uint8_t*)&ch,1,HAL_MAX_DELAY);
	return ch;
}


void upper_task(void *argument)
{

  //upper_go_init();
	//init_degree = upper_go_reset();

	
	
	
	mygo_init(&m,1,0.23,0.005,0.03,10.f);
	
//  _cmd.id = 1;
//  _cmd.mode = 1;
//  _cmd.K_P = 0;
//  _cmd.K_W = 0.2;
//  _cmd.Pos = 0;
//  _cmd.W = 1;
//  _cmd.T = 0;
	
	osDelay(500);
	while(1)
	{
		float d = m.data.Pos/6.33;
		
		mygo_speed_output(&m,4.f+3.f*cos(d));	
		osDelay(1);
	}
}

void test_led_task(void *argument)
{
	while(1){
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_15);
		printf("%f\n",m.data.W);
		osDelay(1);
	}
}

void _tim2_callback()
{
	
}
	
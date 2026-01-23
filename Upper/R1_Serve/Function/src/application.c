#include "application.h"
#define PAI 3.1415926
float init_degree;
MotorCmd_t _cmd = {0};
MotorData_t _data = {0};
myGo m;
uint8_t immm = 0;
uint8_t servestate = 0;

uint8_t initstate = 0;
float startpos;
void task_main(void * argument)
{
	upper_go_init();
	m.cmd = Calmdown;
	mygo_output(&m);
	startpos = m.data.Pos; 

	m.cmd.K_W = 0.1;
	m.cmd.W = 3;
	while(1)
	{
		if(immm == 1)
			mygo_speed_output(&m,40);
		else
		{
			m.cmd = Calmdown;
			mygo_output(&m);
			startpos = m.data.Pos;
		}
			
		
		osDelay(10);
	}
}

void task_serve(void *argument)
{

	while(1)
	{
		if(m.data.Pos >CIRCLE+startpos)
			immm = 0;
		osDelay(100);
	}
}

void test_led_task(void *argument)
{
	uint8_t test = 0;
	
	while(1)
	{
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_15);
		osDelay(100);
	}
}

void hit_time_callback(void* argument)
{
	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);
	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_15);
	
	isSpeed = 0;
}
	
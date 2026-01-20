#include "application.h"

float init_degree;
MotorCmd_t _cmd = {0};
MotorData_t _data = {0};
myGo m;

uint8_t servestate = 0;

uint8_t initstate = 0;

int fputc(int ch, FILE *stream)
{
	HAL_UART_Transmit(&huart4,(uint8_t*)&ch,1,HAL_MAX_DELAY);
	return ch;
}

void task_main(void * argument)
{
	while(initstate!=2)
		osDelay	(1);
	
	upper_bow_execute(0);
	osDelay(500);
	while(1)
	{
		if(servestate == 0)
		{
			upper_hit_execute(50);
			
			while(1)
			{
				if(hit_state == Free)
				{
					servestate = 1;
					osDelay(300);
					break;
				}
				osDelay(1);
			}
		}
		if(servestate == 1)
		{
			upper_hit_execute(40);
			
			while(1)
			{
				if(hit_state == Free)
				{
					servestate = 2;
					break;
				}
				osDelay(1);

			}
		}
		
		osDelay(1);
	}
	
}

void task_hit(void *argument)
{
  upper_go_init();
	hit_state = Free;
	initstate++;
	while(1)
	{
		if(hit_cmd == 1&& hit_state == Free)
		{
			upper_hit_work(targetSpeed);
			osDelay(100);
			continue;
		}
		
		mygo_output(&m);
	
		osDelay(1);
	}
}

void task_bow(void *argument)
{
	upper_3508_init(3);
	bow_state = Free;
	initstate++;
  while(1)
  {
		if(bow_cmd == 1&& bow_state == Free)
		{
			upper_bow_work(targetDegree);
			osDelay(100);
		}
		switch(BowMotor.mode)
		{
			case MOTOR_POSITION:				
				BowMotor.SpeedExpected =  Pid_Regulate(BowMotor.PositionExpected,BowMotor.PositionMeasure,&BowMotorPID.PosPID);

			case MOTOR_SPEED:						
				BowMotor.CurrentExpected = Pid_Regulate(BowMotor.SpeedExpected,BowMotor.SpeedMeasure,&BowMotorPID.SpeedPID);
			
			case MOTOR_CURRENT: 
				break; 
			
			case MOTOR_ERROR:	
				break;
			
			default:
				BowMotor.CurrentExpected = 0;
				break;		
		}
		
		dj_can_set(ID_1_4, MYCAN1, 0, 0, BowMotor.CurrentExpected,0);
    osDelay(1);
  }
}

void test_led_task(void *argument)
{
	uint8_t test = 0;
	
	while(1){
//		switch(test){
//			case 0:
//				BowMotor.PositionExpected = 0;
//				test++;
//				break;
//			case 1:
//				BowMotor.PositionExpected = 60;
//				test=0;
//				break;
//			
//		}
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_15);
		//printf("%f\n",m.data.W);
		osDelay(1000);
	}
}




void _tim2_callback()
{
	
}
	
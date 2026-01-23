#include "application.h"

float init_degree;
MotorCmd_t _cmd = {0};
MotorData_t _data = {0};
myGo m;

uint8_t servestate = 0;

uint8_t initstate = 0;

void task_main(void * argument)
{
	while(initstate!=2)
		osDelay	(1);
	
	upper_hit_execute(5);
	upper_bow_slew_execute(0,0);
	osDelay(500);
	while(1)
	{
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

void task_bow_slew(void *argument)
{
	upper_3508_init();
	initstate++;
  while(1)
  {
		BowMotor.SpeedExpected =  Pid_Regulate(BowMotor.PositionExpected,BowMotor.PositionMeasure,&BowMotorPID.PosPID);
		BowMotor.CurrentExpected = Pid_Regulate(BowMotor.SpeedExpected,BowMotor.SpeedMeasure,&BowMotorPID.SpeedPID);
		SlewMotor.SpeedExpected = Pid_Regulate(SlewMotor.PositionExpected,SlewMotor.PositionMeasure,&SlewMotorPID.PosPID);
		SlewMotor.CurrentExpected = Pid_Regulate(SlewMotor.SpeedExpected,SlewMotor.SpeedMeasure,&SlewMotorPID.SpeedPID);
		
		dj_can_set(ID_1_4, MYCAN1, SlewMotor.CurrentExpected, 0, BowMotor.CurrentExpected,0);
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
	
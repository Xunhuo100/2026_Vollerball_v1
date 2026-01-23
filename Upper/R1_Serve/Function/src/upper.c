#include "upper.h"

#define HIT_END_POS 20.f

extern myGo m;

PIDStructTypedef GoSpeedPID;
HAL_StatusTypeDef tx_res;
HAL_StatusTypeDef rx_res;

MotorCmd_t RiseUp;
MotorCmd_t FallDown;


MotorCmd_t Calmdown;
MotorCmd_t Work;

uint8_t hit_state = Busy;
//uint8_t bow_state = Busy;

uint8_t hit_cmd = 0;
uint8_t isSpeed = 0;
//uint8_t bow_cmd = 0;

float targetDegree;
float targetSpeed;

void upper_go_init()
{
    FallDown.mode = 1;
    FallDown.id   = 1;
    FallDown.K_P   = 0.3;
    FallDown.K_W   = 0.00;
    FallDown.Pos    = 5.0f;
    FallDown.W   = 0;
    FallDown.T  = 1.0f;

    RiseUp.mode = 1;
    RiseUp.id   = 1;
    RiseUp.K_P   = 1.1;
    RiseUp.K_W   = 0.00;
    RiseUp.Pos    = CIRCLE;
    RiseUp.W   = 0;
    RiseUp.T  =  1.0;
	
//		Work.mode = 1;
//    Work.id   = 1;
//    Work.K_P   = 0.f;
//    Work.K_W   = 0.25f;
//    Work.Pos    = 0.f;
//    Work.W   = 0;
//    Work.T  =  0.f;
		
		
		Calmdown.mode = 1;
    Calmdown.id   = 1;
    Calmdown.K_P   = 0.f;
    Calmdown.K_W   = 0.f;
    Calmdown.Pos    = 0.f;
    Calmdown.W   = 0;
    Calmdown.T  =  0.f;
		
	
		mygo_init(&m,1,0.26,0.0,0.33,23.f);
		m.cmd = Calmdown;
		mygo_output(&m);
}

void upper_hit_work(float speed)
{
	hit_state = Busy;
	
	isSpeed = 1;
	while(1)
	{
		if(isSpeed != 1)
		{
			m.cmd.W = 0;
			hit_cmd = 0;
			hit_state = Free;
			
			m.cmd = Calmdown;
			mygo_output(&m);
			
			return;
		}
		mygo_speed_output(&m,speed);
		osDelay(100);
	}
}

uint8_t upper_hit_execute(float speed)
{
	if(hit_state == Free)
	{
		m.cmd = Work;
		m.cmd.W = speed;
		osDelay(50);
		return 1;
	}
	else return 0;
}



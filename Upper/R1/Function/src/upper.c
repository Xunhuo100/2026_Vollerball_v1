#include "upper.h"

#define HIT_END_POS 8.9f

extern myGo m;

PIDStructTypedef GoSpeedPID;
HAL_StatusTypeDef tx_res;
HAL_StatusTypeDef rx_res;
MotorCmd_t RiseUp;
MotorCmd_t FallDown;//两个终点位置

MotorTypeDef BowMotor;
MOTORPIDstructTypedef BowMotorPID;

uint8_t workstate = Busy;
uint8_t hit_state = Busy;
uint8_t bow_state = Busy;

uint8_t hit_cmd = 0;
uint8_t bow_cmd = 0;

float targetDegree;
float targetSpeed;

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
    FallDown.K_P   = 0.3;
    FallDown.K_W   = 0.00;
    FallDown.Pos    = 5.0f;
    FallDown.W   = 0;
    FallDown.T  = 1.0f;

    RiseUp.mode = 1;
    RiseUp.id   = 1;
    RiseUp.K_P   = 2.5;
    RiseUp.K_W   = 0.00;
    RiseUp.Pos    = HIT_END_POS;
    RiseUp.W   = 0;
    RiseUp.T  =  1.6;
	
		mygo_init(&m,1,0.23,0.005,0.03,10.f);
}

void upper_3508_init(uint32_t ID)
{
	BowMotor.ID = ID;
	BowMotor.mode = MOTOR_POSITION;
	MyMotor_PID_init(&(BowMotorPID));
	CAN1_Start();
}

void upper_hit_work(float speed)
{
//	hit_state = Busy;
//	m.cmd = RiseUp;
//	for(uint8_t i = 0;i<2;i++)
//	{
//		mygo_output(&m);
//		osDelay(1);
//	}
//	osDelay(200);
//	m.cmd = FallDown;
//	for(uint8_t i = 0;i<2;i++)
//	{
//		mygo_output(&m);
//		osDelay(1);
//	}
//	hit_state = Free;
//	hit_cmd = 0;
//	return 1;
		
	hit_state = Busy;
	float teststate;
	while(1)
	{
		teststate =  m.data.Pos-HIT_END_POS;
		if(teststate>0.1f)
		{
			m.cmd = RiseUp;
			for(int i =0;i<1;i++)
			{
				mygo_output(&m);
				osDelay(1);
			}
			osDelay(200);
			m.cmd = FallDown;
			for(uint8_t i = 0;i<1;i++)
			{
				mygo_output(&m);
				osDelay(1);
			}
			hit_state = Free;
			hit_cmd = 0;
			return;
		}
		mygo_speed_output(&m,speed);
		osDelay(1);
	}
}

void upper_bow_work(float degree)
{
	uint8_t isFinish = 0;
	bow_state = Busy;
	BowMotor.PositionExpected = degree;
	while(1)
	{
		BowMotor.SpeedExpected =  Pid_Regulate(BowMotor.PositionExpected,BowMotor.PositionMeasure,&BowMotorPID.PosPID);
		BowMotor.CurrentExpected = Pid_Regulate(BowMotor.SpeedExpected,BowMotor.SpeedMeasure,&BowMotorPID.SpeedPID);
		dj_can_set(ID_1_4, MYCAN1, 0, 0, BowMotor.CurrentExpected,0);
		if(fabs(BowMotor.PositionExpected-BowMotor.PositionMeasure)<10)
		{
			isFinish++;
			if(isFinish >= 10)
			{
				bow_state = Free;
				bow_cmd = 0;
				return;
			}
		}
    osDelay(1);
	}
}

uint8_t upper_slew_work(){}

uint8_t upper_hit_execute(float speed)
{
	if(hit_state == Free)
	{
		targetSpeed = speed;
		hit_cmd = 1;
		osDelay(50);
		return 1;
	}
	else return 0;
}

uint8_t upper_bow_execute(float degree)
{
	if(bow_state == Free)
	{
		targetDegree = degree;
		bow_cmd = 1;
		osDelay(50);
		return 1;
	}
	else return 0;
}

uint8_t upper_slew_excute(float degree)
{}
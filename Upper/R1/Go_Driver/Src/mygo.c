#include "mygo.h"

void mygo_init(myGo* m,uint8_t id,float speed_kp,float speed_ki,float speed_kd,float limit_output)
{
	m->id = id;
	
	m->speed_pid.Integral = 0.f;
	m->speed_pid.Kp = speed_kp;
	m->speed_pid.Ki = speed_ki;
	m->speed_pid.Kd = speed_kd;
	m->speed_pid.LimitIntegral = 10.f;
	m->speed_pid.PreError = 0.f;
	m->speed_pid.LimitOutput = limit_output;
	
	m->cmd.id = id;
}

void mygo_output(myGo*m)
{
	modify_data(&(m->cmd));
	m->tx_res = HAL_UART_Transmit(&huart1, (uint8_t *)&(m->cmd.motor_send_data), sizeof(m->cmd.motor_send_data), 10);
	m->rx_res	= HAL_UART_Receive(&huart1, (uint8_t *)&(m->data.motor_recv_data), sizeof(m->data.motor_recv_data), 10);
	extract_data(&(m->data));
}

void mygo_speed_output(myGo*m,float tgt)
{
	float output = Pid_Regulate(tgt,m->data.W,&(m->speed_pid));
	m->cmd.id = m->id;
	m->cmd.K_P = 0;
	m->cmd.K_W = m->speed_pid.Kp;
	m->cmd.mode = 1;
	m->cmd.Pos = 0.0;
	m->cmd.W = tgt;
	m->cmd.T = output-m->cmd.K_W*(m->cmd.W-m->data.W);//直接把PID输出作为前馈力矩会发出巨大噪声，疑似是GO对指令的解析所致，所以这么写了:)
	
	mygo_output(m);
}
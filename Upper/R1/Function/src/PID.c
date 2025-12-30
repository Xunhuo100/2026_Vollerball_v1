#include "PID.h"
/**
 * @brief PID 控制器计算函数
 * @param PID 结构体指针, Reference 目标值, Present_Feedback 当前反馈值
 * @retval float 控制输出值
 */
MOTORPIDstructTypedef MyMotorPID;
 
float Pid_Regulate(float Reference, float Present_Feedback, PIDStructTypedef* PID_Struct)
{
    float Error;
    float Error_Inc;
    float pTerm;
    float iTerm;
    float dTerm;
    float new_integral;
    float Output;
    /*Error computation*/
    Error = Reference - Present_Feedback;

    /*proportional term computation*/
    pTerm = Error * PID_Struct->Kp;

    /*Integral term computation*/
    if (PID_Struct->Ki == 0)
        PID_Struct->Integral = 0;
    else
    {
        iTerm = Error * PID_Struct->Ki;
        new_integral = PID_Struct->Integral + iTerm;

        /*limit integral*/
        if (new_integral > PID_Struct->LimitIntegral)
            PID_Struct->Integral = PID_Struct->LimitIntegral;
        else if (new_integral < -1 * PID_Struct->LimitIntegral)
            PID_Struct->Integral = -1 * PID_Struct->LimitIntegral;
        else
            PID_Struct->Integral = new_integral;
    }

    /*differential term computation*/
    Error_Inc            = Error - PID_Struct->PreError;
    dTerm                = Error_Inc * PID_Struct->Kd;
    PID_Struct->PreError = Error;
    Output               = pTerm + PID_Struct->Integral + dTerm;

    /*limit Output*/
    if (Output >= PID_Struct->LimitOutput)
        return (PID_Struct->LimitOutput);
    else if (Output < -1 * PID_Struct->LimitOutput)
        return (-1 * PID_Struct->LimitOutput);
    else
        return Output;
}

/**
* @brief 
				- Kp,Ki,Kd 需上电机整定，可借住VOFA等软件
				- PosPID.LimitOutput 为位置环时的速度上限
				- SpeedPID.LimitOutput 为速度环时的电流上限，2006电机最大值为8000，3508电机最大值为16000
					- C620 和 C610 控制电流范围为-16384 ~ 16384 ，对应电调输出转矩电流范围为 -20 ~ 20A 
					- C610 最大电流持续电流为 10A，C620 最大电流持续电流为 20A，
				- LimitIntegral 略小于 LimitOutput 即可
 */
void MyMotor_PID_init(MOTORPIDstructTypedef* MotorPID)
{
	
    MotorPID->PosPID.Kp = 5.0f;
    MotorPID->PosPID.Ki = 0.005f;
    MotorPID->PosPID.Kd = 0.10f;
    MotorPID->PosPID.LimitIntegral = 50.0f;
    MotorPID->PosPID.LimitOutput = 200.0f;
		MotorPID->PosPID.Integral = 0;
		MotorPID->PosPID.PreError = 0;
	
    MotorPID->SpeedPID.Kp = 100.0f;
    MotorPID->SpeedPID.Ki = 0.1f;
    MotorPID->SpeedPID.Kd = 0.0f;
    MotorPID->SpeedPID.LimitIntegral = 500.0f;
    MotorPID->SpeedPID.LimitOutput = 1000.0f;
		MotorPID->SpeedPID.Integral = 0;
		MotorPID->SpeedPID.PreError = 0;

		MotorPID->ReservePID.Kp = 0.0f;
    MotorPID->ReservePID.Ki = 0.0f;
    MotorPID->ReservePID.Kd = 0.0f;
    MotorPID->ReservePID.LimitIntegral = 0.0f;
    MotorPID->ReservePID.LimitOutput = 0.0f;
		MotorPID->ReservePID.Integral = 0;
		MotorPID->ReservePID.PreError = 0;
		
}

void upper_go1_init()
{
}


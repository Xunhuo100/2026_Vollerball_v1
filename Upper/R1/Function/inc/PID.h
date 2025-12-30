#ifndef PID_H_
#define PID_H_

/**
 * @brief PID 结构体定义
 * 用于存储 PID 控制器的参数和状态
 */
typedef struct {
    float Kp;               // 比例系数
    float Ki;               // 积分系数
    float Kd;               // 微分系数
    float LimitOutput;      // 输出限幅值
    float LimitIntegral;    // 积分限幅值
    float Integral;         // 积分项累加值
    float PreError;         // 上一次误差
} PIDStructTypedef;

typedef struct {
	PIDStructTypedef SpeedPID;
	PIDStructTypedef PosPID;
	PIDStructTypedef ReservePID;
} MOTORPIDstructTypedef;

extern MOTORPIDstructTypedef MyMotorPID;

float Pid_Regulate(float Reference, float Present_Feedback, PIDStructTypedef* PID_Struct);

void MyMotor_PID_init(MOTORPIDstructTypedef* MotorPID);

#endif

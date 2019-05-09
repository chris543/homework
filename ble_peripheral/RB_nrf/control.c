#include "control.h"
#include "motor.h"
#include "I2C.h"
#include "ICM20602.h"
#include "math.h"
#define L_AIN2 21
#define L_AIN1 22
#define R_BIN1 23
#define R_BIN2 24
unsigned char g_u8MainEventCount;
unsigned char g_u8SpeedControlCount;
unsigned char g_u8SpeedControlPeriod;
unsigned char g_u8DirectionControlPeriod;
unsigned char g_u8DirectionControlCount;
unsigned char g_cMotorDisable = 0; //值等于0时电机正常转动，否则停止转动

int g_iGravity_Offset = 0;
/*-----angle & speed PID -----*/
PID_t g_tCarAnglePID = {10.1, 0, 30}; //
PID_t g_tCarSpeedPID = {10.6, 0.64, 0}; //5.0, 0.4, 0
// PID_t g_tCarSpeedPID = {0, 0, 0}; //5.0, 0.4, 0

/******电机控制参数******/
float g_fSpeedControlOut;
float g_fSpeedControlOutOld;
float g_fSpeedControlOutNew;
float g_fAngleControlOut;
float g_fLeftMotorOut;
float g_fRightMotorOut;

/******速度控制参数******/

short g_s16LeftMotorPulse;
short g_s16RightMotorPulse;

int g_s32LeftMotorPulseOld;
int g_s32RightMotorPulseOld;
int g_s32LeftMotorPulseSigma;
int g_s32RightMotorPulseSigma;

float g_fCarSpeed;
float g_iCarSpeedSet;
float g_fCarSpeedOld;
float g_fCarPosition;

/******蓝牙控制参数******/
float g_fBluetoothSpeed;
float g_fBluetoothDirection;
float g_fBluetoothDirectionOld;
float g_fBluetoothDirectionNew;
float g_fBluetoothDirectionOut;

float g_fCarAngle;       //
float g_fGyroAngleSpeed; //
float g_fGravityAngle;   //

int g_iLeftTurnRoundCnt = 0;
int g_iRightTurnRoundCnt = 0;

static int AbnormalSpinFlag = 0;

void SysTick_Handler(void)
{
    // nrf_gpio_pin_set(7); //toggle io pin for scope timing

    if (!disableBalancing)
    {

        g_u8MainEventCount++;

        g_u8SpeedControlPeriod++;
        SpeedControlOutput(); //速度环控制输出函数，每1ms执行一次
        if (g_u8MainEventCount >= 5)
        {
            g_u8MainEventCount = 0;
            GetMotorPulse(); //捕获电机脉冲（速度）函数，每5ms执行一次
        }
        else if (g_u8MainEventCount == 1)
        {
            MPU6050_Pose();   //读取MPU6050数据函数，每5ms执行一次
            AngleCalculate(); //角度环计算函数，每5ms执行一次
        }
        else if (g_u8MainEventCount == 2)
        {
            AngleControl(); //角度环控制函数，每5ms执行一次
        }
        else if (g_u8MainEventCount == 3)
        {
            g_u8SpeedControlCount++;
            if (g_u8SpeedControlCount >= 5) //25ms
            {
                SpeedControl(); //车模速度控制函数，每25ms调用一次
                g_u8SpeedControlCount = 0;
                g_u8SpeedControlPeriod = 0;
            }
        }
        else if (g_u8MainEventCount == 4)
        {
            // MotorManage();			//电机使能/失能控制函数，每5ms执行一次
            MotorOutput(); //电机输出函数，每5ms执行一次
        }
    }

    // nrf_gpio_pin_clear(7);
}

void AngleCalculate(void)
{
    //-------加速度--------------------------
    //量程为±2g时，灵敏度：16384 LSB/g
    g_fGravityAngle = atan2(g_fAccel_y / 16384.0, g_fAccel_z / 16384.0) * 180.0 / 3.14;
    g_fGravityAngle = g_fGravityAngle - g_iGravity_Offset;
    // printf("Ac: %f\r\n",g_fGravityAngle);

    //-------角速度-------------------------
    //范围为2000deg/s时，换算关系：16.4 LSB/(deg/s)
    g_fGyro_x = g_fGyro_x / 16.4; //计算角速度值
    g_fGyroAngleSpeed = g_fGyro_x;
    // printf("fgx: %hd\r\n",g_fGyro_x);

    //-------互补滤波---------------
    g_fCarAngle = 0.98 * (g_fCarAngle + g_fGyroAngleSpeed * 0.005) + 0.02 * g_fGravityAngle;
    // printf("A: %f\r\n", g_fCarAngle);
}

void GetMotorPulse(void)
{

    g_s16LeftMotorPulse = SwRotaryEncoder_readAbs(L_encoder);
    g_s16RightMotorPulse = -SwRotaryEncoder_readAbs(R_encoder);
    SwRotaryEncoder_clearAbs(L_encoder);
    SwRotaryEncoder_clearAbs(R_encoder);

    g_s32LeftMotorPulseSigma += g_s16LeftMotorPulse;
    g_s32RightMotorPulseSigma += g_s16RightMotorPulse;

    g_iLeftTurnRoundCnt -= g_s16LeftMotorPulse;
    g_iRightTurnRoundCnt -= g_s16RightMotorPulse;
    // printf("L_plus %d\r\n",g_s16LeftMotorPulse);
    // printf("R_plus %d\r\n",g_s16RightMotorPulse);
}
void AngleControl(void)
{
    g_fAngleControlOut = (CAR_ANGLE_SET - g_fCarAngle) * g_tCarAnglePID.P * 5 +
                         (CAR_ANGLE_SPEED_SET - g_fGyroAngleSpeed) * (g_tCarAnglePID.D / 10);
}
void SpeedControlOutput(void)
{
    float fValue;
    fValue = g_fSpeedControlOutNew - g_fSpeedControlOutOld;
    g_fSpeedControlOut = fValue * (g_u8SpeedControlPeriod + 1) / SPEED_CONTROL_PERIOD + g_fSpeedControlOutOld;
}
void SpeedControl(void)
{
    float fP, fI;
    float fDelta;

    g_fCarSpeed = (g_s32LeftMotorPulseSigma + g_s32RightMotorPulseSigma) * 0.5;
    g_s32LeftMotorPulseSigma = g_s32RightMotorPulseSigma = 0; //全局变量 注意及时清零

    g_fCarSpeed = 0.7 * g_fCarSpeedOld + 0.3 * g_fCarSpeed; //低通滤波，使速度更平滑
    g_fCarSpeedOld = g_fCarSpeed;

    fDelta = CAR_SPEED_SET;
    fDelta -= g_fCarSpeed;

    fP = fDelta * (g_tCarSpeedPID.P);
    fI = fDelta * (g_tCarSpeedPID.I / 10.0);

    g_fCarPosition += fI;
    g_fCarPosition += g_fBluetoothSpeed;

    //积分上限设限
    if ((int16_t)g_fCarPosition > CAR_POSITION_MAX)
        g_fCarPosition = CAR_POSITION_MAX;
    if ((int16_t)g_fCarPosition < CAR_POSITION_MIN)
        g_fCarPosition = CAR_POSITION_MIN;

    g_fSpeedControlOutOld = g_fSpeedControlOutNew;
    g_fSpeedControlOutNew = fP + g_fCarPosition;
}

void SetMotorVoltageAndDirection(int i16LeftVoltage, int i16RightVoltage)
{
    if (i16LeftVoltage < 0)
    {
        nrf_gpio_pin_set(L_AIN1);
        nrf_gpio_pin_clear(L_AIN2);

        // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
        // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
        i16LeftVoltage = (-i16LeftVoltage);
    }
    else
    {
        nrf_gpio_pin_set(L_AIN2);
        nrf_gpio_pin_clear(L_AIN1);
        // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
        // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    }

    if (i16RightVoltage < 0)
    {
        nrf_gpio_pin_set(R_BIN1);
        nrf_gpio_pin_clear(R_BIN2);
        // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
        // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        i16RightVoltage = (-i16RightVoltage);
    }
    else
    {
        nrf_gpio_pin_set(R_BIN2);
        nrf_gpio_pin_clear(R_BIN1);
        // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
        // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
    }

    if (i16RightVoltage > MOTOR_OUT_MAX)
    {
        i16RightVoltage = MOTOR_OUT_MAX;
    }
    if (i16LeftVoltage > MOTOR_OUT_MAX)
    {
        i16LeftVoltage = MOTOR_OUT_MAX;
    }

    if (g_cMotorDisable)
    {
        // __HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, 0);
        // __HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_2, 0);
        motorPWM(0, 0);
        motorPWM(1, 0);
    }
    else
    {
        // printf("L: %d\r\n", i16LeftVoltage);
        // printf("R: %d\r\n", i16RightVoltage);
        motorPWM(0, i16LeftVoltage);
        motorPWM(1, i16RightVoltage);
    }
}

void MotorOutput(void)
{
    g_fLeftMotorOut = g_fAngleControlOut - g_fSpeedControlOut - g_fBluetoothDirection;
    g_fRightMotorOut = g_fAngleControlOut - g_fSpeedControlOut + g_fBluetoothDirection;

    /*增加死区常数*/
    if ((int)g_fLeftMotorOut > 0)
        g_fLeftMotorOut += MOTOR_OUT_DEAD_VAL;
    else if ((int)g_fLeftMotorOut < 0)
        g_fLeftMotorOut -= MOTOR_OUT_DEAD_VAL;
    if ((int)g_fRightMotorOut > 0)
        g_fRightMotorOut += MOTOR_OUT_DEAD_VAL;
    else if ((int)g_fRightMotorOut < 0)
        g_fRightMotorOut -= MOTOR_OUT_DEAD_VAL;

    /*输出饱和处理，防止超出PWM范围*/
    if ((int)g_fLeftMotorOut > MOTOR_OUT_MAX)
        g_fLeftMotorOut = MOTOR_OUT_MAX;
    if ((int)g_fLeftMotorOut < MOTOR_OUT_MIN)
        g_fLeftMotorOut = MOTOR_OUT_MIN;
    if ((int)g_fRightMotorOut > MOTOR_OUT_MAX)
        g_fRightMotorOut = MOTOR_OUT_MAX;
    if ((int)g_fRightMotorOut < MOTOR_OUT_MIN)
        g_fRightMotorOut = MOTOR_OUT_MIN;

    SetMotorVoltageAndDirection((int)g_fLeftMotorOut, (int)g_fRightMotorOut);
}
float Scale(float input, float inputMin, float inputMax, float outputMin, float outputMax)
{
    float output;
    if (inputMin < inputMax)
        output = (input - inputMin) / ((inputMax - inputMin) / (outputMax - outputMin));
    else
        output = (inputMin - input) / ((inputMin - inputMax) / (outputMax - outputMin));
    if (output > outputMax)
        output = outputMax;
    else if (output < outputMin)
        output = outputMin;
    return output;
}
void Steer(float direct, float speed)
{
    if (direct > 0)
        g_fBluetoothDirection = Scale(direct, 0, 10, 0, 400);
    else
        g_fBluetoothDirection = -Scale(direct, 0, -10, 0, 400);

    if (speed > 0)
        g_iCarSpeedSet = Scale(speed, 0, 10, 0, 175);
    else
        g_iCarSpeedSet = -Scale(speed, 0, -10, 0, 175);
}

void MotorManage(void)
{

    // AbnormalSpinDetect(g_s16LeftMotorPulse, g_s16RightMotorPulse);

    // LandingDetect();

    if (AbnormalSpinFlag)
    {
        g_cMotorDisable |= (0x01 << 1);
    }
    else
    {
        g_cMotorDisable &= ~(0x01 << 1);
    }

    if (g_fCarAngle > 30 || g_fCarAngle < (-30))
    {
        g_cMotorDisable |= (0x01 << 2);
    }
    else
    {
        g_cMotorDisable &= ~(0x01 << 2);
    }
}

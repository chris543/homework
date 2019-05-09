#include "ICM20602.h"
#include "I2C.h"
#include "nrf_delay.h"
#include "LED.h"

#define MPU6050_ADDR 0x69
#define SMPLRT_DIV 0x19   //陀螺仪采样率，典型值：0x07(125Hz)
#define CONFIG 0x1A       //低通滤波频率，典型值：0x06(5Hz)
#define GYRO_CONFIG 0x1B  //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define ACCEL_CONFIG 0x1C //加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48
#define PWR_MGMT_1 0x6B //电源管理，典型值：0x00(正常启用)
#define WHO_AM_I 0x75   //IIC地址寄存器(默认数值0x68，只读)
#define TESTREG 0x41
typedef enum _axis_t
{
    x_Axis,
    y_Axis,
    z_Axis
} axis_t;

short g_fGyro_x, g_fAccel_y, g_fAccel_z;
uint8_t disableBalancing = 1;

int16_t AcZ, AcY, GyX;
//IMU offset values
int16_t AcZ_offset = 0;
int16_t AcY_offset = 0;
int16_t GyX_offset = 700;
int32_t GyX_offset_sum = 0;
// float Acc_angle; //angle calculated from acc. measurments
float robot_angle;
#define Gyro_amount 0.996 //percent of gyro in complementary filter T/(T+del_t) del_t: sampling rate, T acc. timeconstant ~1s

uint8_t who;
void MPU6050_initialize(void)
{
    int8_t error;
    // uint8_t buff[5] = {0x00, 0x07, 0x04, 0x18, 0x01};

    // error = i2cwrite(MPU6050_ADDR, PWR_MGMT_1, 1, &buff[0]); //解除休眠状态
    // if (error != 0)
    // {
    //     printf("i2c write error\r\n");
    // }

    // nrf_delay_ms(50);

    // error = i2cwrite(MPU6050_ADDR, SMPLRT_DIV, 1, &buff[1]); //陀螺仪125hz //1k /(1+7)
    // if (error != 0)
    // {
    //     printf("i2c write error\r\n");
    // }
    // nrf_delay_ms(50);

    // error = i2cwrite(MPU6050_ADDR, CONFIG, 1, &buff[2]); //Accelerometer:21hz 8.5ms ; Gyroscope:20hz 8.3ms
    // if (error != 0)
    // {
    //     printf("i2c write error\r\n");
    // }
    // nrf_delay_ms(50);

    // error = i2cwrite(MPU6050_ADDR, GYRO_CONFIG, 1, &buff[3]); //±2000°/s
    // if (error != 0)
    // {
    //     printf("i2c write error\r\n");
    // }
    // error = i2cwrite(MPU6050_ADDR, ACCEL_CONFIG, 1, &buff[4]); //±2g
    // if (error != 0)
    // {
    //     printf("i2c write error\r\n");
    // }
    //-------
    unsigned char buff[6] = {0x80, 0x00, 0x07, 0x04, 0x18, 0x01};

    error = i2cwrite(MPU6050_ADDR, PWR_MGMT_1, 1, &buff[0]); //reset
    if (error != 0)
    {
        printf("i2c write error");
    }
    nrf_delay_ms(50);
    error = i2cwrite(MPU6050_ADDR, PWR_MGMT_1, 1, &buff[1]); //关闭睡眠
    if (error != 0)
    {
        printf("i2c write error");
    }
    nrf_delay_ms(50);
    error = i2cwrite(MPU6050_ADDR, SMPLRT_DIV, 1, &buff[2]); //陀螺仪125hz //1k /(1+7)
    if (error != 0)
    {
        printf("i2c write error");
    }
    error = i2cwrite(MPU6050_ADDR, CONFIG, 1, &buff[3]); //Accelerometer:21hz 8.5ms ; Gyroscope:20hz 8.3ms
    if (error != 0)
    {
        printf("i2c write error");
    }
    error = i2cwrite(MPU6050_ADDR, GYRO_CONFIG, 1, &buff[4]); //±2000°/s
    if (error != 0)
    {
        printf("i2c write error");
    }
    error = i2cwrite(MPU6050_ADDR, ACCEL_CONFIG, 1, &buff[5]); //±2g
    if (error != 0)
    {
        printf("i2c write error");
    }

    nrf_delay_ms(500);

    whoAmI();

    nrf_delay_ms(2000);
}

void whoAmI(void)
{
    unsigned char uByte[2]; //temp
    uint8_t buff[1] = {0x00};
    i2cread(MPU6050_ADDR, WHO_AM_I, 1, uByte);
    if (uByte[0] == 0x12)
    {
        printf("I'm ICM20602 0x%x 0x%x\r\n", uByte[0], uByte[1]);
        set_LED(2, 20);
    }
    else
    {
        printf("no ICM20602\r\n");
        set_LED(2, 2);

        //disable all balancing function
        disableBalancing = 1;
    }
}

short DataSynthesis(unsigned char REG_Address)
{
    unsigned char uByte[2];

    i2cread(MPU6050_ADDR, REG_Address, 2, uByte);

    return ((uByte[0] << 8) + uByte[1]);
}

/*
* 读取加速度数据
*/
short ReadAcceData(axis_t axis)
{
    short temp = 0;

    if (axis == x_Axis)
        temp = DataSynthesis(ACCEL_XOUT_H);
    else if (axis == y_Axis)
        temp = DataSynthesis(ACCEL_YOUT_H);
    else if (axis == z_Axis)
        temp = DataSynthesis(ACCEL_ZOUT_H);

    return temp;
}

/*
* 读取陀螺仪角速度数据
*/
short ReadGyroData(axis_t axis)
{
    short temp = 0;

    if (axis == x_Axis)
        temp = DataSynthesis(GYRO_XOUT_H);
    else if (axis == y_Axis)
        temp = DataSynthesis(GYRO_YOUT_H);
    else if (axis == z_Axis)
        temp = DataSynthesis(GYRO_ZOUT_H);

    return temp;
}
void MPU6050_Pose(void)
{
    g_fGyro_x = ReadGyroData(x_Axis);  //陀螺仪x轴
    g_fAccel_y = ReadAcceData(y_Axis); //加速度y轴
    g_fAccel_z = ReadAcceData(z_Axis); //加速度z轴
}

#ifndef MOTOR_H__
#define MOTOR_H__
#include "nrf_gpio.h"

// #include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif
#define L_encoder 0
#define R_encoder 1

// #include <stdint.h>
void InitPWM(void);

void Encoder_Init(void);
int32_t SwRotaryEncoder_read(void);
void SwRotaryEncoder_clearAbs(uint8_t LR_encoder);
int32_t SwRotaryEncoder_readAbs(uint8_t LR_encoder);
void motorPWM(uint8_t motor, int pwm);

// void setMotor(uint8_t nMotor, uint8_t PWM_value);
// void setMotorDirection(uint8_t nMotor, uint8_t direction);
// void setMotorRunTime(uint8_t nMotor, uint16_t runTime);
// void setRGB(uint8_t R,uint8_t G,uint8_t B);


#ifdef __cplusplus
}
#endif

#endif // MOTOR_H__

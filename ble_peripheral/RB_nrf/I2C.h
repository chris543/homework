#ifndef I2C_H__
#define I2C_H__

#include <stdint.h>
#include <stdbool.h>
#include "nrf_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRUE  0
#define FALSE -1

void i2cInit(void);
uint8_t BMP180_read8(uint8_t reg);


bool i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data);

int8_t i2cwrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data);
int8_t i2cread(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif // I2C_H__

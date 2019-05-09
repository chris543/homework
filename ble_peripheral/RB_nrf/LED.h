#ifndef LED_H__
#define LED_H__

#include <stdint.h>
#include <stdbool.h>
#include "nrf_gpio.h"
#define LED_blink 1
#define LED_ON 2
#define LED_OFF 3

#define LED_RED 7
#define LED_GREEN 8
#define LED_BLUE 9
#define BLE_Status_LED 13

#ifdef __cplusplus
extern "C" {
#endif

void LED_Init(void);
void set_LED(uint8_t numberOfLED, uint8_t LEDPeriodTime);
void BleStatusLed(uint8_t action);

#ifdef __cplusplus
}
#endif

#endif // LED_H__

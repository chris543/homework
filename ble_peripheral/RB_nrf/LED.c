#include "LED.h"
#include "app_timer.h"
// #include "boards.h"
// #include "app_uart.h"
// #include "misc.h"

#define BLE_LED_CONDITION_TIMER_INTERVAL APP_TIMER_TICKS(200) // 500 ms intervals
uint8_t LED[3];
#define LED_red_pin 7
#define LED_green_pin 8
#define LED_blue_pin 9

APP_TIMER_DEF(m_LED_RED_id);   //LED Red
APP_TIMER_DEF(m_LED_GREEN_id); // LED Green
APP_TIMER_DEF(m_LED_BLUE_id);  // LED Blue
APP_TIMER_DEF(m_BLE_LED_condition_id);

void BLE_LED_handler(void *p_context)
{
    //Toggle LED pin
    nrf_gpio_pin_toggle(BLE_Status_LED);
}

void LED_Red_handler(void *p_context)
{
    nrf_gpio_pin_toggle(LED_red_pin);
}
void LED_Green_handler(void *p_context)
{
    nrf_gpio_pin_toggle(LED_green_pin);
}
void LED_Blue_handler(void *p_context)
{
    nrf_gpio_pin_toggle(LED_blue_pin);
}

/* LED init
indicate bluetooth status LED
RGB LED
*/
void LED_Init(void)
{

    nrf_gpio_cfg_output(LED_red_pin); //for measure duration
    nrf_gpio_pin_set(LED_red_pin);
    nrf_gpio_cfg_output(LED_green_pin); //for measure duration
    nrf_gpio_pin_set(LED_green_pin);
    nrf_gpio_cfg_output(LED_blue_pin); //for measure duration
    nrf_gpio_pin_set(LED_blue_pin);

    nrf_gpio_cfg_output(BLE_Status_LED); //for measure duration
    // nrf_gpio_pin_set(BLE_Status_LED);
    nrf_gpio_pin_clear(BLE_Status_LED);

    //BLE LED condtion
    app_timer_create(&m_BLE_LED_condition_id, APP_TIMER_MODE_REPEATED, BLE_LED_handler);
    //  BLE LED condition
    BleStatusLed(LED_blink);

    //RGB LED app timer config
    app_timer_create(&m_LED_RED_id, APP_TIMER_MODE_REPEATED, LED_Red_handler);
    app_timer_create(&m_LED_GREEN_id, APP_TIMER_MODE_REPEATED, LED_Green_handler);
    app_timer_create(&m_LED_BLUE_id, APP_TIMER_MODE_REPEATED, LED_Blue_handler);
    //test
    // set_LED(0, 5);
    // set_LED(1, 12);
    // set_LED(2, 23);

    set_LED(0, 0);
    set_LED(1, 0);
    set_LED(2, 0);
}

/**
 * @brief RGB LED blink
 *
 * @param[numberOfLED] number of LED
 * @param[LEDPeriodTime] value * 100 ms
 */
void set_LED(uint8_t numberOfLED, uint8_t LEDPeriodTime)
{
    LED[numberOfLED] = LEDPeriodTime;
    // printf("LED %d %d\n", numberOfLED, LEDPeriodTime );
    switch (numberOfLED)
    {
    case 0:
        if (LED[numberOfLED] == 0)
        {
            app_timer_stop(m_LED_RED_id);
            nrf_gpio_pin_clear(LED_red_pin);
        }
        else if (LED[numberOfLED] == 255)
        {
            app_timer_stop(m_LED_RED_id);
            nrf_gpio_pin_set(LED_red_pin);
        }
        else
        {
            app_timer_stop(m_LED_RED_id);
            app_timer_start(m_LED_RED_id, APP_TIMER_TICKS(LEDPeriodTime * 100), NULL);
        }

        break;
    case 1:
        if (LED[numberOfLED] == 0)
        {
            app_timer_stop(m_LED_GREEN_id);
            nrf_gpio_pin_clear(LED_green_pin);
        }
        else if (LED[numberOfLED] == 255)
        {
            app_timer_stop(m_LED_GREEN_id);
            nrf_gpio_pin_set(LED_green_pin);
        }
        else
        {
            app_timer_stop(m_LED_GREEN_id);
            app_timer_start(m_LED_GREEN_id, APP_TIMER_TICKS(LEDPeriodTime * 100), NULL);
        }
        break;
    case 2:
        if (LED[numberOfLED] == 0)
        {
            app_timer_stop(m_LED_BLUE_id);
            nrf_gpio_pin_clear(LED_blue_pin);
        }
        else if (LED[numberOfLED] == 255)
        {
            app_timer_stop(m_LED_BLUE_id);
            nrf_gpio_pin_set(LED_blue_pin);
        }
        else
        {
            app_timer_stop(m_LED_BLUE_id);
            app_timer_start(m_LED_BLUE_id, APP_TIMER_TICKS(LEDPeriodTime * 100), NULL);
        }
        break;
    }
}

void BleStatusLed(uint8_t action)
{
    switch (action)
    {
    case LED_blink:
        app_timer_start(m_BLE_LED_condition_id, BLE_LED_CONDITION_TIMER_INTERVAL, NULL);

        break;
    case LED_ON:
        app_timer_stop(m_BLE_LED_condition_id);
        nrf_gpio_pin_clear(BLE_Status_LED);

        break;
    case LED_OFF:
        app_timer_stop(m_BLE_LED_condition_id);

        break;
    case 4:

        break;
    default:

        break;
    }
}

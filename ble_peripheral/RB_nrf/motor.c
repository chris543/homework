#include "motor.h"
#include "nrf_drv_pwm.h"
#include "nrf_drv_gpiote.h"

#define L_AIN2 21
#define L_AIN1 22
#define R_BIN1 23
#define R_BIN2 24
#define STBY 28

#define L_MOTOR 17
#define R_MOTOR 18
#define PWM3 19
#define PWM4 20

#define L_phaseA 3
#define L_phaseB 4
#define R_phaseA 5
#define R_phaseB 6

static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);

#define USED_PWM(idx) (1UL << idx)
static uint8_t m_used = 0;
static uint16_t const max_PWM_step = 799;
static nrf_pwm_values_individual_t motor_seq_values;
static nrf_pwm_sequence_t const m_demo1_seq =
    {
        .values.p_individual = &motor_seq_values,
        .length = NRF_PWM_VALUES_LENGTH(motor_seq_values),
        .repeats = 0,
        .end_delay = 0};

uint8_t L_a_last, R_a_last;
int32_t L_abs, R_abs;
int32_t L_last_read, R_last_read; // debouncing
void InitPWM(void)
{
    uint32_t err_code;

    nrf_gpio_cfg_output(L_AIN2); //for measure duration
    nrf_gpio_cfg_output(L_AIN1); //for measure duration
    nrf_gpio_cfg_output(R_BIN1); //for measure duration
    nrf_gpio_cfg_output(R_BIN2); //for measure duration

    nrf_gpio_pin_clear(L_AIN2);
    nrf_gpio_pin_clear(L_AIN2);
    nrf_gpio_pin_clear(R_BIN1);
    nrf_gpio_pin_clear(R_BIN2);

    // // // nrf_gpio_pin_clear(L_AIN2);
    // // nrf_gpio_pin_clear(L_AIN1);
    // // // nrf_gpio_pin_clear(R_BIN1);
    // // nrf_gpio_pin_clear(R_BIN2);

    // // nrf_gpio_pin_set(L_AIN2);
    // // // nrf_gpio_pin_set(L_AIN1);
    // // nrf_gpio_pin_set(R_BIN1);
    // // // nrf_gpio_pin_set(R_BIN2);

    // nrf_gpio_pin_clear(L_AIN2);
    // // nrf_gpio_pin_clear(L_AIN1);
    // nrf_gpio_pin_clear(R_BIN1);
    // // nrf_gpio_pin_clear(R_BIN2);

    // // nrf_gpio_pin_set(L_AIN2);
    // nrf_gpio_pin_set(L_AIN1);
    // // nrf_gpio_pin_set(R_BIN1);
    // nrf_gpio_pin_set(R_BIN2);

    nrf_gpio_cfg_output(STBY); //for measure duration
    nrf_gpio_pin_set(STBY);    //enable TB6612FNG

    nrf_drv_pwm_config_t const config0 =
        {
            .output_pins =
                {
                    L_MOTOR | NRF_DRV_PWM_PIN_INVERTED, // channel 0
                    R_MOTOR | NRF_DRV_PWM_PIN_INVERTED, // channel 1
                    PWM3 | NRF_DRV_PWM_PIN_INVERTED,    // channel 2
                    PWM4 | NRF_DRV_PWM_PIN_INVERTED     // channel 3
                },
            .irq_priority = APP_IRQ_PRIORITY_LOW,
            .base_clock = NRF_PWM_CLK_8MHz,
            .count_mode = NRF_PWM_MODE_UP,
            .top_value = max_PWM_step,
            .load_mode = NRF_PWM_LOAD_INDIVIDUAL,
            .step_mode = NRF_PWM_STEP_AUTO};
    // err_code = nrf_drv_pwm_init(&m_pwm0, &config0, demo1_handler);
    err_code = nrf_drv_pwm_init(&m_pwm0, &config0, NULL);
    APP_ERROR_CHECK(err_code);
    m_used |= USED_PWM(0);

    nrf_drv_pwm_simple_playback(&m_pwm0, &m_demo1_seq, 1,
                                NRF_DRV_PWM_FLAG_LOOP);
}
void motorPWM(uint8_t motor, int pwm)
{
    switch (motor)
    {
    case 0:
        motor_seq_values.channel_0 = 799-pwm;
        break;
    case 1:
        motor_seq_values.channel_1 = 799-pwm;
        break;
    case 2:
        motor_seq_values.channel_2 = 799-pwm;
        break;
    case 3:
        motor_seq_values.channel_3 = 799-pwm;
        break;
    }
}
void SwRotaryEncoder_clearAbs(uint8_t LR_encoder)
{
    switch (LR_encoder)
    {
    case 0:
        L_abs = 0;
        break;
    case 1:
        R_abs = 0;
        break;
    }
}

int32_t SwRotaryEncoder_read(void)
{
    int32_t ret = R_abs - R_last_read;
    R_last_read = R_abs;
    return ret;
}

int32_t SwRotaryEncoder_readAbs(uint8_t LR_encoder)
{
    switch (LR_encoder)
    {
    case 0:
        return L_abs;
        break;
    case 1:
        return R_abs;
        break;
    default:
        return 0;
        break;
    }
}

void L_encoder_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{

    uint8_t bita = nrf_gpio_pin_read(L_phaseA);

    if (bita != L_a_last)
    {
        int32_t step;

        if (bita != nrf_gpio_pin_read(L_phaseB))
        {
            step = 1;
        }
        else
        {
            step = -1;
        }

        L_abs += step;
        L_a_last = bita;
    }
    // printf("L encoder:%d\r\n", SwRotaryEncoder_readAbs(L_encoder));
}

void R_encoder_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    uint8_t bita = nrf_gpio_pin_read(R_phaseA);

    if (bita != R_a_last)
    {
        int32_t step;

        if (bita != nrf_gpio_pin_read(R_phaseB))
        {
            step = 1;
        }
        else
        {
            step = -1;
        }

        R_abs += step;
        R_a_last = bita;
    }

    // printf("R encoder:%d\r\n", SwRotaryEncoder_readAbs(R_encoder));
}

void Encoder_Init(void)
{
    //use gpiote with interrupt
    uint32_t err_code;
    //ultrasonic trig P0.30 echo P0.30
    // set up HC-SR04 pins
    nrf_gpio_pin_dir_set(L_phaseA, NRF_GPIO_PIN_DIR_INPUT);
    nrf_gpio_pin_dir_set(L_phaseB, NRF_GPIO_PIN_DIR_INPUT);

    nrf_gpio_pin_dir_set(R_phaseA, NRF_GPIO_PIN_DIR_INPUT);
    nrf_gpio_pin_dir_set(R_phaseB, NRF_GPIO_PIN_DIR_INPUT);
    // nrf_gpio_pin_pull_t con = GPIO_PIN_CNF_PULL_Pulldown;
    // nrf_gpio_cfg_input(pinEcho, con);

    uint32_t ppi_event_addr;
    nrf_drv_gpiote_in_config_t configL = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    err_code = nrf_drv_gpiote_in_init(L_phaseA, &configL, L_encoder_event_handler);
    if (err_code != NRF_SUCCESS)
    {
        // handle error condition
    }

    nrf_drv_gpiote_in_config_t configR = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    err_code = nrf_drv_gpiote_in_init(R_phaseA, &configR, R_encoder_event_handler);
    if (err_code != NRF_SUCCESS)
    {
        // handle error condition
    }

    ppi_event_addr = nrf_drv_gpiote_in_event_addr_get(L_phaseA);
    ppi_event_addr = nrf_drv_gpiote_in_event_addr_get(R_phaseA);
    // Event and corresponding interrupt are enabled.
    nrf_drv_gpiote_in_event_enable(L_phaseA, true);
    nrf_drv_gpiote_in_event_enable(R_phaseA, true);
}

// if (action == GPIOTE_CONFIG_POLARITY_LoToHi)
// {
//     tCount = 0;
//     nrf_drv_gpiote_in_uninit(pinEcho);

//     nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
//     nrf_drv_gpiote_in_init(pinEcho, &config, pin_event_handler);
//     nrf_drv_gpiote_in_event_enable(pinEcho, true);

//     // nrf_gpio_pin_clear(19);
// }
// else if (action == GPIOTE_CONFIG_POLARITY_HiToLo)
// {
//     nrf_drv_gpiote_in_uninit(pinEcho);
//     nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
//     nrf_drv_gpiote_in_init(pinEcho, &config, pin_event_handler);
//     nrf_drv_gpiote_in_event_enable(pinEcho, true);

//     // nrf_gpio_pin_set(19);
//     EchoComplete = true;

//     float duration = countToUs * tCount;
//     distance = duration * 0.017;
// }
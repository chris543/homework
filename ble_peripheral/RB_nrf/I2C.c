#include "I2C.h"
#include "nrf_twi_mngr.h"

//custom define
#define TWI_INSTANCE_ID 0

#define MAX_PENDING_TRANSACTIONS 5
NRF_TWI_MNGR_DEF(m_nrf_twi_mngr, MAX_PENDING_TRANSACTIONS, TWI_INSTANCE_ID);
static const nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(0);

void i2cInit(void)
{
    // uint32_t err_code;

    // nrf_drv_twi_config_t const config = {
    //     .scl = 26,
    //     .sda = 27,
    //     .frequency = NRF_DRV_TWI_FREQ_400K,
    //     .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
    //     .clear_bus_init = false};

    // err_code = nrf_twi_mngr_init(&m_nrf_twi_mngr, &config);
    // APP_ERROR_CHECK(err_code);

    ret_code_t ret;
    const nrf_drv_twi_config_t config =
        {
            .scl = 26,
            .sda = 27,
            .frequency = NRF_DRV_TWI_FREQ_400K,
            .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
            .clear_bus_init = false};

    ret = nrf_drv_twi_init(&m_twi_master, &config, NULL, NULL);

    if (NRF_SUCCESS == ret)
    {
        nrf_drv_twi_enable(&m_twi_master);
    }

    // return ret;
}

#define BMP085_I2CADDR 0x69
static uint8_t m_buffer[3];
static uint8_t reg_cmd[2];

// nrf_twi_mngr_transfer_t const BMP180_t8[] =
//     {
//         NRF_TWI_MNGR_WRITE(BMP085_I2CADDR, reg_cmd, 1, 0),
//         NRF_TWI_MNGR_READ(BMP085_I2CADDR, m_buffer, 1, 0)};

static void read_mma7660_registers_cb(ret_code_t result, void *p_user_data)
{
    if (result != NRF_SUCCESS)
    {
        // NRF_LOG_WARNING("read_mma7660_registers_cb - error: %d", (int)result);
        printf("error\r\n");

        return;
    }

    // NRF_LOG_DEBUG("MMA7660:");
    // NRF_LOG_HEXDUMP_DEBUG(m_buffer, MMA7660_NUMBER_OF_REGISTERS);

    printf("who am i %x\r\n", m_buffer[0]);
}

uint8_t BMP180_read8(uint8_t reg)
{
    // uint32_t err_code;
    // uint8_t ret;
    reg_cmd[0] = reg;

    // err_code = nrf_twi_mngr_perform(&m_nrf_twi_mngr, NULL, BMP180_t8, sizeof(BMP180_t8) / sizeof(BMP180_t8[0]), NULL);
    // if (err_code == NRF_SUCCESS)
    // {
    //     ret = m_buffer[0];
    //     return ret;
    // }
    // return 0;

    static nrf_twi_mngr_transfer_t const transfers[] =
        {
            // MMA7660_READ(&mma7660_xout_reg_addr,
            //              m_buffer, MMA7660_NUMBER_OF_REGISTERS)
            NRF_TWI_MNGR_WRITE(BMP085_I2CADDR, reg_cmd, 1, 0),
            NRF_TWI_MNGR_READ(BMP085_I2CADDR, m_buffer, 1, 0)};

    static nrf_twi_mngr_transaction_t NRF_TWI_MNGR_BUFFER_LOC_IND transaction =
        {
            .callback = read_mma7660_registers_cb,
            .p_user_data = NULL,
            .p_transfers = transfers,
            .number_of_transfers = sizeof(transfers) / sizeof(transfers[0])};

    APP_ERROR_CHECK(nrf_twi_mngr_schedule(&m_nrf_twi_mngr, &transaction));

    return 0;
}

bool i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
    ret_code_t ret;
    uint16_t addr16 = reg;
    ret = nrf_drv_twi_tx(&m_twi_master, addr, (uint8_t *)&reg, len, true);

    if (NRF_SUCCESS != ret)
    {
        return -1;
    }
    ret = nrf_drv_twi_rx(&m_twi_master, addr, data, 1);

    return ret;
}
//ret: 0 ok
//ret : other value error
int8_t i2cread(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    ret_code_t ret;
    ret = nrf_drv_twi_tx(&m_twi_master, addr, (uint8_t *)&reg, 1, true);
    if (NRF_SUCCESS != ret)
    {
        return FALSE;
    }
    ret = nrf_drv_twi_rx(&m_twi_master, addr, buf, len);
    return ret;
}

int8_t i2cwrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
    ret_code_t ret;
    uint8_t regPack[5] = {reg, 0x00, 0x00, 0x00, 0x00};
    memcpy(regPack + 1, data, len);
    ret = nrf_drv_twi_tx(&m_twi_master, addr, regPack, len + 1, false);
    if (NRF_SUCCESS != ret)
    {
        return FALSE;
    }
    return ret;
}
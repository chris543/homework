
#ifndef OUR_SERVICE_H__
#define OUR_SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"

#define BLE_UUID_OUR_BASE_UUID              {0x23, 0xD1, 0x13, 0xEF, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00} // 128-bit base UUID
#define BLE_UUID_OUR_SERVICE                0xABCD // Just a random, but recognizable value
#define BLE_UUID_TXRX_SERVICE                0xFA12 // Just a random, but recognizable value

#define BLE_UUID_OUR_CHARACTERISTC_UUID          0xBEEF // Just a random, but recognizable value
#define BLE_UUID_SHIELD_UUID          0xEABC // Just a random, but recognizable value

#define BLE_UUID_NUS_TX_CHARACTERISTIC 0x0002                      /**< The UUID of the TX Characteristic. */
#define BLE_UUID_NUS_RX_CHARACTERISTIC 0x0003

typedef struct ble_os_t ble_nus_t;

typedef void (*ble_nus_data_handler_t) (ble_nus_t * p_our_service, uint8_t * p_data, uint16_t length);

typedef struct
{
	ble_nus_data_handler_t data_handler; /**< Event handler to be called for handling received data. */
} ble_nus_init_t;

struct ble_os_t
{
	uint8_t                  	uuid_type;
	uint16_t                    conn_handle;
	uint16_t					service_handle;     /**< Handle of Our Service (as provided by the BLE stack). */
	bool                    	is_notification_enabled;
	ble_gatts_char_handles_t	char_handles;
	ble_gatts_char_handles_t	tx_handles;
	ble_gatts_char_handles_t	rx_handles;
	ble_nus_data_handler_t   	data_handler;
} ;




void our_service_init (ble_nus_t * p_our_service);
void TXRX_service_init (ble_nus_t * p_our_service, const ble_nus_init_t * p_nus_init);
//void ble_our_service_on_ble_evt(ble_nus_t * p_our_service, ble_evt_t * p_ble_evt);
void ble_our_service_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context);

#endif  /* _ OUR_SERVICE_H__ */
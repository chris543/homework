#include <string.h>
#include "our_service.h"
#include "app_error.h"
#include "app_timer.h"

static void tx_char_add(ble_nus_t *p_our_service);
static void rx_char_add(ble_nus_t *p_our_service);
void ble_our_service_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context)
{
	ble_nus_t *p_our_service = (ble_nus_t *)p_context;

	//void ble_our_service_on_ble_evt(ble_nus_t * p_our_service, ble_evt_t * p_ble_evt) {

	ble_gatts_evt_write_t *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

	switch (p_ble_evt->header.evt_id)
	{
	case BLE_GAP_EVT_CONNECTED:
		p_our_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
		// SEGGER_RTT_printf(0, "\r\nconn_handle: %#x\n\r", p_our_service->conn_handle);
		printf("OurService Connected.\n");

		break;
	case BLE_GAP_EVT_DISCONNECTED:
		p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
		// SEGGER_RTT_printf(0, "\r\nconn_handle: %#x\n\r", p_our_service->conn_handle);
		printf("OurService Disonnected.\n");
		break;
	case BLE_GATTS_EVT_WRITE:
		if ((p_evt_write->handle == p_our_service->tx_handles.value_handle)) //&&(p_our_service->data_handler != NULL))
		{
			p_our_service->data_handler(p_our_service, p_evt_write->data, p_evt_write->len);
			// SEGGER_RTT_printf(0, "\r\ndata %x\n\r", p_our_service->tx_handles.value_handle);
			// SEGGER_RTT_printf(0, "\r\ndata %#x\n\r", p_evt_write->data[2]);
			printf("print data.\n");
		}
		// printf("char_handles %x\r\n",p_our_service->char_handles.cccd_handle);
	case BLE_GATTS_EVT_HVN_TX_COMPLETE:
		// printf("hello: \r\n");
		break;
		// sd_ble_gatts_value_get(p_our_service->conn_handle,vv,p_our_service);
		// printf("BLE_GATTS_EVT_WRITE %x\r\n",p_our_service->tx_handles.value_handle);
		// printf("BLE_GATTS_EVT_WRITE %x\r\n",vv);
		// sd_ble_gatts_value_get(uint16_t conn_handle, uint16_t handle, ble_gatts_value_t *p_value));
		//   sd_ble_gatts_hvx(uint16_t conn_handle, ble_gatts_hvx_params_t const *p_hvx_params));
		//   sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);

		break;
	default:
		// No implementation needed.
		break;
	}
}

void our_char_add(ble_nus_t *p_our_service)
{
	uint32_t err_code;

	//-------add second characteristic
	ble_uuid_t char1_uuid;
	ble_uuid128_t base1_uuid = BLE_UUID_OUR_BASE_UUID;
	char1_uuid.uuid = BLE_UUID_SHIELD_UUID;
	err_code = sd_ble_uuid_vs_add(&base1_uuid, &char1_uuid.type);
	APP_ERROR_CHECK(err_code);

	ble_gatts_char_md_t char1_md;
	memset(&char1_md, 0, sizeof(char1_md));
	char1_md.char_props.read = 1;
	char1_md.char_props.write = 1;

	ble_gatts_attr_md_t attr1_md;
	memset(&attr1_md, 0, sizeof(attr1_md));
	attr1_md.vloc = BLE_GATTS_VLOC_STACK;

	ble_gatts_attr_t attr1_char_value;
	memset(&attr1_char_value, 0, sizeof(attr1_char_value));
	attr1_char_value.p_uuid = &char1_uuid;
	attr1_char_value.p_attr_md = &attr1_md;

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr1_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr1_md.write_perm);

	attr1_char_value.max_len = 20;
	attr1_char_value.init_len = 20;
	uint8_t value1[20] = {0};
	attr1_char_value.p_value = value1;

	//-----
	err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
											   &char1_md,
											   &attr1_char_value,
											   &p_our_service->char_handles);
	APP_ERROR_CHECK(err_code);

	ble_uuid_t char_uuid;
	ble_uuid128_t base_uuid = BLE_UUID_OUR_BASE_UUID;
	char_uuid.uuid = BLE_UUID_OUR_CHARACTERISTC_UUID;
	err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
	APP_ERROR_CHECK(err_code);

	// OUR_JOB: Step 2.F Add read/write properties to our characteristic
	ble_gatts_char_md_t char_md;
	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.read = 1;
	char_md.char_props.write = 1;

	ble_gatts_attr_md_t attr_md;
	memset(&attr_md, 0, sizeof(attr_md));
	attr_md.vloc = BLE_GATTS_VLOC_STACK;

	ble_gatts_attr_t attr_char_value;
	memset(&attr_char_value, 0, sizeof(attr_char_value));
	attr_char_value.p_uuid = &char_uuid;
	attr_char_value.p_attr_md = &attr_md;
	//-----------
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

	attr_char_value.max_len = 4;
	attr_char_value.init_len = 4;
	uint8_t value[4] = {0x12, 0x34, 0x56, 0x78};
	attr_char_value.p_value = value;
	//---------------CCCD
	ble_gatts_attr_md_t cccd_md;
	memset(&cccd_md, 0, sizeof(cccd_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
	cccd_md.vloc = BLE_GATTS_VLOC_STACK;
	char_md.p_cccd_md = &cccd_md;
	char_md.char_props.notify = 1;

	//----
	err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
											   &char_md,
											   &attr_char_value,
											   &p_our_service->char_handles);
	APP_ERROR_CHECK(err_code);

	// SEGGER_RTT_printf(0, "\r\nService handle: %#x\n\r", p_our_service->service_handle);
	// SEGGER_RTT_printf(0, "Char value handle: %#x\r\n", p_our_service->char_handles.value_handle);
	// SEGGER_RTT_printf(0, "Char cccd handle: %#x\r\n\r\n", p_our_service->char_handles.cccd_handle);
}

void our_service_init(ble_nus_t *p_our_service)
{

	uint32_t err_code;
	ble_uuid_t service_uuid;
	ble_uuid128_t base_uuid = BLE_UUID_OUR_BASE_UUID;
	service_uuid.uuid = BLE_UUID_OUR_SERVICE;
	err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
	APP_ERROR_CHECK(err_code);

	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
										&service_uuid,
										&p_our_service->service_handle);
	APP_ERROR_CHECK(err_code);
	//3.B
	p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;

	our_char_add(p_our_service);
	printf("Exectuing our_service_init().\n");
	// SEGGER_RTT_printf(0, "Service UUID: 0x%#04x\n", service_uuid.uuid);
	// SEGGER_RTT_printf(0, "Service UUID type: 0x%#02x\n", service_uuid.type);
	// SEGGER_RTT_printf(0, "Service handle: 0x%#04x\n", p_our_service->service_handle);
}

void TXRX_service_init(ble_nus_t *p_our_service, const ble_nus_init_t *p_nus_init)
{

	// uint32_t ble_nus_init(ble_nus_t * p_nus, const ble_nus_init_t * p_nus_init)

	uint32_t err_code;
	ble_uuid_t ble_uuid;
	ble_uuid128_t nus_base_uuid = BLE_UUID_OUR_BASE_UUID;

	// VERIFY_PARAM_NOT_NULL(p_our_service);
	// VERIFY_PARAM_NOT_NULL(p_nus_init);

	// Initialize the service structure.
	p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
	p_our_service->data_handler = p_nus_init->data_handler;
	p_our_service->is_notification_enabled = false;

	/**@snippet [Adding proprietary Service to S110 SoftDevice] */
	// Add a custom base UUID.
	err_code = sd_ble_uuid_vs_add(&nus_base_uuid, &p_our_service->uuid_type);
	APP_ERROR_CHECK(err_code);

	ble_uuid.type = p_our_service->uuid_type;
	ble_uuid.uuid = BLE_UUID_TXRX_SERVICE;

	// Add the service.
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
										&ble_uuid,
										&p_our_service->service_handle);
	/**@snippet [Adding proprietary Service to S110 SoftDevice] */
	APP_ERROR_CHECK(err_code);

	// Add the RX Characteristic.
	rx_char_add(p_our_service);
	APP_ERROR_CHECK(err_code);

	// Add the TX Characteristic.
	tx_char_add(p_our_service);
	APP_ERROR_CHECK(err_code);
}

static void rx_char_add(ble_nus_t *p_our_service)
{
	ble_gatts_char_md_t char_md;
	ble_gatts_attr_md_t cccd_md;
	ble_gatts_attr_t attr_char_value;
	ble_uuid_t char_uuid;
	ble_gatts_attr_md_t attr_md;

	memset(&cccd_md, 0, sizeof(cccd_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

	cccd_md.vloc = BLE_GATTS_VLOC_STACK;

	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.read = 1;
	char_md.char_props.write = 1;

	char_md.p_cccd_md = &cccd_md;
	char_md.char_props.notify = 1;

	char_md.p_char_user_desc = NULL;
	char_md.p_char_pf = NULL;
	char_md.p_user_desc_md = NULL;
	// char_md.p_cccd_md         = &cccd_md;
	char_md.p_sccd_md = NULL;

	char_uuid.type = p_our_service->uuid_type; //**
	char_uuid.uuid = BLE_UUID_NUS_RX_CHARACTERISTIC;

	memset(&attr_md, 0, sizeof(attr_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

	attr_md.vloc = BLE_GATTS_VLOC_STACK;

	attr_md.rd_auth = 0;
	attr_md.wr_auth = 0;
	attr_md.vlen = 1;

	memset(&attr_char_value, 0, sizeof(attr_char_value));

	attr_char_value.p_uuid = &char_uuid;
	attr_char_value.p_attr_md = &attr_md;

	attr_char_value.max_len = 20;
	attr_char_value.init_len = 20;
	uint8_t value[20] = {0};
	attr_char_value.p_value = value;

	attr_char_value.init_offs = 0;

	sd_ble_gatts_characteristic_add(p_our_service->service_handle,
									&char_md,
									&attr_char_value,
									&p_our_service->char_handles);

	// static uint32_t rx_char_add(ble_nus_t * p_nus, const ble_nus_init_t * p_nus_init)
	// ble_gatts_char_md_t char_md;
	// ble_gatts_attr_md_t cccd_md;
	// ble_gatts_attr_t    attr_char_value;
	// ble_uuid_t          ble_uuid;
	// ble_gatts_attr_md_t attr_md;

	// memset(&cccd_md, 0, sizeof(cccd_md));

	// BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	// BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

	// cccd_md.vloc = BLE_GATTS_VLOC_STACK;

	// memset(&char_md, 0, sizeof(char_md));
	// char_md.char_props.read = 1;
	// char_md.char_props.write = 1;

	// char_md.char_props.notify = 1;
	// char_md.p_char_user_desc  = NULL;
	// char_md.p_char_pf         = NULL;
	// char_md.p_user_desc_md    = NULL;
	// char_md.p_cccd_md         = &cccd_md;
	// char_md.p_sccd_md         = NULL;

	// ble_uuid.type = p_our_service->uuid_type;
	// ble_uuid.uuid = BLE_UUID_NUS_RX_CHARACTERISTIC;

	// memset(&attr_md, 0, sizeof(attr_md));

	// BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	// BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

	// attr_md.vloc    = BLE_GATTS_VLOC_STACK;
	// attr_md.rd_auth = 0;
	// attr_md.wr_auth = 0;
	// attr_md.vlen    = 1;

	// memset(&attr_char_value, 0, sizeof(attr_char_value));

	// attr_char_value.p_uuid    = &ble_uuid;
	// attr_char_value.p_attr_md = &attr_md;
	// attr_char_value.init_len  = sizeof(uint8_t);
	// attr_char_value.init_offs = 0;
	// attr_char_value.max_len   = 20;

	// sd_ble_gatts_characteristic_add(p_our_service->service_handle,
	//                                 &char_md,
	//                                 &attr_char_value,
	//                                 &p_our_service->char_handles);
}

static void tx_char_add(ble_nus_t *p_our_service)
{
	//-------add second characteristic
	ble_gatts_char_md_t char_md;
	ble_gatts_attr_t attr_char_value;
	ble_uuid_t char_uuid;
	ble_gatts_attr_md_t attr_md;

	memset(&char_md, 0, sizeof(char_md));

	char_md.char_props.read = 1;
	char_md.char_props.write_wo_resp = 1;
	char_md.p_char_user_desc = NULL;
	char_md.p_char_pf = NULL;
	char_md.p_user_desc_md = NULL;
	char_md.p_cccd_md = NULL;
	char_md.p_sccd_md = NULL;

	char_uuid.type = p_our_service->uuid_type;
	char_uuid.uuid = BLE_UUID_NUS_TX_CHARACTERISTIC;

	memset(&attr_md, 0, sizeof(attr_md));

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

	attr_md.vloc = BLE_GATTS_VLOC_STACK;
	attr_md.rd_auth = 0;
	attr_md.wr_auth = 0;
	attr_md.vlen = 1;

	memset(&attr_char_value, 0, sizeof(attr_char_value));

	attr_char_value.p_uuid = &char_uuid;
	attr_char_value.p_attr_md = &attr_md;
	attr_char_value.init_len = 20;
	attr_char_value.max_len = 20;
	uint8_t value[20] = {0};
	attr_char_value.p_value = value;

	sd_ble_gatts_characteristic_add(p_our_service->service_handle,
									&char_md,
									&attr_char_value,
									&p_our_service->tx_handles);

	//------------------------------------------------------------------------------------------------

	// ble_gatts_char_md_t char_md;
	// ble_gatts_attr_t    attr_char_value;
	// ble_uuid_t          ble_uuid;
	// ble_gatts_attr_md_t attr_md;

	// memset(&char_md, 0, sizeof(char_md));

	// char_md.char_props.write         = 1;
	// char_md.char_props.write_wo_resp = 1;
	// char_md.p_char_user_desc         = NULL;
	// char_md.p_char_pf                = NULL;
	// char_md.p_user_desc_md           = NULL;
	// char_md.p_cccd_md                = NULL;
	// char_md.p_sccd_md                = NULL;

	// ble_uuid.type = p_our_service->uuid_type;
	// ble_uuid.uuid = BLE_UUID_NUS_TX_CHARACTERISTIC;

	// memset(&attr_md, 0, sizeof(attr_md));

	// BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	// BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

	// attr_md.vloc    = BLE_GATTS_VLOC_STACK;
	// attr_md.rd_auth = 0;
	// attr_md.wr_auth = 0;
	// attr_md.vlen    = 1;

	// memset(&attr_char_value, 0, sizeof(attr_char_value));

	// attr_char_value.p_uuid    = &ble_uuid;
	// attr_char_value.p_attr_md = &attr_md;
	// attr_char_value.init_len  = 1;
	// attr_char_value.init_offs = 0;
	// attr_char_value.max_len   = 20;

	// sd_ble_gatts_characteristic_add(p_our_service->service_handle,
	//                                 &char_md,
	//                                 &attr_char_value,
	//                                 &p_our_service->tx_handles);
}

void our_termperature_characteristic_update(ble_nus_t *p_our_service, int32_t *temperature_value)
{
	// OUR_JOB: Step 3.E, Update characteristic value
	if (p_our_service->conn_handle != BLE_CONN_HANDLE_INVALID)
	{
		uint16_t len = 20;
		ble_gatts_hvx_params_t hvx_params;
		memset(&hvx_params, 0, sizeof(hvx_params));

		hvx_params.handle = p_our_service->char_handles.value_handle;
		hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
		hvx_params.offset = 0;
		hvx_params.p_len = &len;
		hvx_params.p_data = (uint8_t *)temperature_value;

		sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);
		// counter++;
		// SEGGER_RTT_printf(0, "counter: %d\n", counter); // Print out the service handle. Should match service handle shown in MCP under Attribute values
		// SEGGER_RTT_printf(0, "temperature_value: %04x\n", (uint8_t*)temperature_value); // Print out the service handle. Should match service handle shown in MCP under Attribute values
		// SEGGER_RTT_printf(0, "temperature_valuee: %#x\r\n", *hvx_params.p_data);

		float convert_C = (*temperature_value) * 0.25;
		char c[5];
		sprintf(c, "%.2f", convert_C);
		// SEGGER_RTT_printf(0, "temperature C: %f\n", (*temperature_value)*0.25); // Print out the service handle. Should match service handle shown in MCP under Attribute values
		// SEGGER_RTT_printf(0, "temperature C: %s\n", c); // Print out the service handle. Should match service handle shown in MCP under Attribute values
	}
}
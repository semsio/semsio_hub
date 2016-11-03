#include "sdk_config.h"
#include "ble_battery_service.h"
#include "ble_srv_common.h"
#include "sdk_common.h"

static ble_battery_service_t ble_service;

void bas_on_ble_evt(ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            ble_on_connect(&ble_service.conn_handle,p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            ble_on_disconnect(&ble_service.conn_handle,p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

static uint32_t battery_char_add(){
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    uint8_t             initial_battery_level;

    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_BATTERY_LEVEL_CHAR);

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = &initial_battery_level;

    return sd_ble_gatts_characteristic_add(ble_service.service_handle,
                                               &char_md,
                                               &attr_char_value,
                                               &ble_service.battery_char_handles);
}

uint32_t ble_battery_init()
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;
    
    
    // Add service
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_BATTERY_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_service.service_handle);
    VERIFY_SUCCESS(err_code);

    // Add characteristics.
    err_code = battery_char_add();
    VERIFY_SUCCESS(err_code);

    return NRF_SUCCESS;
}

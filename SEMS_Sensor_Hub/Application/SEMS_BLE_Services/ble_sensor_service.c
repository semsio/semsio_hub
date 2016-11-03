#include "sdk_config.h"
#include "sems.h"
#include "ble_services.h"
#include "ble_sensor_service.h"
#include "ble_srv_common.h"
#include "sdk_common.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

static ble_sensor_service_t ble_service;

void sems_ble_on_sensor_evt(ble_evt_t * p_ble_evt)
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

void sems_ble_set_sensor_data(uint8_t* data, uint16_t size){
    ble_gatts_value_t gatts_value;
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = size;
    gatts_value.offset  = 0;
    gatts_value.p_value = data;

    // Update database.
    uint32_t err_code = sd_ble_gatts_value_set(ble_service.conn_handle,
                                      ble_service.sensor_char_handles.value_handle,
                                      &gatts_value);
}

uint32_t sems_ble_sensor_service_init()
{
    uint32_t   err_code;
    ble_service.conn_handle=BLE_CONN_HANDLE_INVALID;
    // Add service.
    err_code=sems_add_service_uuid(LBS_UUID_SENSOR_SERVICE,&ble_service.uuid_type,&ble_service.service_handle);
    VERIFY_SUCCESS(err_code);
    
    // Add characteristics.
    err_code = sems_add_char_uuid(LBS_UUID_SENSOR_CHAR,&ble_service.uuid_type,&ble_service.service_handle,&ble_service.sensor_char_handles,1,0,BLE_GATT_MAX_SIZE);
    VERIFY_SUCCESS(err_code);
    
    return NRF_SUCCESS;
}

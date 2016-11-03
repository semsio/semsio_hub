#include <stdlib.h>  /* malloc, free, rand, system */
#include "sdk_config.h"
#include "sems.h"
#include "ble_services.h"
#include "ble_action_service.h"
#include "ble_srv_common.h"
#include "sdk_common.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "pb_decode.h"

static ble_action_service_t ble_service;
static sems_action_handler action_handler;

static void on_write(ble_evt_t * p_ble_evt)
{
    if(p_ble_evt->evt.gatts_evt.params.write.handle==ble_service.action_char_handles.value_handle)
    {
        NRF_LOG_INFO("receive value called \n");
        ble_gatts_evt_write_t *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
        NRF_LOG_INFO("receive value length %d \n",p_evt_write->len);
        
        sems_ActionData action_data=sems_ActionData_init_zero;
        pb_istream_t stream = pb_istream_from_buffer(p_evt_write->data, p_evt_write->len);
        bool status = pb_decode(&stream, sems_ActionData_fields, &action_data);
        NRF_LOG_INFO("decode status %d \n",status);
        NRF_LOG_FLUSH();
        if(status==true){
            if(action_handler!=NULL)
            {
                action_handler(&action_data);
            }
        }
    }
}

void sems_ble_on_action_evt(ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            ble_on_connect(&ble_service.conn_handle,p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            ble_on_disconnect(&ble_service.conn_handle,p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

uint32_t sems_ble_action_service_init()
{
    uint32_t   err_code;
    ble_service.conn_handle=BLE_CONN_HANDLE_INVALID;
    // Add service.
    err_code=sems_add_service_uuid(LBS_UUID_ACTION_SERVICE,&ble_service.uuid_type,&ble_service.service_handle);
    VERIFY_SUCCESS(err_code);
    
    // Add characteristics.
    err_code = sems_add_char_uuid(LBS_UUID_ACTION_CHAR,&ble_service.uuid_type,&ble_service.service_handle,&ble_service.action_char_handles,0,1,30);
    VERIFY_SUCCESS(err_code);
    
    return NRF_SUCCESS;
    
}

void sems_set_action_handler(sems_action_handler handler)
{
    action_handler=handler;
}

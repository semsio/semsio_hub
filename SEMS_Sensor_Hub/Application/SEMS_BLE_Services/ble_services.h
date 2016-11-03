#ifndef __BLE_SERVICES_H__
#define __BLE_SERVICES_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "sems.h"

#ifdef __cplusplus
extern "C" {
#endif
#define BLE_GATT_MAX_SIZE       128
#define BLE_BROADCAST_MAX_SIZE       27
#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define LBS_SEMS_UUID_BASE        {0x7d, 0x07, 0x79, 0xff, 0xfa, 0x95, 0x01, 0xb7, \
                              0xd0, 0x47, 0x43, 0x0f, 0x00, 0x00, 0x00, 0x00}

#define ADVERTISING_SERVICE_UUID 0xFFFF;

/**@brief BLE Service init func. */
void ble_stack_init(void);
void sems_advertising_init();
void gap_params_init(void);
void conn_params_init(void);
void sems_ble_start(void);
void sems_ble_services_init(void);

/**@brief Function for update advertising data.
 *
 * @param[in]   scan_response_data  data.
 * @param[in]   data_length   data length.
 */
void sems_update_advertising_response_data(void* scan_response_data, uint16_t data_length);

/**@brief Function for handling the Connect event.
 *
 * @param[in]   ble_conn_handle  BLE Service connection handle.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_on_connect(uint16_t *ble_conn_handle ,ble_evt_t * p_ble_evt);

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   ble_conn_handle  BLE Service connection handle.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_on_disconnect(uint16_t *ble_conn_handle ,ble_evt_t * p_ble_evt);

/**@brief Function for set GATT data.
 *
 * @param[in]   ble_conn_handle   Handle of BLE Service.
 * @param[in]   ble_char_handle   Handle of BLE characteristic.
 * @param[in]   data   data.
 * @param[in]   len   data length.
 */
uint32_t sems_set_gatt_data(uint16_t *ble_conn_handle, ble_gatts_char_handles_t *ble_char_handle, uint8_t* data, uint32_t len);

/**@brief Function for adding the BLE service.
 *
 * @param[in]   uuid        BLE service uuid.
 * @param[in]   uuid_type   BLE service uuid type.
 * @param[out]   ble_service_handle     Handle of BLE Service
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t sems_add_service_uuid(uint16_t uuid,uint8_t *uuid_type,uint16_t *ble_service_handle);
/**@brief Function for adding the BLE characteristic.
 *
 * @param[in]   uuid                    BLE Characteristic uuid.
 * @param[in]   uuid_type               BLE Characteristic uuid type.
 * @param[in]   ble_service_handle      Handle of BLE Service.
 * @param[out]   ble_char_handle         Handle of BLE characteristic.
 * @param[in]   can_read                Is characteristic can read.
 * @param[in]   can_write               Is characteristic can write.
 * @param[in]   maxlen                  Max data transmission length of characteristic.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t sems_add_char_uuid(uint32_t uuid,
                            uint8_t *uuid_type,
                            uint16_t *ble_service_handle,
                            ble_gatts_char_handles_t *ble_char_handle,
                            uint8_t can_read,
                            uint8_t can_write,
                            uint32_t maxlen);


#ifdef __cplusplus
}
#endif

#endif
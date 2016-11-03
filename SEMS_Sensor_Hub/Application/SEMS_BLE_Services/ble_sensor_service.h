#include <stdint.h>
#include <stdbool.h>
#include "sems.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_services.h"

#ifdef __cplusplus
extern "C" {
#endif
  
#define LBS_UUID_SENSOR_SERVICE     0x1520
#define LBS_UUID_SENSOR_CHAR  0x1521

/**@brief Sensor BLE service structure. This contains various status information for the service. */
typedef struct
{
    uint16_t                    service_handle;      /**< Handle of BLE Service (as provided by the BLE stack). */
    uint8_t                     uuid_type;           /**< UUID type for the LED Button Service. */
    uint16_t                    conn_handle;         /**< Handle of the current connection (as provided by the BLE stack). BLE_CONN_HANDLE_INVALID if not in a connection. */
    ble_gatts_char_handles_t    sensor_char_handles;
} ble_sensor_service_t;

/**@brief Function for handle sensor service ble event.
 *
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void sems_ble_on_sensor_evt(ble_evt_t * p_ble_evt);

/**@brief Function for set ble sensor service data.
 *
 * @param[in]   data  data.
 * @param[in]   size  data length.
 */
void sems_ble_set_sensor_data(uint8_t * data, uint16_t size);

/**@brief Function for init sensor ble service.
 */
uint32_t sems_ble_sensor_service_init();

#ifdef __cplusplus
}
#endif

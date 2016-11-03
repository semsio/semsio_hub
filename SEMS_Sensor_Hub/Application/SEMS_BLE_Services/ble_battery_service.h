#include <stdint.h>
#include <stdbool.h>
#include "sems.h"
#include "ble.h"
#include "ble_srv_common.h"

#include "ble_services.h"

#ifdef __cplusplus
extern "C" {
#endif

  /**@brief Battery BLE service structure. This contains various status information for the service. */
typedef struct
{
    uint16_t                    service_handle;      /**< Handle of BLE Service (as provided by the BLE stack). */
    uint8_t                     uuid_type;           /**< UUID type for the LED Button Service. */
    uint16_t                    conn_handle;         /**< Handle of the current connection (as provided by the BLE stack). BLE_CONN_HANDLE_INVALID if not in a connection. */
    ble_gatts_char_handles_t    battery_char_handles;
} ble_battery_service_t;

/**@brief Function for init battery ble service.
 */
uint32_t ble_battery_init();

/**@brief Function for handle battery service ble event.
 *
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void bas_on_ble_evt(ble_evt_t * p_ble_evt);

#ifdef __cplusplus
}
#endif

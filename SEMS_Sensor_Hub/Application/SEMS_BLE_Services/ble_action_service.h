#include <stdint.h>
#include <stdbool.h>
#include "sems.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "sems.pb.h"
#include "ble_services.h"

#ifdef __cplusplus
extern "C" {
#endif
  
#define LBS_UUID_ACTION_SERVICE     0x1522
#define LBS_UUID_ACTION_CHAR  0x1523

/**@brief Action BLE service structure. This contains various status information for the service. */
typedef struct
{
    uint16_t                    service_handle;      /**< Handle of BLE Service (as provided by the BLE stack). */
    uint8_t                     uuid_type;           /**< UUID type for the LED Button Service. */
    uint16_t                    conn_handle;         /**< Handle of the current connection (as provided by the BLE stack). BLE_CONN_HANDLE_INVALID if not in a connection. */
    ble_gatts_char_handles_t    action_char_handles;
} ble_action_service_t;

typedef void (*sems_action_handler)(sems_ActionData *action_data);
/**@brief Function for set action event handler.
 *
 * @param[in]   sems_action_handler  event handler func.
 */
void sems_set_action_handler(sems_action_handler handler);

/**@brief Function for handle action service ble event.
 *
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void sems_ble_on_action_evt(ble_evt_t * p_ble_evt);

/**@brief Function for init action ble service.
 */
uint32_t sems_ble_action_service_init();


#ifdef __cplusplus
}
#endif

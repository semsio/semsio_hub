#ifndef __SENSOR_HANDLER__H__
#define __SENSOR_HANDLER__H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>  /* malloc, free, rand, system */
#include "sems_sensor.h"

/**@brief Data update type enum.
 *
*/
typedef enum 
{
    BLE_UPDATE_TYPE_GATT=1, BLE_UPDATE_TYPE_ADVERTISING
}ble_update_type;

/**@brief Advertising data structure. */
typedef struct{
    sems_sensor_t const* sensor;
    uint8_t *data;
    uint16_t size;
}advertising_once_t;

/**@brief Function update gatt data in gatt_sensorlist.
 *
 * @details This func will read data from all sensors in gatt_sensorlist and set to sensor gatt service.
 */
void sems_ble_update_gatt_data();

/**@brief Function update advertising data in advertising_sensorlist.
 *
 * @details This func will read data from all sensors in advertising_sensorlist and advertising them.
 */
void sems_ble_update_advertising_data();

/**@brief Function set advertising data once.
 *
 * @details This func will advertising data once.
 * @note data will advertising 3 seconds.Then they will be erease.
 */
void sems_ble_advertising_once(sems_sensor_t const* sensor,void *data,uint16_t size);

/**@brief Function add sensor to sensor list.
 *
 * @param[in]   sensor  sensor will be add.
 * @param[in]   type    data update type.
 */
void sems_ble_add_sensor(sems_sensor_t *sensor,ble_update_type type);

/**@brief Function remove sensor from sensor list.
 *
 * @param[in]   sensor  sensor will be remove.
 * @param[in]   type    data update type.
 */
void sems_ble_remove_sensor(sems_sensor_t *sensor,ble_update_type type);

/**@brief Function remove sensor by sensortag from sensor list.
 *
 * @param[in]   sensor_tag  sensor tag.
 * @param[in]   type        data update type.
 */
void sems_ble_remove_sensor_tag(uint16_t sensor_tag,ble_update_type type);

/**@brief Handler function for handle sensor data which need transmission over gatt service.
 */
void sems_ble_gatt_sensor_event_handler();

/**@brief Handler function for handle sensor data which need transmission over advertising.
 */
void sems_ble_advertising_sensor_event_handler();

/**@brief Function for init sensor handler.
 */
uint32_t sensor_handler_init();
#endif
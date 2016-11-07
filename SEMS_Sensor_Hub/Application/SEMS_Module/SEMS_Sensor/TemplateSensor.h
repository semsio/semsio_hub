/** @file
 * @brief Template sensor for help develpor build a custom sensor.
 */
#include "sems_sensor.h"

#pragma once

///< TODO:set you sensor tag
#define SEMS_TEMPLATE_SENSOR_TAG        0x0000

/**
 * @brief holde TEMPLATE sensor result data.
 */
typedef struct
{
    //TODO: Set strust.
    uint8_t result1;
    uint8_t result2;
} sems_TEMPLATE_data_t;

/**
 * @brief Holde TEMPLATE config data.=
 */
typedef struct
{
    //TODO: Set strust.
    uint8_t config1;
    uint8_t config2;
} sems_TEMPLATE_sensor_config_t;

/**
 * @brief Build a sensor instance.
 *
 * @retval Pointer to sensor instance.
 */
sems_sensor_t* get_sems_TEMPLATE_sensor();
/** @file
 * @brief Common defines and macros for TSL2561 digital luminosity senosr.
 */
#include "TemplateSensor.h"

 /**
 * @brief Function for get data
 *
 * @param[in] p_sensor Pointer to sensor instance.
 */
static ret_code_t get_sensor_data(sems_sensor_t const *p_sensor)
{
     //TODO:Get sensor data.
    sems_TEMPLATE_data_t *p_data = p_sensor->p_sensor_data;
    p_data->result1 = 1;
    p_data->result2 = 2;
    return NRF_SUCCESS;
}

 /**
 * @brief Function for initialize sensor
 *
 * @param[in] p_sensor Pointer to sensor instance.
 */
static ret_code_t sems_TEMPLATE_sensor_init(sems_sensor_t const *p_sensor)
{
    //TODO: Initialize code if nessesary for example TWI, GPIO. If it's not just return NRF_SUCCESS;
    return NRF_SUCCESS;
}

 /**
 * @brief Function for uninitialize sensor
 *
 * @param[in]p_sensor Pointer to sensor instance.
 */
static ret_code_t sems_TEMPLATE_sensor_uninit(sems_sensor_t const *p_sensor)
{
    //TODO: Uninitialize and free memory code if nessesary. If it's not just return NRF_SUCCESS;
    return NRF_SUCCESS;
}

 /**
 * @brief Function for setup sensor event. If sensor not support event you can delete this function.
 *
 * @param[in] p_sensor           Pointer to sensor instance.
 * @param[in] enable             enable or disable sensor.
 * @param[in] p_event_config     Pointer to sensor event config for example event triger condition.
 */
ret_code_t sems_TEMPLATE_set_event(sems_sensor_t const  *p_sensor, bool enable, void * p_event_config)
{
    //TODO:Setup sensor event.
    return NRF_SUCCESS;
}

/**
 * @brief Build a sensor instance.
 *
 * @retval Pointer to sensor instance.
 */
sems_sensor_t* get_sems_TEMPLATE_sensor()
{
    //TODO:Declare (malloc) sems_sensor_t£¬sems_TEMPLATE_data_t£¬sems_TEMPLATE_sensor_config_t
    static sems_sensor_t sensor;
    static sems_TEMPLATE_data_t data;
    static sems_TEMPLATE_sensor_config_t config;
    
    //TODO:setup sensor config
    config.config1 = 0;
    config.config2 = 1;
    
    //TODO: setup sensor;
    //If your sensor can't be static you can check GPIOSensor.c¡£
    SEMS_SENSOR_INIT(sensor,SEMS_TEMPLATE_SENSOR_TAG, &config, &data, sizeof(data), get_sensor_data, sems_TEMPLATE_sensor_init, sems_TEMPLATE_sensor_uninit, sems_TEMPLATE_set_event);
    return &sensor;
}
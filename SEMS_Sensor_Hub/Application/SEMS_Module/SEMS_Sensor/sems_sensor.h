/** @file
 * @brief Common defines and macros for SEMS sensor.
 */
#ifndef __SEMS_SENSOR_H
#define __SEMS_SENSOR_H

#include "app_error.h"
#include "app_twi.h"
#include "app_timer.h"
#include "app_error.h"
#include "ble_services.h"

#define SEMS_TWI_SDA         27
#define SEMS_TWI_SCL         26
#define SEMS_APP_TIMER_PRESCALER    APP_TIMER_PRESCALER     /**< prescaler of the app timer. */

#define SEMS_CATE_SENSOR        0x00
#define SEMS_CATE_OPERATOR      

#define SEMS_TWI_TAG                 0x1000
#define SEMS_GPIO_TAG                0x2000

/**
 * @brief stensor state.
 */
typedef enum 
{
    SEMS_SENSOR_UNINITED = 0x0,     /**<sensor is uninited. */
    SEMS_SENSOR_INITED = 0x1,       /*<sensor is inited. */
    SEMS_SENSOR_POLLING = 0x2       /*<sensor is polling */
}semf_sensor_state_t;
        
/**
 * @brief sems sensor descriptor.
 */
typedef struct sems_sensor_s sems_sensor_t;

/**
 * @brief SEMS sensor data callback prototype.
 *
 * @param[in] p_source    Pointer to the data source.
 * @param[in] p_data      Pointer to the sensor data instance.
 * @param[in] err_code    Result state of data.(NRF_SUCCESS on success,
 *                        otherwise a relevant error code).    
 */
typedef void(*sems_sensor_data_handler_t)(sems_sensor_t const* p_source, void* p_data, ret_code_t err_code);

/**
 * @brief SEMS sensor event data callback prototype.
 *
 * @param[in] p_source    Pointer to the data source.
 * @param[in] p_data      Pointer to the sensor data instance.
 * @param[in] err_code    Result state of data. (NRF_SUCCESS on success,
 *                        otherwise a relevant error code). 
 */
typedef void(*sems_sensor_event_handler_t)(sems_sensor_t const* p_source, void* p_event_data, ret_code_t err_code);


/**
 * @brief SEMS sensor initializing handler type. 
 *
 * @param[in] p_source    Pointer to the instance to be initialized.
 */
typedef ret_code_t (*sems_sensor_init_t)(sems_sensor_t const *p_sensor);

/**
 * @brief SEMS sensor uninitializing handler type. 
 *
 * @param[in] p_source    Pointer to the instance to be uninitialized.
 */
typedef ret_code_t (*sems_sensor_uninit_t)(sems_sensor_t const  *p_sensor);

/**
 * @brief SEMS sensor get data handler type. 
 *
 * @param[in] p_source    Pointer to the instance to be got data.
 */
typedef ret_code_t (*sems_sensor_get_data_t)(sems_sensor_t const *p_sensor);

/**
 * @brief SEMS sensor set sensor event handler type. 
 *
 * @param[in] p_source          Pointer to the instance to be set event.
 * @param[in] enable            enable or disable event.
 * @param[in] p_event_config    Pointer to the sensor event config instance.
 */
typedef ret_code_t (*sems_sensor_set_event_t)(sems_sensor_t const  *p_sensor, bool enable, void * p_event_config);


/**
 * @brief sems sensor descriptor struce
 */
struct sems_sensor_s
{
    sems_sensor_init_t              sensor_init;            ///< Pointer to initialize sensor hanlder function. Never change this variable, after setup.
    sems_sensor_uninit_t            sensor_uninit;          ///< Pointer to uninitialize sensor hanlder function. Never change this variable, after setup.
    sems_sensor_get_data_t          get_sensor_data;        ///< Pointer to get sensor data hanlder function. Never change this variable, after setup.
    sems_sensor_set_event_t         set_sensor_event;       ///< Pointer to set sensor event hanlder function. Never change this variable, after setup.
    
    app_timer_id_t                  app_timer_id;           ///< Sensor app timer instance. Never set a value directly.
    
    sems_sensor_data_handler_t      polling_data_handler;   ///< Point to polling callback function. Never set a value directly.
    
    sems_sensor_event_handler_t     event_data_handler;     ///< Point to event callback function. Never set a value directly.
    
    uint16_t                        sensor_tag;             ///< Tag of SEMS sensor. Never change this variable, after setup.
    semf_sensor_state_t             sensor_state;           ///< State of SEMS sensor. Never set a value directly.
    
    void const *                    p_sensor_config;        ///< Pointer to config instance of sensor. Never change this variable, after setup.
    void *                          p_sensor_data;          ///< Pointer to sensor data instance.
    size_t                          data_size;              ///< Size of sensor data.
};

/**
 * @brief SEMS sensor initialize
 *
 * This macro will create a app time as a static varible.
 * 
 * @param[in] sensor_id     SEMS sensor instance, will be initialized.
 * @parem[in] tag           Sensor tag.
 * @param[in] p_config      Pointer to the sensor config instance; Value can't be NULL.
 * @param[in] p_data        Pointer to the sensor data instance; Value can't be NULL.
 * @param[in] size          Size of sensor data.
 * @param[in] get_data      Pointer to get sensor data hanlder function. Value can't be NULL.
 * @param[in] init          Pointer to initialize sensor hanlder functionn. Value can't be NULL. 
                            If initial is not necessary, just create a function and return NRF_SUCCESS.
 * @param[in] uninit        Pointer to uninitialize sensor hanlder function. Value can't be NULL.
                            If uninitial is not necessary, just create a function and return NRF_SUCCESS.
 * @param[in] set_event     Pointer to set sensor event hanlder function.If it is not necessary, just set value is NULL.
 *
 * @note This macro will create a app time as a static varible.And do not change the timer NEVER !!
 */
#define SEMS_SENSOR_INIT(sensor_id, tag, p_config, p_data, size, get_data, init,  uninit, set_event)  \
    do {                                                \
        APP_TIMER_DEF(sensor_id##_timer_id);            \
        sensor_id.sensor_init = init;                   \
        sensor_id.sensor_uninit = uninit;               \
        sensor_id.get_sensor_data = get_data;           \
        sensor_id.set_sensor_event = set_event;         \
        sensor_id.app_timer_id = sensor_id##_timer_id;  \
        sensor_id.polling_data_handler = NULL;          \
        sensor_id.event_data_handler = NULL;            \
        sensor_id.sensor_tag = tag;                     \
        sensor_id.sensor_state = SEMS_SENSOR_UNINITED;  \
        sensor_id.p_sensor_config = p_config;           \
        sensor_id.p_sensor_data = p_data;               \
        sensor_id.data_size = size;                     \
    } while(0)


/**
 * @brief Function for manual flush SEMS sensor data.
 * 
 * Manual get sensor data once
 *
 * @param[in] p_source          Pointer to the sensor instance to be flushed.
 *
 *
 * @retval     NRF_SUCCESS               If the sensor was successfully flushed.
 * @retval     NRF_ERROR_INVALID_PARAM   If a parameter was invalid.
 * @retval     NRF_ERROR_INVALID_STATE   If the sensor has not been initialized.
 * @retval     NRF_ERROR_INVALID_DATA    If the sensor was invalid.
 *
 * @note    Get get_sensor_data of the sensor can't be NULL; 
 * @note    Sensor must be initialized.
 */
ret_code_t sems_sensor_flush_data(sems_sensor_t *p_sensor);

/**
 * @brief Function for set sensor begin poll
 *
 * Sensor data will flushing automatically in each cycle.
 *
 *
 * @param[in] p_source              Pointer to the sensor instance.
 * @param[in] polling_data_handler  Sensor data callback function. It will be invoked 
 *                                  in each cycle.
 * @param[in] cycle_ms               Number of ms to flush cycle. If value is 0 then poll will be stoped.
 *
 *
 * @retval     NRF_SUCCESS               If the sensor was successfully flushed.
 * @retval     NRF_ERROR_INVALID_PARAM   If a parameter was invalid.
 * @retval     NRF_ERROR_INVALID_STATE   If the sensor has not been initialized.
 * @retval     NRF_ERROR_INVALID_DATA    If the sensor was invalid.
 *
 * @note    If cycle_ms is 0, then poll will be stoped;
 * @note    get_sensor_data of the sensor can't be NULL.
 * @note    Sensor must be initialized.
 * @note    If app timer setup and start failed then this function will return a error_code.
 */
ret_code_t sems_sensor_polling(sems_sensor_t *p_sensor, sems_sensor_data_handler_t polling_data_handler, uint32_t cycle_ms);

/**
 * @brief Function for initialize sensor
 *
 * sensor_init of the sensor will be invoked when this function execute.
 * 
 * @param[in] p_sensor      Pointer to the sensor instance.
 *
 * @retval NRF_SUCCESS                  If the sensor was successfully initialized.
 * @retval NRF_ERROR_INVALID_PARAM      If p_sensor is NULL.
 * @retval NRF_ERROR_INVALID_STATE      If the sensor has been initialized.
 * @retval NRF_ERROR_INVALID_DATA       If the sensor was invalid.
 *
 * @note sensor_init of the sensor can't be NULL;
 */
ret_code_t sems_sensor_init(sems_sensor_t *p_sensor);

/**
 * @brief Function for uninitializ sensor.
 *
 * sensor_uninit of the sensor will be invoked when this function execute.
 *
 * @param[in] p_sensor      Pointer to the sensor instance.
*
 * @retval NRF_SUCCESS                  If the sensor was successfully uninitialized.
 * @retval NRF_ERROR_INVALID_PARAM      If p_sensor is NULL.
 * @retval NRF_ERROR_INVALID_STATE      If the sensor satat is SEMS_SENSOR_POLLING or SEMS_SENSOR_UNINITED.
 * @retval NRF_ERROR_INVALID_DATA       If the sensor was invalid.
 *
 * @note sensor_uninit of the sensor can't be NULL.
 */
ret_code_t sems_sensor_uninit(sems_sensor_t *p_sensor);

/**
 * @brief Function for enable or disable sensor event.
 *
 * set_sensor_event of the sensor will be invoked when this function execute.
 *
 * @param[in] p_sensor              Pointer to the sensor instance.
 * @param[in] p_event_handler       Pointer to the sensor event callback function.
                                    If value is NULL then disable event.
 * @param[in] p_event_config        Pointer to the sensor config instance.
 *
 * @retval NRF_SUCCESS                  If the sensor was successfully enable or disable.
 * @retval NRF_ERROR_INVALID_PARAM      If p_sensor is NULL.
 * @retval NRF_ERROR_NOT_SUPPORTED      set_sensor_event of the sensor is NULL.
 * @retval NRF_ERROR_INVALID_STATE      If the sensor has not been initialized.
 */
ret_code_t sems_sensor_set_event_handler(sems_sensor_t *p_sensor, sems_sensor_event_handler_t p_event_handler, void * p_event_config);


#endif


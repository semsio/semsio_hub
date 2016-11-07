/** @file
 * @brief Common defines and macros for GPIO senosr.
 *
 * Support get pin value and gpio event.
 */

#include "nrf_drv_gpiote.h"
#include "sems_sensor.h"
#include "app_error.h"

/**
 * @brief Holde GPIO config data.
 */
typedef struct
{
    nrf_drv_gpiote_pin_t            pin;
    nrf_gpio_pin_pull_t             pull; 
    
} sems_gpio_sensor_config_t;

/**
 * @brief Holde GPIO result data.
 */
#define GPIO_CONFIG(PIN,PULL)                                               \
    {                                                                       \
        .pin = PIN,                                                         \
        .pull = PULL,                                                       \
    }

typedef uint8_t sems_gpio_pin_val_t; 

/**
 * @brief Holde GPIO event data.
 */
typedef struct 
{
    sems_gpio_pin_val_t     pin_val;
    nrf_gpiote_polarity_t   action;
} sems_gpio_event_data_t;

/**
 * @brief Function for get a GPIO SEMS sensor.
 *
 * @retval  Point to SEMS sensor instance.
 */         
sems_sensor_t* create_gpio_sensor(nrf_drv_gpiote_pin_t pin, nrf_gpio_pin_pull_t pull);
/** @file
 * @brief Common defines and macros for TSL2561 digital luminosity senosr.
 */

#include "sems_sensor.h"
#include "app_twi.h"
#include "sems_module.h"

#pragma once

#define SEMS_LUMINOSITY_TAG          SEMS_TWI_TAG + 0x0030

///< TSL2561 register
#define TSL2561_CMD           0x80
#define TSL2561_CMD_CLEAR     0xC0
#define	TSL2561_REG_CONTROL   0x00
#define	TSL2561_REG_TIMING    0x01
#define	TSL2561_REG_THRESH_L  0x02
#define	TSL2561_REG_THRESH_H  0x04
#define	TSL2561_REG_INTCTL    0x06
#define	TSL2561_REG_ID        0x0A
#define	TSL2561_REG_DATA_0    0x0C
#define	TSL2561_REG_DATA_1    0x0E

///< TWI salve address of TSL2561
#define TSL2561_ADD_0		0x39
#define TSL2561_ADD_1		0x49
#define TSL2561_ADD_2		0x29

#define TSL2561_BUFFER_SIZE 0x02

/**
 * @brief integration time.
 */
typedef enum 
{
    TSL2561_TIME0 = 0x0,        /*< Scale:0.034  Nominal integration time:13.7ms */
    TSL2561_TIME1 = 0x1,        /*< Scale:0.252  Nominal integration time:101ms */
    TSL2561_TIME2 = 0x2,        /*< Scale:1  Nominal integration time:402ms */
    TSL2561_TIME_NAN = 0x3
} integrate_time_t;

/**
 * @brief Holde TSL2561 config data.
 * Never declare a variable of this type, but use the macro @ref TSL2561_CONFIG_0,  @ref TSL2561_CONFIG_1 and  @ref TSL2561_CONFIG_2  instead.
 */
typedef struct
{
    app_twi_t *                     p_app_twi;      ///< Pointer to app_twi instance.
    uint8_t                         slave_addr;     ///< TSL2561 slve address.
} sems_tsl2561_sensor_config_t;


/**
 * @brief holde TSL2561 result data.
 */
typedef float sems_tsl2561_lux_t;

/**
 * @brief setup a TSL2561 config with TWI salve address @ref TSL2561_ADD_0
 */
#define TSL2561_CONFIG_DEF_SETUP(p_config)         \
    do                                             \
    {                                              \
        p_config->p_app_twi = P_SEMS_TWI_INSTANCE; \
        p_config->slave_addr = TSL2561_ADD_0;      \
    } while (0)

/**
 * @brief setup a TSL2561 config with TWI salve address @ref TSL2561_ADD_1
 */
#define TSL2561_CONFIG_ADD49_SETUP(p_config)       \
    do                                             \
    {                                              \
        p_config->p_app_twi = P_SEMS_TWI_INSTANCE; \
        p_config->slave_addr = TSL2561_ADD_1;      \
    } while (0)

/**
 * @brief setup a TSL2561 config with TWI salve address @ref TSL2561_ADD_2
 */
#define TSL2561_CONFIG_ADD29_SETUP(p_config)       \
    do                                             \
    {                                              \
        p_config->p_app_twi = P_SEMS_TWI_INSTANCE; \
        p_config->slave_addr = TSL2561_ADD_2;      \
    } while (0)

/**
 * @brief Function for get a TSL2561 SEMS sensor with slave address is @ref TSL2561_ADD_0
 *
 * @retval  Point to SEMS sensor instance.
 */
sems_sensor_t* get_sems_tsl2561_default_sensor();

/**
 * @brief Function for get a TSL2561 SEMS sensor with slave address is @ref TSL2561_ADD_2
 *
 * @retval  Point to SEMS sensor instance.
 */
sems_sensor_t* get_sems_tsl2561_addr29_sensor();

/**
 * @brief Function for get a TSL2561 SEMS sensor with slave address is @ref TSL2561_ADD_1
 *
 * @retval  Point to SEMS sensor instance.
 */
sems_sensor_t* get_sems_tsl2561_addr49_sensor();

/** @file
 * @brief Common defines and macros for HTU21D senosr.
 */

#include "app_error.h"
#include "app_twi.h"
#include "sems_sensor.h"

///< HTU21D  TWI salve address
#define HTU21D_ADDRESS 0x40

///< HTU21D register
#define HUMIDITY_NO_HOLD_MASTER_ADDRESS 0xF5
#define TEMPERATURE_NO_HOLD_MASTER_ADDRESS 0xF3

/**
 * @brief holde HTU21D result data.
 */
typedef struct
{
    float       humidity;
    float       tempeature;
} sems_htu21d_data_t;

/**
 * @brief Holde HTU21D config data.
 * Never declare a variable of this type, but use the macro @ref HTU21D_CONFIG instead.
 */
typedef struct
{
    app_twi_t  *                    p_app_twi;
    uint8_t                         slave_addr;
} sems_htu21d_sensor_config_t;

/**
 * @brief setup a HTU21D config.
 */
#define HTU21D_CONFIG                                                       \
    {                                                                       \
        .p_app_twi = P_SEMS_TWI_INSTANCE,                                   \
        .slave_addr = HTU21D_ADDRESS,                                       \
    }

/**
 * @brief setup a empty HTU21D  result data.
 */
#define HTU21D_DATA_EMPTY                                                   \
    {                                                                       \
        .humidity = 0,                                                      \
        .humidity = 0,                                                      \
    }
    
/**
 * @brief Function for get a HTU21D SEMS sensor.
 *
 * @retval  Point to SEMS sensor instance.
 */ 
sems_sensor_t* get_sems_htu21d_sensor();

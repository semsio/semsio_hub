/** @file
 * @brief Common defines and macros for SEMS MODEL.
 */
#ifndef __SEMS_MODULE_H
#define __SEMS_MODULE_H

#include "app_twi.h"

#define SEMS_MODULE_TWI_SCL         26
#define SEMS_MODULE_TWI_SDA         27

extern  app_twi_t sems_twi_instance;                ///< APP_TWI_INSTANCE(0) for TWI sensor

#define P_SEMS_TWI_INSTANCE &sems_twi_instance      ///< Address of sems_twi_instance;

/**
 * @brief Function for initializing SEMS module control
 * 
 * This function initialize APP_TWI_INSTANCE(0) and GPIOTE.
 *
 * @note If TWI init failed or GPIOTE init failed then this function will return a error code.
 */
ret_code_t sems_module_init(void);



#endif
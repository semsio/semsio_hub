/** @file
 * @brief Common defines and macros for SEMS IR action.
 */

#ifndef __SEMS_IR_ACTION_H
#define __SEMS_IR_ACTION_H

#include <stdint.h>
#include <stdbool.h>
#include "app_util_platform.h"
#include "nrf_drv_gpiote.h"
#include "sems_actuator.h"
#include "app_error.h"

#define SEMS_IR_TAG                0x5000

#define SEMS_IR_TIMER_CARRIER                NRF_TIMER1                     ///< Generate 38Khz carrier.

#define SEMS_IR_CARRIER_COUNTER              NRF_TIMER2                     ///< Count single cycle of carrier.
#define SEMS_IR_CARRIER_COUNTER_IRQn         TIMER2_IRQn
#define SEMS_IR_CARRIER_COUNTER_IRQHandler   TIMER2_IRQHandler
#define SEMS_IR_CARRIER_COUNTER_IRQ_Priority APP_IRQ_PRIORITY_HIGH

#define SEMS_IR_PPI_CH_A         0
#define SEMS_IR_PPI_CH_B         1
#define SEMS_IR_PPI_CH_C         2
#define SEMS_IR_PPI_CH_D         3
#define SEMS_IR_PPI_CH_E         4

#define SEMS_IR_PPI_GROUP        0

#define SEMS_IR_CARRIER_LOW_US   18         ///< Duty Cycle:0.3
#define SEMS_IR_CARRIER_HIGH_US  8

#define SEMS_IR_BUFFER_LEN       128             ///< Max length of raw data buffer.

/**
 * @brief IR actuator config type.
 */
typedef nrf_drv_gpiote_pin_t sems_ir_config_t;

/**
 * @brief Convert send data to raw data function prototype. 
 *
 * @param[in] p_send_data       Pointer to the send data instance.
 * @param[out] p_raw_buffer     Pointer to the raw data buffer.unit is us.
 * @param[out] p_raw_lenght     Length of row data.
 *
 * @note p_raw_lenght must less than SEMS_IR_BUFFER_LEN.
 */
typedef ret_code_t (*sems_ir_encode_t)(void* p_send_data, uint32_t p_raw_buffer[], uint8_t* p_raw_lenght);


/**
 * @brief Struct for holde IR operate data.
 */
typedef struct 
{
    void*                   p_data;             /*< IR operate data */
    sems_ir_encode_t        encode_handler;     /*< IR operate data encode function */
    
} sems_ir_operate_data_t;

/**
 * @brief Function for get a SEMS IR actuator.
 *
 * @retval  Point to SEMS IR actuator instance.
 */  
sems_actuator_t* get_sems_ir_actuator(nrf_drv_gpiote_pin_t ir_pin);

    
#endif

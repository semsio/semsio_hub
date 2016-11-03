/** @file
 * @brief Common defines and macros for SEMS IR action.
 */

#ifndef __SEMS_IR_ACTION_H
#define __SEMS_IR_ACTION_H

#include <stdint.h>
#include <stdbool.h>
#include "app_util_platform.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"

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

#define SEMS_IR_BUFFER_LEN  128             ///< Max length of raw data buffer.


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
 * @brief Function for initialize IR action.
 * This function will setup timer and PPI.
 */
ret_code_t sems_ir_init(nrf_drv_gpiote_pin_t ir_pin);

/**
 * @brief Function for send IR data.
 *
 * @param[in] p_data        Pointer to data instance, which will be sent.
 * @param[in] encode_func   Pointer to the data encode function.
 * 
 */
ret_code_t sems_ir_send(void* p_data, sems_ir_encode_t encode_func);

    
#endif

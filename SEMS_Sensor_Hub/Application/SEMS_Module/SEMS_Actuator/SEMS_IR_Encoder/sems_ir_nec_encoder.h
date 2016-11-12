/** @file
 * @brief Common defines for IR NEC protocal data.
 */
#include "sems_ir_actuator.h"

#pragma once

/**
 * @brief Holde NEC data, which will be sent.
 */
typedef struct 
{
    uint8_t address;        /*< Address code. */
    uint8_t command;        /*< Command code. */
    uint8_t repeat;         /*< Repeat count. */
} sems_nec_data;


/**
 * @brife Function for convert @ref sems_nec_data to IR raw data.
 *
 * @param[in] p_send_data       Pointer to the NEC data instance.
 * @param[out] p_raw_buffer     Pointer to the raw data buffer.
 * @param[out] p_raw_lenght     Length of row data.
 */
ret_code_t sems_ir_nec_encode(void* p_send_data, uint32_t p_raw_buffer[], uint8_t* p_raw_lenght);
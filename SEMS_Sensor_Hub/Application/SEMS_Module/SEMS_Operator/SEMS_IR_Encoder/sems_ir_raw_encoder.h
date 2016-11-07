/** @file
 * @brief Common defines for IR raw data.
 */

#include "sems_ir_operator.h"

#pragma once

/**
 * @brief Holde raw data, which will be sent.
 */
typedef struct 
{
    uint16_t    length;         /*< length of send data. */
    uint16_t*   p_raw_data;     /*< send data. */
} sems_raw_data;

/**
 * @brife This function just copy raw data to IR raw data buffer.
 *
 * @param[in] p_send_data       Pointer to the send data instance.
 * @param[out] p_raw_buffer     Pointer to the raw data buffer.
 * @param[out] p_raw_lenght     Length of row data.
 */
ret_code_t sems_ir_raw_encode(void* p_send_data, uint32_t p_raw_buffer[], uint8_t* p_raw_lenght);
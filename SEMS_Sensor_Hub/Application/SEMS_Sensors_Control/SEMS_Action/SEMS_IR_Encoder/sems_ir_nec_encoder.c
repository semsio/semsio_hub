#include "sems_ir_action.h"
#include "sems_ir_nec_encoder.h"



ret_code_t sems_ir_nec_encode(void* p_data, uint32_t p_raw_buffer[], uint8_t* p_raw_lenght)
{
    if (p_data == NULL)
        return NRF_ERROR_INVALID_PARAM;
    
    sems_nec_data *p_send_data = (sems_nec_data *)p_data;
    
    int8_t index =0;
    int32_t total_us = 0;
    
    p_raw_buffer[0] = 9000;
    p_raw_buffer[1] = 4500;
    index = 2;
    total_us = 13500;
    for (int bit = 0; bit < 8; bit++)
    {
        
        if(((p_send_data->address) >> bit) & 0x01)
        {
            // 560us on, 2.25ms time until next symbol
            p_raw_buffer[index++] = 560;
            p_raw_buffer[index++] = 1690;//2250 - 560;
            total_us += 2250;
        }
        else
        {
            // 560us on, 1.12ms time until next symbol
            p_raw_buffer[index++] = 560;
            p_raw_buffer[index++] = 560;//1120 - 560;
            total_us += 1120;
        }
    }
    
    for (int bit = 0; bit < 8; bit++)
    {
        
        if((((p_send_data->address) >> bit) & 0x01) == 0)
        {
            // 560us on, 2.25ms time until next symbol
            p_raw_buffer[index++] = 560;
            p_raw_buffer[index++] = 1690;//2250 - 560;
            total_us += 2250;
        }
        else
        {
            // 560us on, 1.12ms time until next symbol
            p_raw_buffer[index++] = 560;
            p_raw_buffer[index++] = 560;//1120 - 560;
            total_us += 1120;
        }
    }
    
    for (int bit = 0; bit < 8; bit++)
    {
        
        if(((p_send_data->command) >> bit) & 0x01)
        {
            // 560us on, 2.25ms time until next symbol
            p_raw_buffer[index++] = 560;
            p_raw_buffer[index++] = 1690;//2250 - 560;
            total_us += 2250;
        }
        else
        {
            // 560us on, 1.12ms time until next symbol
            p_raw_buffer[index++] = 560;
            p_raw_buffer[index++] = 560;//1120 - 560;
            total_us += 1120;
        }
    }
    
    for (int bit = 0; bit < 8; bit++)
    {
        
        if((((p_send_data->command) >> bit) & 0x01) == 0)
        {
            // 560us on, 2.25ms time until next symbol
            p_raw_buffer[index++] = 560;
            p_raw_buffer[index++] = 1690;//2250 - 560;
            total_us += 2250;
        }
        else
        {
            // 560us on, 1.12ms time until next symbol
            p_raw_buffer[index++] = 560;
            p_raw_buffer[index++] = 560;//1120 - 560;
            total_us += 1120;
        }
    }
    p_raw_buffer[index++] = 560;
    total_us += 560;
    
    p_raw_buffer[index++] = 110000 - total_us;
    if (p_send_data ->repeat > 0)
    {
        for (int i =0 ; i <p_send_data ->repeat ; i++ )
        {
            if (index < (SEMS_IR_BUFFER_LEN - 4))
            {
                p_raw_buffer[index++]  = 9000;
                p_raw_buffer[index++]  = 2250;
                p_raw_buffer[index++]  = 560;
                p_raw_buffer[index++]  = 98190;
            } else 
            {
                return NRF_ERROR_INVALID_PARAM;
            }
        }
    }
    *p_raw_lenght = index;
    
    return NRF_SUCCESS;
    
}
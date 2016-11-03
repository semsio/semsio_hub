#include "sems_ir_action.h"
#include "sems_ir_raw_encoder.h"



ret_code_t sems_ir_raw_encode(void* p_data, uint32_t p_raw_buffer[], uint8_t* p_raw_lenght)
{
    if (p_data == NULL)
        return NRF_ERROR_INVALID_PARAM;
    
    sems_raw_data *p_send_data = (sems_raw_data *)p_data;
    
    if (p_send_data->length > SEMS_IR_BUFFER_LEN)
        return NRF_ERROR_INVALID_PARAM;
    
    *p_raw_lenght = p_send_data->length;
    
    for (int i =0 ; i< p_send_data->length; i++)
    {
        p_raw_buffer[i] = p_send_data->p_raw_data[i];
    }
    return NRF_SUCCESS;
}
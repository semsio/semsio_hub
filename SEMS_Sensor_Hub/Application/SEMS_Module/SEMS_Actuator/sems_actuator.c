#include "sems_actuator.h"

ret_code_t sems_actuator_init(sems_actuator_t *p_actuator)
{
    if (p_actuator ==NULL)
        return NRF_ERROR_INVALID_PARAM;
    if (p_actuator->actuator_init == NULL)
    {
        return NRF_ERROR_INVALID_DATA;
    } else
    {
        if (p_actuator->actuator_state != SEMS_ACTUATOR_UNINITED)
            return NRF_ERROR_INVALID_STATE;

        ret_code_t err_code = p_actuator->actuator_init(p_actuator);
        if (err_code == NRF_SUCCESS)
        {
            p_actuator->actuator_state = SEMS_ACTUATOR_INITED;
        }
        
        return err_code;
    }
}

ret_code_t sems_actuator_uninit(sems_actuator_t *p_actuator)
{
    if (p_actuator ==NULL)
        return NRF_ERROR_INVALID_PARAM;
    if (p_actuator->actuator_uninit == NULL)
    {
        return NRF_ERROR_INVALID_DATA;
    } else
    {
        if (p_actuator->actuator_state != SEMS_ACTUATOR_INITED)  
            return NRF_ERROR_INVALID_STATE;
        
        ret_code_t err_code  = p_actuator->actuator_uninit(p_actuator);
        if (err_code == NRF_SUCCESS)
        {
            p_actuator->actuator_state = SEMS_ACTUATOR_UNINITED;
        }
        
        return err_code;
    }
}

ret_code_t sems_actuator_execute(sems_actuator_t *p_actuator, void* p_data)
{
    if (p_actuator == NULL)
    return NRF_ERROR_INVALID_PARAM;
    
    if (p_actuator->actuator_state != SEMS_ACTUATOR_INITED)
        return NRF_ERROR_INVALID_STATE;
    if (p_actuator->execute_operation == NULL)
        return NRF_ERROR_INVALID_DATA;
    ret_code_t err_code;
    err_code = p_actuator->execute_operation(p_actuator, p_data);
    return err_code;
}
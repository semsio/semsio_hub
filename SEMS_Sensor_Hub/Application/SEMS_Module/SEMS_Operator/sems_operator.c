#include "sems_operator.h"

ret_code_t sems_operator_init(sems_operator_t *p_operator)
{
    if (p_operator ==NULL)
        return NRF_ERROR_INVALID_PARAM;
    if (p_operator->operator_init == NULL)
    {
        return NRF_ERROR_INVALID_DATA;
    } else
    {
        if (p_operator->operator_state != SEMS_OPERATOR_UNINITED)
            return NRF_ERROR_INVALID_STATE;

        ret_code_t err_code = p_operator->operator_init(p_operator);
        if (err_code == NRF_SUCCESS)
        {
            p_operator->operator_state = SEMS_OPERATOR_INITED;
        }
        
        return err_code;
    }
}

ret_code_t sems_operator_uninit(sems_operator_t *p_operator)
{
    if (p_operator ==NULL)
        return NRF_ERROR_INVALID_PARAM;
    if (p_operator->operator_uninit == NULL)
    {
        return NRF_ERROR_INVALID_DATA;
    } else
    {
        if (p_operator->operator_state != SEMS_OPERATOR_INITED)  
            return NRF_ERROR_INVALID_STATE;
        
        ret_code_t err_code  = p_operator->operator_uninit(p_operator);
        if (err_code == NRF_SUCCESS)
        {
            p_operator->operator_state = SEMS_OPERATOR_UNINITED;
        }
        
        return err_code;
    }
}

ret_code_t sems_operator_execute(sems_operator_t *p_operator, void* p_data)
{
    if (p_operator == NULL)
    return NRF_ERROR_INVALID_PARAM;
    
    if (p_operator->operator_state != SEMS_OPERATOR_INITED)
        return NRF_ERROR_INVALID_STATE;
    if (p_operator->execute_operation == NULL)
        return NRF_ERROR_INVALID_DATA;
    ret_code_t err_code;
    err_code = p_operator->execute_operation(p_operator, p_data);
    return err_code;
}
/** @file
 * @brief Template operator for help develpor build custom operator.
 */

#include "sems_TEMPLATE_operator.h"

 /**
 * @brief Function for initialize operator
 *
 * @param[in] p_operator Pointer to operator instance.
 */
static ret_code_t sems_TEMPLATE_init(sems_operator_t const *p_operator)
{
    //TODO: Initialize code if nessesary for example TWI, GPIO. If it's not just return NRF_SUCCESS;
    return NRF_SUCCESS;
}
 
 /**
 * @brief Function for uninitialize operator
 *
 * @param[in] p_operator Pointer to operator instance.
 */
static ret_code_t sems_TEMPLATE_uninit(sems_operator_t const *p_operator)
{
    //TODO: Uninitialize and free memory code if nessesary. If it's not just return NRF_SUCCESS;
    return NRF_SUCCESS;
}
    
/**
 * @brief Function for execute operatoion.
 *
 * execute_operation of the operator will be invoked when this function execute.
 *
 * @param[in] p_operator      Pointer to the operator instance.
 * @param[in] p_data          Pointer to the execute data instance.
 *
 */
static ret_code_t sems_TEMPLATE_execute(sems_operator_t const *p_operator, void *p_data)
{
    sems_TEMPLATE_data_t *p_template_data = p_data;
    //TODO: Execute operation
    
    return NRF_SUCCESS;
}



sems_operator_t* get_sems_TEMPLATE_operator()
{
    //TODO:Declare (malloc) sems_operator_t£¬sems_TEMPLATE_operator_config_t
    static sems_TEMPLATE_operator_config_t config;
    static sems_operator_t sems_operator;
    
    //TODO:setup operator config
    config.config1 = 0;
    config.config2 = 1;
    
    //TODO: setup operator;
    SEMS_OPERATOR_INIT(sems_operator,SEMS_TEMPLATE_OPERATOR_TAG, &config, sems_TEMPLATE_init, sems_TEMPLATE_uninit, sems_TEMPLATE_execute);
    return &sems_operator;
}

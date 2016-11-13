/** @file
 * @brief Template actuator for help develpor build custom actuator.
 */

#include "sems_TEMPLATE_actuator.h"

 /**
 * @brief Function for initialize actuator
 *
 * @param[in] p_actuator Pointer to actuator instance.
 */
static ret_code_t sems_TEMPLATE_init(sems_actuator_t const *p_actuator)
{
    //TODO: Initialize code if nessesary for example TWI, GPIO. If it's not just return NRF_SUCCESS;
    return NRF_SUCCESS;
}
 
 /**
 * @brief Function for uninitialize actuator
 *
 * @param[in] p_actuator Pointer to actuator instance.
 */
static ret_code_t sems_TEMPLATE_uninit(sems_actuator_t const *p_actuator)
{
    //TODO: Uninitialize and free memory code if nessesary. If it's not just return NRF_SUCCESS;
    return NRF_SUCCESS;
}
    
/**
 * @brief Function for execute operatoion.
 *
 * execute_operation of the actuator will be invoked when this function execute.
 *
 * @param[in] p_actuator      Pointer to the actuator instance.
 * @param[in] p_data          Pointer to the execute data instance.
 *
 */
static ret_code_t sems_TEMPLATE_execute(sems_actuator_t const *p_actuator, void *p_data)
{
    sems_TEMPLATE_data_t *p_template_data = p_data;
    //TODO: Execute operation
    
    return NRF_SUCCESS;
}



sems_actuator_t* get_sems_TEMPLATE_actuator()
{
    //TODO:Declare (malloc) sems_actuator_t£¬sems_TEMPLATE_actuator_config_t
    static sems_TEMPLATE_actuator_config_t config;
    static sems_actuator_t sems_actuator;
    
    //TODO:setup actuator config
    config.config1 = 0;
    config.config2 = 1;
    
    //TODO: setup actuator;
    SEMS_ACTUATOR_INIT(sems_actuator,SEMS_TEMPLATE_ACTUATOR_TAG, &config, sems_TEMPLATE_init, sems_TEMPLATE_uninit, sems_TEMPLATE_execute);
    return &sems_actuator;
}

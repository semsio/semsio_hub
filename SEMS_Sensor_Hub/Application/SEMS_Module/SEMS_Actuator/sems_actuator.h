/** @file
 * @brief Common defines and macros for SEMS actuator.
 */
#ifndef __SEMS_ACTUATOR_H
#define __SEMS_ACTUATOR_H

#include "app_error.h"
#include "app_twi.h"
#include "app_timer.h"
#include "app_error.h"
#include "ble_services.h"
/**
 * @brief actuator state.
 */
typedef enum 
{
    SEMS_ACTUATOR_UNINITED = 0x0,   /**<Actuator is uninited. */
    SEMS_ACTUATOR_INITED = 0x1,     /*<Actuator is inited. */
    SEMS_ACTUATOR_BUSY = 0x2        /*<Actuator is busy */
}semf_actuator_state_t;


/**
 * @brief sems actuator descriptor.
 */
typedef struct sems_actuator_s sems_actuator_t;

/**
 * @brief SEMS actuator initializing handler type. 
 *
 * @param[in] p_source    Pointer to the instance to be initialized.
 */
typedef ret_code_t (*sems_actuator_init_t)(sems_actuator_t const *p_actuator);

/**
 * @brief SEMS actuator uninitializing handler type. 
 *
 * @param[in] p_source    Pointer to the instance to be uninitialized.
 */
typedef ret_code_t (*sems_actuator_uninit_t)(sems_actuator_t const  *p_actuator);

/**
 * @brief SEMS actuator execute operation handler type. 
 *
 * @param[in] p_source    Pointer to the instance to be got data.
 * @param[in] p_data      Pointer to the instance data for execute operation
 */
typedef ret_code_t (*sems_actuator_execute_t)(sems_actuator_t const *p_actuator, void *p_data);

/**
 * @brief SEMS operation descriptor sture
 */
struct sems_actuator_s
{
    sems_actuator_init_t            actuator_init;            ///< Pointer to initialize actuator hanlder function. Never change this variable, after setup.
    sems_actuator_uninit_t          actuator_uninit;          ///< Pointer to uninitialize actuator hanlder function. Never change this variable, after setup.
    sems_actuator_execute_t         execute_operation;        ///< Pointer to execute operation hanlder function. Never change this variable, after setup.
    
    uint16_t                        actuator_tag;             ///< Tag of SEMS actuator. Never change this variable, after setup.
    semf_actuator_state_t           actuator_state;           ///< State of SEMS actuator. Never set a value directly.
    
    void const *                    p_actuator_config;          ///< Pointer to config instance of actuator. Never change this variable, after setup.
};


/**
 * @brief SEMS actuator initialize
 *
 * 
 * @param[in] actuator_id       SEMS actuator instance, will be initialized.
 * @parem[in] tag               Operator tag.
 * @param[in] p_config          Pointer to the actuator config instance; Value can't be NULL.
 * @param[in] init              Pointer to initialize actuator hanlder functionn. Value can't be NULL. 
                                If initial is not necessary, just create a function and return NRF_SUCCESS.
 * @param[in] uninit            Pointer to uninitialize actuator hanlder function. Value can't be NULL.
                                If uninitial is not necessary, just create a function and return NRF_SUCCESS.
 * @param[in] execute           Pointer to execute operation hanlder function.Value can't be NULL.
 *
 * @note This macro will create a app time as a static varible.And do not change the timer NEVER !!
 */
#define SEMS_ACTUATOR_INIT(actuator_id, tag, p_config, init, uninit, execute)  \
    do {                                                        \
        actuator_id.actuator_init = init;                       \
        actuator_id.actuator_uninit = uninit;                   \
        actuator_id.execute_operation = execute;                \
        actuator_id.actuator_tag = tag;                         \
        actuator_id.actuator_state = SEMS_ACTUATOR_UNINITED;    \
        actuator_id.p_actuator_config = p_config;               \
    } while(0);
        
        
/**
 * @brief Function for initialize actuator
 *
 * actuator_init of the actuator will be invoked when this function execute.
 * 
 * @param[in] p_actuator      Pointer to the actuator instance.
 *
 * @retval NRF_SUCCESS                  If the actuator was successfully initialized.
 * @retval NRF_ERROR_INVALID_PARAM      If p_actuator is NULL.
 * @retval NRF_ERROR_INVALID_STATE      If the actuator has been initialized.
 * @retval NRF_ERROR_INVALID_DATA       If the actuator was invalid.
 *
 * @note actuator_init of the actuator can't be NULL;
 */
ret_code_t sems_actuator_init(sems_actuator_t *p_actuator);

/**
 * @brief Function for uninitializ actuator.
 *
 * actuator_uninit of the actuator will be invoked when this function execute.
 *
 * @param[in] p_actuator      Pointer to the actuator instance.
 *
 * @retval NRF_SUCCESS                  If the actuator was successfully uninitialized.
 * @retval NRF_ERROR_INVALID_PARAM      If p_actuator is NULL.
 * @retval NRF_ERROR_INVALID_STATE      If the actuator satat is SEMS_OPERATOR_UNINITED.
 * @retval NRF_ERROR_INVALID_DATA       If the actuator was invalid.
 *
 * @note actuator_uninit of the actuator can't be NULL.
 */
ret_code_t sems_actuator_uninit(sems_actuator_t *p_actuator);

/**
 * @brief Function for execute operatoion.
 *
 * execute_operation of the actuator will be invoked when this function execute.
 *
 * @param[in] p_actuator      Pointer to the actuator instance.
 * @param[in] p_data          Pointer to the execute data instance.
 *
 * @retval NRF_SUCCESS                  If the operation success.
 * @retval NRF_ERROR_INVALID_PARAM      If p_actuator is NULL.
 * @retval NRF_ERROR_INVALID_STATE      If the actuator satat is SEMS_OPERATOR_UNINITED.
 * @retval NRF_ERROR_INVALID_DATA       If the actuator was invalid.
 *
 * @note execute_operation of the actuator can't be NULL.
 */
ret_code_t sems_actuator_execute(sems_actuator_t *p_actuator, void* p_data);
#endif
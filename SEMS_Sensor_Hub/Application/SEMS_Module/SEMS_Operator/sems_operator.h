/** @file
 * @brief Common defines and macros for SEMS operator.
 */
#ifndef __SEMS_OPERATOR_H
#define __SEMS_OPERATOR_H

#include "app_error.h"
#include "app_twi.h"
#include "app_timer.h"
#include "app_error.h"
#include "ble_services.h"
/**
 * @brief operator state.
 */
typedef enum 
{
    SEMS_OPERATOR_UNINITED = 0x0,   /**<operator is uninited. */
    SEMS_OPERATOR_INITED = 0x1,     /*<operator is inited. */
    SEMS_OPERATOR_BUSY = 0x2        /*<operator is busy */
}semf_operator_state_t;


/**
 * @brief sems operator descriptor.
 */
typedef struct sems_operator_s sems_operator_t;

/**
 * @brief SEMS operator initializing handler type. 
 *
 * @param[in] p_source    Pointer to the instance to be initialized.
 */
typedef ret_code_t (*sems_operator_init_t)(sems_operator_t const *p_operator);

/**
 * @brief SEMS operator uninitializing handler type. 
 *
 * @param[in] p_source    Pointer to the instance to be uninitialized.
 */
typedef ret_code_t (*sems_operator_uninit_t)(sems_operator_t const  *p_operator);

/**
 * @brief SEMS operator execute operation handler type. 
 *
 * @param[in] p_source    Pointer to the instance to be got data.
 * @param[in] p_data      Pointer to the instance data for execute operation
 */
typedef ret_code_t (*sems_operator_execute_t)(sems_operator_t const *p_operator, void *p_data);

/**
 * @brief SEMS operation descriptor sture
 */
struct sems_operator_s
{
    sems_operator_init_t            operator_init;            ///< Pointer to initialize operator hanlder function. Never change this variable, after setup.
    sems_operator_uninit_t          operator_uninit;          ///< Pointer to uninitialize operator hanlder function. Never change this variable, after setup.
    sems_operator_execute_t         execute_operation;        ///< Pointer to execute operation hanlder function. Never change this variable, after setup.
    
    uint16_t                        operator_tag;             ///< Tag of SEMS operator. Never change this variable, after setup.
    semf_operator_state_t           operator_state;           ///< State of SEMS operator. Never set a value directly.
    
    void const *                    p_operator_config;          ///< Pointer to config instance of operator. Never change this variable, after setup.
};


/**
 * @brief SEMS operator initialize
 *
 * 
 * @param[in] operator_id       SEMS operator instance, will be initialized.
 * @parem[in] tag               Operator tag.
 * @param[in] p_config          Pointer to the operator config instance; Value can't be NULL.
 * @param[in] init              Pointer to initialize sensor hanlder functionn. Value can't be NULL. 
                                If initial is not necessary, just create a function and return NRF_SUCCESS.
 * @param[in] uninit            Pointer to uninitialize sensor hanlder function. Value can't be NULL.
                                If uninitial is not necessary, just create a function and return NRF_SUCCESS.
 * @param[in] execute           Pointer to execute operation hanlder function.Value can't be NULL.
 *
 * @note This macro will create a app time as a static varible.And do not change the timer NEVER !!
 */
#define SEMS_OPERATOR_INIT(operator_id, tag, p_config, init, uninit, execute)  \
    do {                                                        \
        operator_id.operator_init = init;                       \
        operator_id.operator_uninit = uninit;                   \
        operator_id.execute_operation = execute;                \
        operator_id.operator_tag = tag;                         \
        operator_id.operator_state = SEMS_OPERATOR_UNINITED;    \
        operator_id.p_operator_config = p_config;               \
    } while(0);
        
        
/**
 * @brief Function for initialize operator
 *
 * operator_init of the operator will be invoked when this function execute.
 * 
 * @param[in] p_operator      Pointer to the operator instance.
 *
 * @retval NRF_SUCCESS                  If the operator was successfully initialized.
 * @retval NRF_ERROR_INVALID_PARAM      If p_operator is NULL.
 * @retval NRF_ERROR_INVALID_STATE      If the operator has been initialized.
 * @retval NRF_ERROR_INVALID_DATA       If the operator was invalid.
 *
 * @note sensor_init of the sensor can't be NULL;
 */
ret_code_t sems_operator_init(sems_operator_t *p_operator);

/**
 * @brief Function for uninitializ operator.
 *
 * operator_uninit of the operator will be invoked when this function execute.
 *
 * @param[in] p_operator      Pointer to the sensor instance.
 *
 * @retval NRF_SUCCESS                  If the operator was successfully uninitialized.
 * @retval NRF_ERROR_INVALID_PARAM      If p_operator is NULL.
 * @retval NRF_ERROR_INVALID_STATE      If the operator satat is SEMS_SENSOR_UNINITED.
 * @retval NRF_ERROR_INVALID_DATA       If the operator was invalid.
 *
 * @note sensor_uninit of the sensor can't be NULL.
 */
ret_code_t sems_operator_uninit(sems_operator_t *p_operator);

/**
 * @brief Function for execute operatoion.
 *
 * execute_operation of the operator will be invoked when this function execute.
 *
 * @param[in] p_operator      Pointer to the sensor instance.
 * @param[in] p_data          Pointer to the execute data instance.
 *
 * @retval NRF_SUCCESS                  If the operation success.
 * @retval NRF_ERROR_INVALID_PARAM      If p_operator is NULL.
 * @retval NRF_ERROR_INVALID_STATE      If the operator satat is SEMS_SENSOR_UNINITED.
 * @retval NRF_ERROR_INVALID_DATA       If the operator was invalid.
 *
 * @note sensor_uninit of the sensor can't be NULL.
 */
ret_code_t sems_operator_execute(sems_operator_t *p_operator, void* p_data);
#endif
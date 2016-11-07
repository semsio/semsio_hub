/** @file
 * @brief Template operator for help develpor build custom operator.
 * For more example you can check sems_ir_operator.
 */
#include "sems_operator.h"

 ///< TODO:set you operator tag
#define SEMS_TEMPLATE_OPERATOR_TAG        0x0000
    
     
/**
 * @brief holde TEMPLATE operator execute data.
 */
typedef struct
{
    //TODO: Set strust.
    uint8_t result1;
    uint8_t result2;
} sems_TEMPLATE_data_t;

/**
 * @brief Holde TEMPLATE operator config data.
 */
typedef struct
{
    //TODO: Set strust.
    uint8_t config1;
    uint8_t config2;
} sems_TEMPLATE_operator_config_t;

/**
 * @brief Function for get a SEMS TEMPLATE operator.
 *
 * @retval  Point to SEMS TEMPLATE operator instance.
 */  
sems_operator_t* get_sems_TEMPLATE_operator();
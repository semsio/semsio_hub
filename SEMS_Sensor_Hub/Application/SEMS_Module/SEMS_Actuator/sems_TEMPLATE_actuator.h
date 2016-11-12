/** @file
 * @brief Template actuator for help develpor build custom actuator.
 * For more example you can check sems_ir_actuator.
 */
#include "sems_actuator.h"

 ///< TODO:set you actuator tag
#define SEMS_TEMPLATE_ACTUATOR_TAG        0x0000
    
     
/**
 * @brief holde TEMPLATE actuator execute data.
 */
typedef struct
{
    //TODO: Set strust.
    uint8_t result1;
    uint8_t result2;
} sems_TEMPLATE_data_t;

/**
 * @brief Holde TEMPLATE actuator config data.
 */
typedef struct
{
    //TODO: Set strust.
    uint8_t config1;
    uint8_t config2;
} sems_TEMPLATE_actuator_config_t;

/**
 * @brief Function for get a SEMS TEMPLATE actuator.
 *
 * @retval  Point to SEMS TEMPLATE actuator instance.
 */  
sems_actuator_t* get_sems_TEMPLATE_actuator();
#include "sems_module.h"
#include "app_twi.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "nrf_drv_gpiote.h"

static nrf_drv_twi_config_t const config = {
       .scl                = SEMS_MODULE_TWI_SCL,
       .sda                = SEMS_MODULE_TWI_SDA,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

app_twi_t sems_twi_instance = APP_TWI_INSTANCE(0);

ret_code_t sems_module_init(void)
{
    ret_code_t err_code;
    
    APP_TWI_INIT(&sems_twi_instance, &config, 5, err_code);
    if (err_code == NRF_SUCCESS)
    {
        err_code = nrf_drv_gpiote_init();
    }
    return err_code;
}
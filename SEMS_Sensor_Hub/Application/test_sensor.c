#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "TSL2561.h"
#include "HTU21D.h"
#include "GPIOSensor.h"
#include "sems_module.h"
#include "sems_operator.h"
#include "sems_sensor.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "app_util_platform.h"
#include "nrf_drv_twi.h"
#include "app_twi.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "test_sensor.h"
#include "sems_ir_operator.h"
#include "sems_ir_nec_encoder.h"
#include "sems_ir_raw_encoder.h"

static void data_handler(sems_sensor_t const* p_sensor, void* data, ret_code_t err_code)
{
    if (p_sensor->sensor_tag == SEMS_GPIO_TAG)
    {
        if (err_code == NRF_SUCCESS) {
            sems_gpio_pin_val_t *gpio_data = data;
            sems_gpio_sensor_config_t const *p_config = p_sensor->p_sensor_config;
            NRF_LOG_INFO("GPIO %d: %d \n", p_config->pin, *gpio_data);
        } else 
        {
            NRF_LOG_INFO("GPIO Error Code %d \n", err_code);
        }
    } else 
        if (p_sensor->sensor_tag == SEMS_HUMIDTY_TEMPERATURE_TAG) 
    {
        if (err_code == NRF_SUCCESS) {
            sems_htu21d_data_t *htu21d_data = data;
            NRF_LOG_INFO("TEMP %d  HUM %d \n", htu21d_data->tempeature, htu21d_data->humidity);
        } else 
        {
            NRF_LOG_INFO("HUM&TEMP Error Code %d \n", err_code);
        }
    } 
    else if (p_sensor->sensor_tag == SEMS_LUMINOSITY_TAG)
    {
        if (err_code == NRF_SUCCESS) {;
            float *lux_data = data;
            NRF_LOG_INFO("LUMINOSITY %d   \n", *lux_data);
        } else 
        {
            NRF_LOG_INFO("LUMINOSITY Error Code %d \n", err_code);
        }
    }else
    {
        NRF_LOG_INFO("Wrong tag \n");
    }
    NRF_LOG_FLUSH();
}

static void event_data_handler(sems_sensor_t const* p_sensor, void* p_data, ret_code_t err_code)
{
    NRF_LOG_INFO("GPIO 20 EVENT \n");
    NRF_LOG_FLUSH();
    ir_send();
}

static sems_operator_t *m_ir_operator_prt;

void test_sensor()
{
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    
    APP_TIMER_INIT(SEMS_APP_TIMER_PRESCALER, 5, false);
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);

    err_code = sems_module_init();
    APP_ERROR_CHECK(err_code);


    sems_sensor_t *p_htu21d = get_sems_htu21d_sensor();
    err_code = sems_sensor_init(p_htu21d);
    APP_ERROR_CHECK(err_code);

    sems_sensor_t *p_tsl2561 = get_sems_tsl2561_default_sensor();
    err_code = sems_sensor_init(p_tsl2561);
    APP_ERROR_CHECK(err_code);

    sems_sensor_t *p_gpio20 = create_gpio_sensor(20, NRF_GPIO_PIN_PULLUP);
    err_code = sems_sensor_init(p_gpio20);
    APP_ERROR_CHECK(err_code);

    m_ir_operator_prt = get_sems_ir_operator(24);
    err_code = sems_operator_init(m_ir_operator_prt);
    APP_ERROR_CHECK(err_code);

    err_code = sems_sensor_polling(p_htu21d, data_handler, 1000*5);
    APP_ERROR_CHECK(err_code);

    err_code = sems_sensor_polling(p_tsl2561, data_handler, 1000*10);
    APP_ERROR_CHECK(err_code);

    nrf_gpiote_polarity_t t = NRF_GPIOTE_POLARITY_HITOLO;
    err_code = sems_sensor_set_event_handler(p_gpio20, event_data_handler, &t);
    APP_ERROR_CHECK(err_code);
    		
    while (true)
    {

        //Enter System-on idle mode
        __WFE();
        __SEV();
        __WFE();	
    }
}



void ir_send()
{
//   sems_nec_data data;
//   data.address = 0xFF;
//   data.command = 0x00;
//   data.repeat = 5;
//    
//  ret_code_t err_code = sems_ir_send(&data, sems_ir_nec_encode);
    
    uint16_t raw_data [10] ={ 100,200,300,400,500,600,700,800,900,1000};

    sems_raw_data data;
    data.p_raw_data = raw_data;
    data.length = 10;
    
    sems_ir_operate_data_t ir_data;
    ir_data.p_data = &data;
    ir_data.encode_handler = sems_ir_raw_encode;
    ret_code_t err_code = sems_operator_execute(m_ir_operator_prt, &ir_data);
    if (err_code != NRF_SUCCESS)
    {
    NRF_LOG_INFO("IR BUSY \n");
    NRF_LOG_FLUSH();
    }
}
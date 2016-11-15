#include <stdlib.h>
#include <string.h> /* memset */
#include "GPIOSensor.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "sems_sensor.h"

static sems_sensor_t *gpio_sensors[32] = {NULL};

static void gpiote_evt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    sems_gpio_event_data_t data;
    
    data.action = action;
    sems_sensor_t *p_sensor = gpio_sensors[pin];
    if (p_sensor != NULL && p_sensor->event_data_handler != NULL)
    {
        sems_gpio_pin_val_t *p_val = (sems_gpio_pin_val_t*)p_sensor->p_sensor_data;
        *p_val = (sems_gpio_pin_val_t)((NRF_GPIO->IN & 1 << pin) >> pin);
        data.pin_val = pin = *p_val;
        p_sensor->event_data_handler(p_sensor, &data, NRF_SUCCESS);
    }
}

static void get_data(sems_sensor_t const *p_sensor, sems_gpio_pin_val_t *p_val)
{
    sems_gpio_sensor_config_t const *p_config = p_sensor->p_sensor_config;
    *p_val = (sems_gpio_pin_val_t)((NRF_GPIO->IN & 1 << p_config->pin) >> p_config->pin);
}

static ret_code_t get_sensor_data(sems_sensor_t const *p_sensor)
{
    sems_gpio_sensor_config_t const *p_config = p_sensor->p_sensor_config;
    if (gpio_sensors[p_config->pin] != p_sensor)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    get_data(p_sensor, p_sensor->p_sensor_data);
    return NRF_SUCCESS;
}

ret_code_t sems_gpio_set_event(sems_sensor_t const  *p_sensor, bool enable, void * p_event_config)
{
    if (p_sensor == NULL)
        return NRF_ERROR_INVALID_PARAM;
    
    sems_gpio_sensor_config_t const *p_config = p_sensor->p_sensor_config;
    if (gpio_sensors[p_config->pin] != p_sensor)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    if (enable == false)
    {
        nrf_drv_gpiote_in_uninit(p_config->pin);
        nrf_drv_gpiote_in_event_disable(p_config->pin);
        return NRF_SUCCESS;
    }
    
    nrf_gpiote_polarity_t *p_sense = p_event_config;
    ret_code_t err_code = NRF_ERROR_INVALID_DATA;
    
    if (*p_sense == NRF_GPIOTE_POLARITY_LOTOHI)
    {
        nrf_drv_gpiote_in_config_t in_config =  GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);
        in_config.pull = p_config->pull;
        err_code = nrf_drv_gpiote_in_init(p_config->pin, &in_config, gpiote_evt_handler);
    }
    if (*p_sense  == NRF_GPIOTE_POLARITY_HITOLO)
    {
         nrf_drv_gpiote_in_config_t in_config =  GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
         in_config.pull = p_config->pull;
         err_code = nrf_drv_gpiote_in_init(p_config->pin, &in_config, gpiote_evt_handler);
    }
    if (*p_sense  == NRF_GPIOTE_POLARITY_TOGGLE)
    {
         nrf_drv_gpiote_in_config_t in_config =  GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
         in_config.pull = p_config->pull;
         err_code = nrf_drv_gpiote_in_init(p_config->pin, &in_config, gpiote_evt_handler);
    }
    if (err_code == NRF_SUCCESS)
    { 
        nrf_drv_gpiote_in_event_enable(p_config->pin, false);
    }
    return err_code;
}

ret_code_t sems_gpio_sensor_init(sems_sensor_t const *p_sensor)
{
    if (p_sensor == NULL)
        return NRF_ERROR_INVALID_PARAM;

    sems_gpio_sensor_config_t const *p_config = p_sensor->p_sensor_config;
    if (gpio_sensors[p_config->pin] != p_sensor)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    
    uint32_t pin_number = p_config->pin;
    nrf_gpio_pin_pull_t pull_config = p_config->pull;

    nrf_gpio_cfg_input(pin_number, pull_config);
    
    return  NRF_SUCCESS;
}


ret_code_t sems_gpio_sensor_uninit(sems_sensor_t const *p_sensor)
{
    if (p_sensor == NULL)
        return NRF_ERROR_INVALID_PARAM;
    
    sems_gpio_sensor_config_t *p_config = p_sensor->p_sensor_config;
    if (gpio_sensors[p_config->pin] != p_sensor)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    nrf_drv_gpiote_pin_t pin = p_config->pin;
    
    nrf_drv_gpiote_in_uninit(pin);
    nrf_drv_gpiote_in_event_disable(pin);
    
    free(p_config);
    free(gpio_sensors[pin]->p_sensor_data);
    free(gpio_sensors[pin]->app_timer_id);
    free(gpio_sensors[pin]);
    gpio_sensors[pin] = NULL;
    p_sensor = NULL;
    return NRF_SUCCESS;
}

sems_sensor_t* get_gpio_sensor(nrf_drv_gpiote_pin_t pin, nrf_gpio_pin_pull_t pull)
{ 
    if (pin >31)
    {
        return NULL;
    }
    if (gpio_sensors[pin] != NULL)
    {
        sems_gpio_sensor_config_t const *p_config = gpio_sensors[pin]->p_sensor_config;
        if (p_config->pull == pull && p_config->pin == pin)
        {
            return gpio_sensors[pin];
        } else 
        {
            return NULL;
        }
    }
    
    sems_gpio_pin_val_t *p_data = (sems_gpio_pin_val_t*)malloc(sizeof(sems_gpio_pin_val_t));
    sems_gpio_sensor_config_t *p_config = (sems_gpio_sensor_config_t*)malloc(sizeof(sems_gpio_sensor_config_t));
    app_timer_t *p_timer = (app_timer_t*)malloc(sizeof(app_timer_t));
    sems_sensor_t *p_sensor = (sems_sensor_t*)malloc(sizeof(sems_sensor_t));

    if (p_data == NULL || p_config == NULL || p_timer == NULL || p_sensor == NULL)
    {
        free(p_config);
        free(p_data);
        free(p_timer);
        free(p_sensor);
        return NULL;
        
    }
    
    *p_data = false;
    p_config->pin = pin;
    p_config->pull = pull;
    memset(p_timer, 0, sizeof(app_timer_t));
    
    SEMS_SENSOR_SETUP(p_sensor, p_timer, SEMS_GPIO_TAG, p_config, p_data, sizeof(sems_gpio_pin_val_t), get_sensor_data, sems_gpio_sensor_init,  sems_gpio_sensor_uninit, sems_gpio_set_event);
    
    gpio_sensors[pin] = p_sensor;
    return p_sensor;
}







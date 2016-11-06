#include "sems_sensor.h"
#include "app_error.h"
#include "app_util_platform.h"


static void timeout_handler(void * p_context)
{
    sems_sensor_t *p_sensor = p_context;
     ret_code_t err_code;
     
    if (p_sensor->get_sensor_data == NULL)
    {
        if (p_sensor->polling_data_handler != NULL)  
        {
            p_sensor->polling_data_handler(p_sensor, NULL, NRF_ERROR_INVALID_DATA);
        }
        return;
    }
    void *p_data = p_sensor->p_sensor_data;
    err_code = p_sensor->get_sensor_data(p_sensor);
    
    if (p_sensor->polling_data_handler != NULL)  
    {
        p_sensor->polling_data_handler(p_sensor, p_data, err_code);
    }
    return;
}

ret_code_t sems_sensor_flush_data(sems_sensor_t *p_sensor)
{
    if (p_sensor == NULL || p_sensor->p_sensor_data == NULL)
        return NRF_ERROR_INVALID_PARAM;
    
    if (p_sensor->sensor_state != SEMS_SENSOR_INITED)
        return NRF_ERROR_INVALID_STATE;
    if (p_sensor->get_sensor_data == NULL)
        return NRF_ERROR_INVALID_DATA;
    ret_code_t err_code;
    err_code = p_sensor->get_sensor_data(p_sensor);
    return err_code;
}

ret_code_t sems_sensor_polling(sems_sensor_t *p_sensor, sems_sensor_data_handler_t polling_data_handler, uint32_t cycle_ms)
{
    if (p_sensor == NULL)
        return NRF_ERROR_INVALID_PARAM;
    if (p_sensor->sensor_state == SEMS_SENSOR_UNINITED)
        return NRF_ERROR_INVALID_STATE;
     if (p_sensor->get_sensor_data == NULL)
        return NRF_ERROR_INVALID_DATA;
     
    ret_code_t err_code;
    
    if (p_sensor->sensor_state == SEMS_SENSOR_POLLING)  //stop timer first
    {
        err_code = app_timer_stop(p_sensor->app_timer_id);
        if (err_code != NRF_SUCCESS)
            return err_code;
        p_sensor->sensor_state = SEMS_SENSOR_INITED;
        p_sensor->polling_data_handler = NULL;
    }
    
    if (cycle_ms == 0) //stop
        return NRF_SUCCESS;
    
    if ( polling_data_handler == NULL)
        return NRF_ERROR_INVALID_PARAM;
    
    err_code = app_timer_create(&p_sensor->app_timer_id, APP_TIMER_MODE_REPEATED, timeout_handler);
    if (err_code != NRF_SUCCESS)
            return err_code;
    err_code = app_timer_start(p_sensor->app_timer_id, APP_TIMER_TICKS(cycle_ms, SEMS_APP_TIMER_PRESCALER), p_sensor);
    if (err_code == NRF_SUCCESS)
    {
        p_sensor->polling_data_handler = polling_data_handler;
        p_sensor->sensor_state = SEMS_SENSOR_POLLING;
    }
    return err_code;
}

ret_code_t sems_sensor_init(sems_sensor_t *p_sensor)
{
    if (p_sensor ==NULL)
        return NRF_ERROR_INVALID_PARAM;
    if (p_sensor->sensor_init == NULL)
    {
        return NRF_ERROR_INVALID_DATA;
    } else
    {
        if (p_sensor->sensor_state != SEMS_SENSOR_UNINITED)
            return NRF_ERROR_INVALID_STATE;

        ret_code_t err_code = p_sensor->sensor_init(p_sensor);
        if (err_code == NRF_SUCCESS)
        {
            p_sensor->sensor_state = SEMS_SENSOR_INITED;
        }
        
        return err_code;
    }
}
    
ret_code_t sems_sensor_uninit(sems_sensor_t *p_sensor)
{
    if (p_sensor ==NULL)
        return NRF_ERROR_INVALID_PARAM;
    if (p_sensor->sensor_uninit == NULL)
    {
        return NRF_ERROR_INVALID_DATA;
    } else
    {
        if (p_sensor->sensor_state != SEMS_SENSOR_INITED)  
            return NRF_ERROR_INVALID_STATE;
        
        ret_code_t err_code  = p_sensor->sensor_uninit(p_sensor);
        if (err_code == NRF_SUCCESS)
        {
            p_sensor->sensor_state = SEMS_SENSOR_UNINITED;
        }
        
        return err_code;
    }
}
    
ret_code_t sems_sensor_set_event_handler(sems_sensor_t *p_sensor, sems_sensor_event_handler_t p_event_handler, void * p_event_config)
{
    if (p_sensor ==NULL)
        return NRF_ERROR_INVALID_PARAM;
    if (p_sensor->set_sensor_event == NULL)
    {
         return NRF_ERROR_NOT_SUPPORTED;
    } else 
    {
        if (p_sensor->sensor_state == SEMS_SENSOR_UNINITED)  
            return NRF_ERROR_INVALID_STATE;
        ret_code_t err_code;
        if (p_event_handler == NULL)
        {
            err_code = p_sensor->set_sensor_event(p_sensor, false, p_event_config);
            if (err_code == NRF_SUCCESS)
            {
                p_sensor->event_data_handler = NULL;
            }
        }
        else
        {
            err_code = p_sensor->set_sensor_event(p_sensor, true, p_event_config);
            if (err_code == NRF_SUCCESS)
            {
                p_sensor->event_data_handler = p_event_handler;
            }
        }
        return err_code;
    }
}
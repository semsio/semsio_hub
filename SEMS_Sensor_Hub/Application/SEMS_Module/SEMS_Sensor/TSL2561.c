#include <math.h>
#include <stdlib.h>
#include "TSL2561.h"
#include "app_timer.h"
#include "app_twi.h"
#include "app_error.h"
#include "sems_sensor.h"

static sems_sensor_t *p_sensor_0 = NULL;
static sems_sensor_t *p_sensor_1 = NULL;
static sems_sensor_t *p_sensor_2 = NULL;


static ret_code_t get_tsl2561_power(app_twi_t* p_twi, uint8_t slave_addr, bool *pow_state)
{
    uint8_t buffer[TSL2561_BUFFER_SIZE];
    app_twi_transfer_t twi_transfer = APP_TWI_TRANSFER(APP_TWI_WRITE_OP(slave_addr), buffer,1 , APP_TWI_NO_STOP);
    
    buffer[0] = (TSL2561_REG_CONTROL & 0x0F) | TSL2561_CMD;
    ret_code_t err_code = app_twi_perform(p_twi, &twi_transfer,1, NULL);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    twi_transfer.operation = APP_TWI_READ_OP(slave_addr);
    err_code = app_twi_perform(p_twi, &twi_transfer,1, NULL);  
    if (err_code == NRF_SUCCESS)
    {
        if (buffer[0] == 0)
        {
            *pow_state = false;
        } else 
        {
            *pow_state = true;
        }
    }
    return err_code;
}

static ret_code_t set_tsl2561_power(app_twi_t* p_twi, uint8_t slave_addr, bool pow_state)
{
    uint8_t buffer[TSL2561_BUFFER_SIZE];
    app_twi_transfer_t twi_transfer = APP_TWI_TRANSFER(APP_TWI_WRITE_OP(slave_addr), buffer,2 , APP_TWI_NO_STOP);

    buffer[0] = (TSL2561_REG_CONTROL & 0x0F) | TSL2561_CMD;
    if (pow_state)
    {
        buffer[1] = 0x03;
    } else {
        buffer[1] = 0x00;
    }
    ret_code_t err_code = app_twi_perform(p_twi, &twi_transfer,1 , NULL);
    return err_code;
}

static ret_code_t set_tsl2561_timing(app_twi_t* p_twi, uint8_t slave_addr, bool is_gain, integrate_time_t time)
{
    uint8_t buffer[TSL2561_BUFFER_SIZE];
    app_twi_transfer_t twi_transfer = APP_TWI_TRANSFER(APP_TWI_WRITE_OP(slave_addr), buffer,2 , APP_TWI_NO_STOP);
    
    buffer[0] = (TSL2561_REG_TIMING & 0x0F) | TSL2561_CMD;
    if (is_gain)
    {
        buffer[1] = 0x10;
    } else 
    {
        buffer[1] = 0x00;
    }
    switch(time)
    {
        case TSL2561_TIME0:
            break;
        case TSL2561_TIME1:
            buffer[1] |= 0x01;
            break;
        case TSL2561_TIME2:
            buffer[1] |= 0x02;
            break;
        default:
            break;
        
    }
    ret_code_t err_code = app_twi_perform(p_twi, &twi_transfer, 1, NULL);
    return err_code;
}

static ret_code_t get_tsl2561_timing(app_twi_t* p_twi, uint8_t slave_addr, bool *is_gain, integrate_time_t *time)
{
    uint8_t buffer[TSL2561_BUFFER_SIZE];
    app_twi_transfer_t twi_transfer = APP_TWI_TRANSFER(APP_TWI_WRITE_OP(slave_addr), buffer,1 , APP_TWI_NO_STOP);
   
    buffer[0] = (TSL2561_REG_TIMING & 0x0F) | TSL2561_CMD;
    ret_code_t err_code = app_twi_perform(p_twi, &twi_transfer, 1, NULL);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    twi_transfer.operation = APP_TWI_READ_OP(slave_addr);
    err_code = app_twi_perform(p_twi, &twi_transfer,1 , NULL);  
    if (err_code == NRF_SUCCESS)
    {
        *time = (integrate_time_t)(buffer[0] & 0x03);
        *is_gain = (bool)buffer >> 4;
    }
    return err_code;
}

static ret_code_t get_tsl2561_data0(app_twi_t* p_twi, uint8_t slave_addr, uint16_t *data0)
{
    uint8_t buffer[TSL2561_BUFFER_SIZE];
    app_twi_transfer_t twi_transfer = APP_TWI_TRANSFER(APP_TWI_WRITE_OP(slave_addr), buffer,1 , APP_TWI_NO_STOP);
    
    buffer[0] = (TSL2561_REG_DATA_0 & 0x0F) | TSL2561_CMD;
    ret_code_t err_code = app_twi_perform(p_twi, &twi_transfer,1 , NULL);  
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    twi_transfer.operation = APP_TWI_READ_OP(slave_addr);
    twi_transfer.length = 2;
    err_code = app_twi_perform(p_twi, &twi_transfer,1 , NULL);  
    if (err_code == NRF_SUCCESS)
    {
        *data0 = buffer[1];
        *data0 = *data0 << 8;
        *data0 = *data0 | buffer[0];
    }
    return err_code;
}

static ret_code_t get_tsl2561_data1(app_twi_t* p_twi, uint8_t slave_addr, uint16_t *data1)
{
    uint8_t buffer[TSL2561_BUFFER_SIZE];
    app_twi_transfer_t twi_transfer = APP_TWI_TRANSFER(APP_TWI_WRITE_OP(slave_addr), buffer,1 , APP_TWI_NO_STOP);

    buffer[0] = (TSL2561_REG_DATA_1 & 0x0F) | TSL2561_CMD;
    ret_code_t err_code = app_twi_perform(p_twi, &twi_transfer,1 , NULL);  
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    twi_transfer.operation = APP_TWI_READ_OP(slave_addr);
    twi_transfer.length = 2;
    err_code = app_twi_perform(p_twi, &twi_transfer,1 , NULL);  
    if (err_code == NRF_SUCCESS)
    {
        *data1 = buffer[1];
        *data1 = *data1 << 8;
        *data1 = *data1 | buffer[0];
    }
    return err_code;
}

static bool get_lux(bool gain, integrate_time_t time, uint16_t data0, uint32_t data1, float *lux)
{
	if ((data0 == 0xFFFF) || (data1 == 0xFFFF))
	{
		return false;
	}
	else
	{
        int ms = 0;
        switch(time)
        {
            case TSL2561_TIME0:
                ms =14;
                break;
            case TSL2561_TIME1:
                ms = 101;
                break;
            case TSL2561_TIME2:
                ms = 402;
                break;
            default:
                break;
            
        }
        
		float d0,d1;
		d0 = data0;
		d1 = data1;
		d0 *= (402.0/ms);
		d1 *= (402.0/ms);
		double ratio = d1 / d0;
		if (!gain)
		{
			d1 *= 16;
			d0 *= 16;
		}
		if (ratio < 0.5)
		{
			*lux = d0*(0.0304 - 0.062 * pow(ratio,1.4));
			return true;
		}

		if (ratio < 0.61)
		{
			*lux = 0.0224 * d0 - 0.031 * d1;
			return true;
		}

		if (ratio < 0.80)
		{
			*lux = 0.0128 * d0 - 0.0153 * d1;
			return true;
		}

		if (ratio < 1.30)
		{
			*lux = 0.00146 * d0 - 0.00112 * d1;
			return true;
		}
		*lux = 0.0;
		return true;
	}
}

static ret_code_t get_data(sems_sensor_t const *p_sensor, sems_tsl2561_lux_t* p_data)
{
    sems_tsl2561_sensor_config_t *p_config = p_sensor->p_sensor_config;
    integrate_time_t time;
    bool is_gain;
    bool pow_state;
    
    
    ret_code_t err_code = get_tsl2561_power(p_config->p_app_twi, p_config->slave_addr ,&pow_state);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    if (pow_state == false)
    {
        return NRF_ERROR_INTERNAL;
    }
    
    err_code = get_tsl2561_timing(p_config->p_app_twi, p_config->slave_addr, &is_gain, &time);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    if (time != TSL2561_TIME2 || is_gain) 
    {
        return NRF_ERROR_INTERNAL;
    }
    
    uint16_t data0, data1;
    err_code = get_tsl2561_data0(p_config->p_app_twi, p_config->slave_addr, &data0);
    if (err_code != NRF_SUCCESS)
        return err_code;
    err_code = get_tsl2561_data1(p_config->p_app_twi, p_config->slave_addr, &data1);
    if (err_code != NRF_SUCCESS)
        return err_code;
    
    if (get_lux(is_gain, time, data0, data1, p_data))
    {
        return err_code;
    } else 
    {
        return NRF_ERROR_INVALID_DATA;
    }
}

static ret_code_t get_sensor_data(sems_sensor_t const *p_sensor)
{
    if (p_sensor != p_sensor_0 && p_sensor != p_sensor_1 && p_sensor != p_sensor_2)
    {
       return NRF_ERROR_INVALID_PARAM; 
    }
    return get_data(p_sensor, p_sensor->p_sensor_data);
}

static void free_tsl2561_sensor(sems_sensor_t* p_sensor)
{
    free(p_sensor->p_sensor_config);
    p_sensor->p_sensor_config = NULL;
    
    free(p_sensor->p_sensor_data);
    p_sensor->p_sensor_data = NULL;
    
    free(p_sensor);
    p_sensor = NULL;
    
}

static ret_code_t sems_tsl2561_sensor_init(sems_sensor_t const  *p_sensor)
{
    if (p_sensor != p_sensor_0 && p_sensor != p_sensor_1 && p_sensor != p_sensor_2)
    {
       return NRF_ERROR_INVALID_PARAM; 
    }

    sems_tsl2561_sensor_config_t *p_config = p_sensor->p_sensor_config;
    ret_code_t err_code = set_tsl2561_power(p_config->p_app_twi, p_config->slave_addr, true);
    if (err_code != NRF_SUCCESS)
    {
        
        free_tsl2561_sensor((sems_sensor_t*)p_sensor);
    
        if (p_sensor == p_sensor_0)
        {
            p_sensor_0 = NULL;
        } else if (p_sensor == p_sensor_1)
        {
            p_sensor_1 = NULL;
        } else if (p_sensor == p_sensor_2)
        {
            p_sensor_2 = NULL;
        }
        return err_code;
    }

    err_code = set_tsl2561_timing(p_config->p_app_twi, p_config->slave_addr, false, TSL2561_TIME2);
    if (err_code != NRF_SUCCESS)
    {
        free_tsl2561_sensor((sems_sensor_t*)p_sensor);
    
        if (p_sensor == p_sensor_0)
        {
            p_sensor_0 = NULL;
        } else if (p_sensor == p_sensor_1)
        {
            p_sensor_1 = NULL;
        } else if (p_sensor == p_sensor_2)
        {
            p_sensor_2 = NULL;
        }
        return err_code;
    }

    
    return NRF_SUCCESS;
}

static ret_code_t sems_tsl2561_sensor_uninit(sems_sensor_t const  *p_sensor)
{
    if (p_sensor != p_sensor_0 && p_sensor != p_sensor_1 && p_sensor != p_sensor_2)
    {
       return NRF_ERROR_INVALID_PARAM; 
    }
    sems_tsl2561_sensor_config_t *p_config = p_sensor->p_sensor_config;
    ret_code_t err_code = set_tsl2561_power(p_config->p_app_twi, p_config->slave_addr, false);
    if (err_code != NRF_SUCCESS)
        return err_code;
    
    free_tsl2561_sensor((sems_sensor_t*)p_sensor);
    
    if (p_sensor == p_sensor_0)
    {
        p_sensor_0 = NULL;
    } else if (p_sensor == p_sensor_1)
    {
        p_sensor_1 = NULL;
    } else if (p_sensor == p_sensor_2)
    {
        p_sensor_2 = NULL;
    }
    
    return NRF_SUCCESS;
}

sems_sensor_t* get_sems_tsl2561_default_sensor()
{  
    if (p_sensor_0 != NULL)
    {
        return p_sensor_0;
    }
    
    sems_tsl2561_sensor_config_t *p_config = (sems_tsl2561_sensor_config_t*)malloc(sizeof(sems_tsl2561_sensor_config_t));
    sems_tsl2561_lux_t *p_data = (sems_tsl2561_lux_t*)malloc(sizeof(sems_tsl2561_lux_t));
    p_sensor_0 = (sems_sensor_t*)malloc(sizeof(sems_sensor_t));
    
    if (p_config == NULL || p_data == NULL || p_sensor_0 == NULL)
    {
        free(p_sensor_0);
        free(p_data);
        free(p_config);
        p_sensor_0 = NULL;
        p_data = NULL;
        p_config = NULL;
        return NULL;
    }
    
    *p_data = 0;
    TSL2561_CONFIG_DEF_SETUP(p_config);
    
    SEMS_SENSOR_SETUP_WITH_OUT_TIMER(p_sensor_0,SEMS_LUMINOSITY_TAG, p_config, p_data, sizeof(sems_tsl2561_lux_t), get_sensor_data, sems_tsl2561_sensor_init, sems_tsl2561_sensor_uninit, NULL);
    return p_sensor_0;
}

sems_sensor_t* get_sems_tsl2561_addr49_sensor()
{
    if (p_sensor_1 != NULL)
    {
        return p_sensor_1;
    }
    
    sems_tsl2561_sensor_config_t *p_config = (sems_tsl2561_sensor_config_t*)malloc(sizeof(sems_tsl2561_sensor_config_t));
    sems_tsl2561_lux_t *p_data = (sems_tsl2561_lux_t*)malloc(sizeof(sems_tsl2561_lux_t));
    p_sensor_1 = (sems_sensor_t*)malloc(sizeof(sems_sensor_t));
    
    if (p_config == NULL || p_data == NULL || p_sensor_1 == NULL)
    {
        free(p_sensor_1);
        free(p_data);
        free(p_config);
        p_sensor_0 = NULL;
        p_data = NULL;
        p_config = NULL;
        return NULL;
    }
    
    *p_data = 0;
    TSL2561_CONFIG_ADD49_SETUP(p_config);
    
    SEMS_SENSOR_SETUP_WITH_OUT_TIMER(p_sensor_1,SEMS_LUMINOSITY_TAG, p_config, p_data, sizeof(sems_tsl2561_lux_t), get_sensor_data, sems_tsl2561_sensor_init, sems_tsl2561_sensor_uninit, NULL);
    return p_sensor_1;
}

sems_sensor_t* get_sems_tsl2561_addr29_sensor()
{
    if (p_sensor_2 != NULL)
    {
        return p_sensor_2;
    }
    
    sems_tsl2561_sensor_config_t *p_config = (sems_tsl2561_sensor_config_t*)malloc(sizeof(sems_tsl2561_sensor_config_t));
    sems_tsl2561_lux_t *p_data = (sems_tsl2561_lux_t*)malloc(sizeof(sems_tsl2561_lux_t));
    p_sensor_2 = (sems_sensor_t*)malloc(sizeof(sems_sensor_t));
    
    if (p_config == NULL || p_data == NULL || p_sensor_2 == NULL)
    {
        free(p_sensor_2);
        free(p_data);
        free(p_config);
        p_sensor_0 = NULL;
        p_data = NULL;
        p_config = NULL;
        return NULL;
    }
    
    *p_data = 0;
    TSL2561_CONFIG_ADD29_SETUP(p_config);
    
    SEMS_SENSOR_SETUP_WITH_OUT_TIMER(p_sensor_2,SEMS_LUMINOSITY_TAG, p_config, p_data, sizeof(sems_tsl2561_lux_t), get_sensor_data, sems_tsl2561_sensor_init, sems_tsl2561_sensor_uninit, NULL);
    return p_sensor_2;
}







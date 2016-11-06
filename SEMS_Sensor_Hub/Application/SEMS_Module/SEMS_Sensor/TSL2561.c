#include <math.h>
#include "TSL2561.h"
#include "app_timer.h"
#include "app_twi.h"
#include "app_error.h"
#include "sems_sensor.h"

//static ret_code_t get_tsl2561_id(sems_tsl2561_sensor_config_t const* p_config, app_twi_transfer_t *p_write_transfer, app_twi_transfer_t *p_read_transfer, uint8_t *id)
//{
//    uint8_t *data_buffer = p_write_transfer->p_data;
//    p_write_transfer->length = 1;
//    p_read_transfer->length = 1;
//    data_buffer[0] = (TSL2561_REG_ID & 0x0F) | TSL2561_CMD;
//    ret_code_t err_code = app_twi_perform(p_config->p_app_twi, p_write_transfer,1, NULL);
//    if (err_code != NRF_SUCCESS)
//    {
//        return err_code;
//    }
//    err_code = app_twi_perform(p_config->p_app_twi, p_read_transfer,1, NULL);  
//    if (err_code == NRF_SUCCESS)
//    {
//        *id = data_buffer[0];
//    }
//    return err_code;
//}

static ret_code_t get_tsl2561_power(sems_tsl2561_sensor_config_t const* p_config, app_twi_transfer_t *p_write_transfer, app_twi_transfer_t *p_read_transfer, bool *pow_state)
{
    uint8_t *data_buffer = p_write_transfer->p_data;
    p_write_transfer->length = 1;
    p_read_transfer->length = 1;
    data_buffer[0] = (TSL2561_REG_CONTROL & 0x0F) | TSL2561_CMD;
    ret_code_t err_code = app_twi_perform(p_config->p_app_twi, p_write_transfer,1, NULL);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    err_code = app_twi_perform(p_config->p_app_twi, p_read_transfer,1, NULL);  
    if (err_code == NRF_SUCCESS)
    {
        if (data_buffer[0] == 0)
        {
            *pow_state = false;
        } else 
        {
            *pow_state = true;
        }
    }
    return err_code;
}

static ret_code_t set_tsl2561_power(sems_tsl2561_sensor_config_t const* p_config, app_twi_transfer_t *p_write_transfer, bool pow_state)
{
    uint8_t *data_buffer = p_write_transfer->p_data;
    p_write_transfer->length = 2;
    data_buffer[0] = (TSL2561_REG_CONTROL & 0x0F) | TSL2561_CMD;
    if (pow_state)
    {
        data_buffer[1] = 0x03;
    } else {
        data_buffer[1] = 0x00;
    }
    ret_code_t err_code = app_twi_perform(p_config->p_app_twi, p_write_transfer,1 , NULL);
    return err_code;
}

static ret_code_t set_tsl2561_timing(sems_tsl2561_sensor_config_t const* p_config, app_twi_transfer_t *p_write_transfer, bool is_gain, integrate_time_t time)
{
    uint8_t *data_buffer = p_write_transfer->p_data;
    p_write_transfer->length = 2;
    data_buffer[0] = (TSL2561_REG_TIMING & 0x0F) | TSL2561_CMD;
    if (is_gain)
    {
        data_buffer[1] = 0x10;
    } else 
    {
        data_buffer[1] = 0x00;
    }
    switch(time)
    {
        case TSL2561_TIME0:
            break;
        case TSL2561_TIME1:
            data_buffer[1] |= 0x01;
            break;
        case TSL2561_TIME2:
            data_buffer[1] |= 0x02;
            break;
        default:
            break;
        
    }
    ret_code_t err_code = app_twi_perform(p_config->p_app_twi, p_write_transfer, 1, NULL);
    return err_code;
}

static ret_code_t get_tsl2561_timing(sems_tsl2561_sensor_config_t const *p_config, app_twi_transfer_t *p_write_transfer, app_twi_transfer_t *p_read_transfer, bool *is_gain, integrate_time_t *time)
{
    uint8_t *data_buffer = p_write_transfer->p_data;
    p_write_transfer->length = 1;
    p_read_transfer->length = 1;
    data_buffer[0] = (TSL2561_REG_TIMING & 0x0F) | TSL2561_CMD;
    ret_code_t err_code = app_twi_perform(p_config->p_app_twi, p_write_transfer,1, NULL);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    err_code = app_twi_perform(p_config->p_app_twi, p_read_transfer,1, NULL);  
    if (err_code == NRF_SUCCESS)
    {
        *time = (integrate_time_t)(data_buffer[0] & 0x03);
        *is_gain = (bool)data_buffer >> 4;
    }
    return err_code;
}

static ret_code_t get_tsl2561_data0(sems_tsl2561_sensor_config_t const* p_config, app_twi_transfer_t *p_write_transfer, app_twi_transfer_t *p_read_transfer, uint16_t *data0)
{
    uint8_t *data_buffer = p_write_transfer->p_data;
    p_write_transfer->length = 1;
    p_read_transfer->length = 2;
    data_buffer[0] = (TSL2561_REG_DATA_0 & 0x0F) | TSL2561_CMD;
    ret_code_t err_code = app_twi_perform(p_config->p_app_twi, p_write_transfer,1, NULL);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    err_code = app_twi_perform(p_config->p_app_twi, p_read_transfer,1, NULL);  
    if (err_code == NRF_SUCCESS)
    {
        *data0 = data_buffer[1];
        *data0 = *data0 << 8;
        *data0 = *data0 | data_buffer[0];
    }
    return err_code;
}

static ret_code_t get_tsl2561_data1(sems_tsl2561_sensor_config_t const* p_config, app_twi_transfer_t *p_write_transfer, app_twi_transfer_t *p_read_transfer, uint16_t *data1)
{
    uint8_t *data_buffer = p_write_transfer->p_data;
    p_write_transfer->length = 1;
    p_read_transfer->length = 2;
    data_buffer[0] = (TSL2561_REG_DATA_1 & 0x0F) | TSL2561_CMD;
    ret_code_t err_code = app_twi_perform(p_config->p_app_twi, p_write_transfer,1, NULL);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    err_code = app_twi_perform(p_config->p_app_twi, p_read_transfer,1, NULL);  
    if (err_code == NRF_SUCCESS)
    {
        *data1 = data_buffer[1];
        *data1 = *data1 << 8;
        *data1 = *data1 | data_buffer[0];
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

static ret_code_t get_tsl2561_lux(sems_tsl2561_sensor_config_t const* p_config, app_twi_transfer_t *p_write_transfer, app_twi_transfer_t *p_read_transfer, bool is_gain, integrate_time_t time, uint16_t *p_data0, uint16_t *p_data1, sems_tsl2561_lux_t *p_lux)
{
    ret_code_t err_code;
    err_code = get_tsl2561_data0(p_config, p_write_transfer, p_read_transfer, p_data0);
    if (err_code != NRF_SUCCESS)
        return err_code;
    err_code = get_tsl2561_data1(p_config, p_write_transfer, p_read_transfer, p_data1);
    if (err_code != NRF_SUCCESS)
        return err_code;
    if (get_lux(is_gain, time, *p_data0, *p_data1, p_lux))
    {
        return err_code;
    } else 
    {
        return NRF_ERROR_INVALID_DATA;
    }
}


static uint8_t buffer[2];
static app_twi_transfer_t write_transfer;
static app_twi_transfer_t read_transfer;
static bool is_busy;

static ret_code_t get_data(sems_sensor_t const *p_sensor, sems_tsl2561_lux_t* p_data)
{
    if (is_busy == true) 
    {
        return NRF_ERROR_BUSY;
    }
    is_busy = true;
    
    sems_tsl2561_sensor_config_t const *p_config = p_sensor->p_sensor_config;
    integrate_time_t time;
    bool is_gain;
    bool pow_state;
    
    
    ret_code_t err_code = get_tsl2561_power(p_config, &write_transfer, &read_transfer, &pow_state);
    if (err_code != NRF_SUCCESS)
    {
        is_busy = false;
        return err_code;
    }
    if (pow_state == false)
    {
        err_code = set_tsl2561_power(p_config, &write_transfer, true);
        if (err_code != NRF_SUCCESS)
        {
            is_busy = false;
            return err_code;
        }
    }
    
    err_code = get_tsl2561_timing(p_config, &write_transfer, &read_transfer, &is_gain, &time);
    if (err_code != NRF_SUCCESS)
    {
        is_busy = false;
        return err_code;
    }
    if (time != TSL2561_TIME2 || is_gain) 
    {
        is_gain = false;
        time = TSL2561_TIME2;
        err_code = set_tsl2561_timing(p_config, &read_transfer, is_gain, time);
        if (err_code != NRF_SUCCESS)
        {
            is_busy = false;
            return err_code;
        }
    }
    
    uint16_t data0, data1;
    err_code = get_tsl2561_lux(p_config, &write_transfer, &read_transfer, is_gain, time, &data0, &data1, p_data);
    if (err_code != NRF_SUCCESS)
    {
        is_busy = false;
        return err_code;
    }
    is_busy = false;
    return err_code;
}

static ret_code_t get_sensor_data(sems_sensor_t const *p_sensor)
{
    return get_data(p_sensor, p_sensor->p_sensor_data);
}

static void init_twi_transfer(sems_tsl2561_sensor_config_t const *p_config)
{
    write_transfer.operation = APP_TWI_WRITE_OP(p_config->slave_addr);
    write_transfer.flags = APP_TWI_NO_STOP;
    write_transfer.length =1;
    write_transfer.p_data = buffer;
    
    read_transfer.operation = APP_TWI_READ_OP(p_config->slave_addr);
    read_transfer.flags = APP_TWI_NO_STOP;
    read_transfer.length =1;
    read_transfer.p_data = buffer;

}

static ret_code_t sems_tsl2561_sensor_init(sems_sensor_t const  *p_sensor)
{
    is_busy = false;
    init_twi_transfer(p_sensor->p_sensor_config);
    
    ret_code_t err_code = set_tsl2561_power(p_sensor->p_sensor_config, &write_transfer, true);
    if (err_code != NRF_SUCCESS)
        return err_code;
    
    err_code = set_tsl2561_timing(p_sensor->p_sensor_config, &write_transfer, false, TSL2561_TIME2);
    if (err_code != NRF_SUCCESS)
        return err_code;
    return NRF_SUCCESS;
}

static ret_code_t sems_tsl2561_sensor_uninit(sems_sensor_t const  *p_sensor)
{
    is_busy = false;
    
    ret_code_t err_code = set_tsl2561_power(p_sensor->p_sensor_config, &write_transfer, false);
    if (err_code != NRF_SUCCESS)
        return err_code;
    
    return NRF_SUCCESS;
}

const static sems_tsl2561_sensor_config_t tsl2561_config_0 = TSL2561_CONFIG_0;
static sems_tsl2561_lux_t tsl2561_data_0 = 0;

const static sems_tsl2561_sensor_config_t tsl2561_config_1 = TSL2561_CONFIG_1;
static sems_tsl2561_lux_t tsl2561_data_1 = 0;

const static sems_tsl2561_sensor_config_t tsl2561_config_2 = TSL2561_CONFIG_2;
static sems_tsl2561_lux_t tsl2561_data_2 = 0;

static sems_sensor_t sensor_0;
static sems_sensor_t sensor_1;
static sems_sensor_t sensor_2;


sems_sensor_t* get_sems_tsl2561_default_sensor()
{ 
    SEMS_SENSOR_INIT(sensor_0,SEMS_LUMINOSITY_TAG, &tsl2561_config_0, &tsl2561_data_0, sizeof(sems_tsl2561_lux_t), get_sensor_data, sems_tsl2561_sensor_init, sems_tsl2561_sensor_uninit, NULL);
    return &sensor_0;
}

sems_sensor_t* get_sems_tsl2561_addr49_sensor()
{
    SEMS_SENSOR_INIT(sensor_1,SEMS_LUMINOSITY_TAG, &tsl2561_config_1, &tsl2561_data_1, sizeof(sems_tsl2561_lux_t), get_sensor_data, sems_tsl2561_sensor_init, sems_tsl2561_sensor_uninit, NULL);
    return &sensor_1;
}

sems_sensor_t* get_sems_tsl2561_addr29_sensor()
{
    SEMS_SENSOR_INIT(sensor_2,SEMS_LUMINOSITY_TAG, &tsl2561_config_2, &tsl2561_data_2, sizeof(sems_tsl2561_lux_t), get_sensor_data, sems_tsl2561_sensor_init, sems_tsl2561_sensor_uninit, NULL);
    return &sensor_2;
}



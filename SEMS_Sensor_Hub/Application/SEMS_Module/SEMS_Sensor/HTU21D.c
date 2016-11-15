#ifndef _HTU21D_H
#define _HTU21D_H

#include "HTU21D.h"
#include <stdlib.h>
#include "nrf_delay.h"
#include "app_error.h"
#include "app_timer.h"

#pragma once

//Give this function the 2 byte message (measurement) and the check_value byte from the HTU21D
//If it returns 0, then the transmission was good
//If it returns something other than 0, then the communication was corrupted
//From: http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html
//POLYNOMIAL = 0x0131 = x^8 + x^5 + x^4 + 1 : http://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
#define SHIFTED_DIVISOR 0x988000 //This is the 0x0131 polynomial shifted to farthest left of three bytes

#define HTU21D_READ_GAP 0x32

static sems_sensor_t *p_singleton_sensor = NULL;

static uint8_t checkCRC(uint16_t sensorData, uint8_t checkSum)
{
    //Test cases from datasheet:
    //message = 0xDC, checkvalue is 0x79
    //message = 0x683A, checkvalue is 0x7C
    //message = 0x4E85, checkvalue is 0x6B
    uint32_t remainder = (uint32_t)sensorData << 8; //Pad with 8 bits because we have to add in the check value

    remainder |= checkSum; //Add on the check value

    uint32_t divsor = (uint32_t)SHIFTED_DIVISOR;

    for (int i = 0 ; i < 16 ; i++) //Operate on only 16 positions of max 24. The remaining 8 are our remainder and should be zero when we're done.
    {
        //Serial.print("remainder: ");
        //Serial.println(remainder, BIN);
        //Serial.print("divsor:    ");
        //Serial.println(divsor, BIN);
        //Serial.println();
        if( remainder & (uint32_t)1<<(23 - i) ) //Check if there is a one in the left position
          remainder ^= divsor;
        divsor >>= 1; //Rotate the divsor max 16 times so that we have 8 bits left of a remainder

    }
    return (uint8_t)remainder;
}

static ret_code_t get_htu21d_temperature(sems_htu21d_sensor_config_t  const * p_config, app_twi_transfer_t *p_write_transfer, app_twi_transfer_t *p_read_transfer, float *data)
{
    uint8_t *data_buffer = p_write_transfer->p_data;
    data_buffer[0] = TEMPERATURE_NO_HOLD_MASTER_ADDRESS;
    ret_code_t err_code = app_twi_perform(p_config->p_app_twi, p_write_transfer,1, NULL);

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    nrf_delay_ms(HTU21D_READ_GAP);
    err_code = app_twi_perform(p_config->p_app_twi, p_read_transfer,1, NULL);  
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    uint16_t rawTemperature  = ((unsigned int) data_buffer[0] << 8) | (unsigned int) data_buffer[1];
    uint8_t checkSum = data_buffer[2];
    if (checkCRC(rawTemperature , checkSum) == 0)
    {
        rawTemperature  &= 0xFFFC;
        float tempTemperature  = rawTemperature  * (175.72 / 65536.0); //2^16 = 65536
        *data  = tempTemperature  - 46.85; //From page 14
        return NRF_SUCCESS;
    } else 
    {
        return NRF_ERROR_INVALID_DATA;
    }
}

static ret_code_t get_htu21d_humidity(sems_htu21d_sensor_config_t const *p_config, app_twi_transfer_t *p_write_transfer, app_twi_transfer_t *p_read_transfer, float *data)
{
    uint8_t *data_buffer = p_write_transfer->p_data;
    data_buffer[0] = HUMIDITY_NO_HOLD_MASTER_ADDRESS;
    ret_code_t err_code = app_twi_perform(p_config->p_app_twi, p_write_transfer,1, NULL);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    nrf_delay_ms(HTU21D_READ_GAP);
    err_code = app_twi_perform(p_config->p_app_twi, p_read_transfer,1, NULL);  
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    uint16_t rawHumidity  = ((unsigned int) data_buffer[0] << 8) | (unsigned int) data_buffer[1];
    uint8_t checkSum = data_buffer[2];
    if (checkCRC(rawHumidity, checkSum) == 0)
    {
        rawHumidity &= 0xFFFC;
        float tempRH = rawHumidity * (125.0 / 65536.0); //2^16 = 65536
        *data  = tempRH - 6.0; //From page 14
        return NRF_SUCCESS;
    } else 
    {
        return NRF_ERROR_INVALID_DATA;
    }  
}


static uint8_t buffer[3];
static app_twi_transfer_t write_transfer = APP_TWI_WRITE(HTU21D_ADDRESS, buffer, 1, APP_TWI_NO_STOP);
static app_twi_transfer_t read_transfer = APP_TWI_READ(HTU21D_ADDRESS, buffer, 3, APP_TWI_NO_STOP);

static ret_code_t get_data(sems_sensor_t const *p_sensor, sems_htu21d_data_t  *p_data)
{    
    sems_htu21d_sensor_config_t const *p_config = p_sensor->p_sensor_config;

    ret_code_t err_code = get_htu21d_temperature(p_config, &write_transfer, &read_transfer, &(p_data->tempeature));
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    err_code = get_htu21d_humidity(p_config, &write_transfer, &read_transfer, &(p_data->humidity));
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return err_code;
}

static ret_code_t get_sensor_data(sems_sensor_t const *p_sensor)
{
    if (p_sensor != p_singleton_sensor)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    return get_data(p_sensor, p_sensor->p_sensor_data);
}

static ret_code_t sems_htu21d_sensor_uninit(sems_sensor_t const  *p_sensor)
{
    if (p_sensor != p_singleton_sensor)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    
    sems_htu21d_sensor_config_t *p_config = p_singleton_sensor->p_sensor_config;
    free(p_config);
    p_singleton_sensor->p_sensor_config = NULL;
    
    free(p_singleton_sensor->p_sensor_data);
    p_singleton_sensor->p_sensor_data = NULL;
    
    free(p_singleton_sensor);
    p_singleton_sensor = NULL;
    p_sensor = NULL;
    
    return NRF_SUCCESS;
}

static ret_code_t sems_htu21d_sensor_init(sems_sensor_t const  *p_sensor)
{
    if (p_sensor != p_singleton_sensor)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    return NRF_SUCCESS;
}

sems_sensor_t* get_sems_htu21d_sensor()
{
    if (p_singleton_sensor != NULL)
    {
        return p_singleton_sensor;
    }
    
    sems_htu21d_data_t *p_data = (sems_htu21d_data_t*)malloc(sizeof(sems_htu21d_data_t));
    sems_htu21d_sensor_config_t *p_config = (sems_htu21d_sensor_config_t*)malloc(sizeof(sems_htu21d_sensor_config_t));
    p_singleton_sensor = (sems_sensor_t*)malloc(sizeof(sems_sensor_t));
    
    if (p_singleton_sensor == NULL || p_data == NULL || p_config == NULL)
    {
        free(p_singleton_sensor);
        free(p_data);
        free(p_config);
        p_singleton_sensor = NULL;
        p_data = NULL;
        p_config = NULL;
        return NULL;
    }
    
    HTU21D_CONFIG_SETUP(p_config);
    HTU21D_DATA_EMPTY_SETUP(p_data);
    
    SEMS_SENSOR_SETUP_WITH_OUT_TIMER(p_singleton_sensor,SEMS_HUMIDTY_TEMPERATURE_TAG, p_config, p_data, sizeof(sems_htu21d_data_t), get_sensor_data, sems_htu21d_sensor_init, sems_htu21d_sensor_uninit, NULL);
    return p_singleton_sensor;
}



#endif

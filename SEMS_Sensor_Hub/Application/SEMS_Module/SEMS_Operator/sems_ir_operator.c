#include "sems_ir_operator.h"
#include "app_error.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf_soc.h"
#include "nrf_nvic.h"


static uint32_t *m_current_data_ptr;
static uint32_t m_bits_remaining;
static uint32_t m_integrated_time_us;
static uint32_t m_next_cc_for_update;
static bool     m_busy;

static uint32_t raw_buffer[SEMS_IR_BUFFER_LEN];

static uint32_t pulse_count_calculate(uint32_t time_us)
{
    return (time_us + (SEMS_IR_CARRIER_LOW_US + SEMS_IR_CARRIER_HIGH_US) / 2) / (SEMS_IR_CARRIER_LOW_US + SEMS_IR_CARRIER_HIGH_US);
}

static ret_code_t sems_ir_init(sems_operator_t const *p_operator)
{
    nrf_drv_gpiote_pin_t *p_ir_pin = (nrf_drv_gpiote_pin_t*)p_operator->p_operator_config;
    nrf_drv_gpiote_pin_t ir_pin = *p_ir_pin;
    ret_code_t err_code = 0;
    
    nrf_drv_gpiote_out_config_t config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);
    err_code = nrf_drv_gpiote_out_init(ir_pin, &config);
    uint32_t gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(ir_pin);
    nrf_drv_gpiote_out_task_enable(ir_pin);
    
    // Carrier timer init
    SEMS_IR_TIMER_CARRIER->MODE          = TIMER_MODE_MODE_Timer;
    SEMS_IR_TIMER_CARRIER->BITMODE       = TIMER_BITMODE_BITMODE_16Bit;
    SEMS_IR_TIMER_CARRIER->PRESCALER     = 4;
    SEMS_IR_TIMER_CARRIER->CC[0]         = SEMS_IR_CARRIER_LOW_US;    
    SEMS_IR_TIMER_CARRIER->CC[1]         = SEMS_IR_CARRIER_LOW_US + SEMS_IR_CARRIER_HIGH_US; 
    SEMS_IR_TIMER_CARRIER->SHORTS        = TIMER_SHORTS_COMPARE1_CLEAR_Msk;

    // Modulation timer init
    SEMS_IR_CARRIER_COUNTER->MODE        = TIMER_MODE_MODE_Counter;  
    SEMS_IR_CARRIER_COUNTER->BITMODE     = TIMER_BITMODE_BITMODE_16Bit;
    SEMS_IR_CARRIER_COUNTER->INTENSET    = TIMER_INTENSET_COMPARE0_Msk | TIMER_INTENSET_COMPARE1_Msk;
    SEMS_IR_CARRIER_COUNTER->EVENTS_COMPARE[0] = 0;
    SEMS_IR_CARRIER_COUNTER->EVENTS_COMPARE[1] = 0;

    err_code |= sd_nvic_SetPriority(SEMS_IR_CARRIER_COUNTER_IRQn, SEMS_IR_CARRIER_COUNTER_IRQ_Priority);
    err_code |= sd_nvic_EnableIRQ(SEMS_IR_CARRIER_COUNTER_IRQn);

    err_code |= sd_ppi_channel_assign(SEMS_IR_PPI_CH_A, &SEMS_IR_TIMER_CARRIER->EVENTS_COMPARE[1], &SEMS_IR_CARRIER_COUNTER->TASKS_COUNT);
    
    err_code |= sd_ppi_channel_assign(SEMS_IR_PPI_CH_B, &SEMS_IR_TIMER_CARRIER->EVENTS_COMPARE[0],  (uint32_t volatile*)gpiote_task_addr);
    err_code |= sd_ppi_channel_assign(SEMS_IR_PPI_CH_C, &SEMS_IR_TIMER_CARRIER->EVENTS_COMPARE[1],  (uint32_t volatile*)gpiote_task_addr);


    err_code |= sd_ppi_group_assign(SEMS_IR_PPI_GROUP, 1 << SEMS_IR_PPI_CH_B | 1 << SEMS_IR_PPI_CH_C);
    err_code |= sd_ppi_group_task_disable(SEMS_IR_PPI_GROUP);
    
    err_code |= sd_ppi_channel_assign(SEMS_IR_PPI_CH_D, &SEMS_IR_CARRIER_COUNTER->EVENTS_COMPARE[0], &NRF_PPI->TASKS_CHG[SEMS_IR_PPI_GROUP].DIS);
    err_code |= sd_ppi_channel_assign(SEMS_IR_PPI_CH_E, &SEMS_IR_CARRIER_COUNTER->EVENTS_COMPARE[1], &NRF_PPI->TASKS_CHG[SEMS_IR_PPI_GROUP].EN);

    err_code |= sd_ppi_channel_enable_set(1 << SEMS_IR_PPI_CH_A | 1 << SEMS_IR_PPI_CH_B | 1 << SEMS_IR_PPI_CH_C | 1 << SEMS_IR_PPI_CH_D | 1 << SEMS_IR_PPI_CH_E);
    
    m_busy = false;
    return err_code;
}

static ret_code_t sems_ir_uninit(sems_operator_t const *p_operator)
{
    nrf_drv_gpiote_pin_t ir_pin = (nrf_drv_gpiote_pin_t)&p_operator->p_operator_config;
    ret_code_t err_code = 0;
    err_code |= sd_ppi_channel_enable_clr(1 << SEMS_IR_PPI_CH_A | 1 << SEMS_IR_PPI_CH_B | 1 << SEMS_IR_PPI_CH_C | 1 << SEMS_IR_PPI_CH_D | 1 << SEMS_IR_PPI_CH_E);
    err_code |= sd_ppi_group_task_disable(SEMS_IR_PPI_GROUP);
    err_code |= sd_nvic_DisableIRQ(SEMS_IR_CARRIER_COUNTER_IRQn);
    nrf_drv_gpiote_out_uninit(ir_pin);
    nrf_drv_gpiote_out_task_disable(ir_pin);
    return err_code;
}



void SEMS_IR_CARRIER_COUNTER_IRQHandler(void)
{
    while(SEMS_IR_CARRIER_COUNTER->EVENTS_COMPARE[m_next_cc_for_update])
    {
        SEMS_IR_CARRIER_COUNTER->EVENTS_COMPARE[m_next_cc_for_update] = 0;
        
        m_bits_remaining--;
        if(m_bits_remaining >= 2)
        {            
            m_integrated_time_us += *m_current_data_ptr++;
            SEMS_IR_CARRIER_COUNTER->CC[m_next_cc_for_update] = pulse_count_calculate(m_integrated_time_us); 
        }
        else if(m_bits_remaining == 0)
        {
            SEMS_IR_TIMER_CARRIER->TASKS_STOP = 1;
            m_busy = false;
        }
        m_next_cc_for_update = 1 - m_next_cc_for_update;
    }
}

ret_code_t sems_ir_row_send(uint32_t p_time_us[], uint32_t length)
{
    m_current_data_ptr = p_time_us;
    
    m_integrated_time_us = *m_current_data_ptr++;
    SEMS_IR_CARRIER_COUNTER->CC[0] = pulse_count_calculate(m_integrated_time_us);
    m_integrated_time_us += *m_current_data_ptr++;
    SEMS_IR_CARRIER_COUNTER->CC[1] = pulse_count_calculate(m_integrated_time_us);
    
    m_bits_remaining = length;
    m_next_cc_for_update = 0;

    sd_ppi_group_task_enable(SEMS_IR_PPI_GROUP);
    
    SEMS_IR_CARRIER_COUNTER->TASKS_CLEAR = 1;
    SEMS_IR_CARRIER_COUNTER->TASKS_START = 1;
    
    SEMS_IR_TIMER_CARRIER->TASKS_CLEAR = 1;
    SEMS_IR_TIMER_CARRIER->TASKS_START = 1;
    return NRF_SUCCESS;
}

static ret_code_t sems_ir_execute(sems_operator_t const *p_operator, void *p_data)
{
    if (p_data == NULL)
        return NRF_ERROR_INVALID_PARAM;
    
    sems_ir_operate_data_t *p_operate_data = (sems_ir_operate_data_t*)p_data;
    
    if (p_operate_data->p_data == NULL || p_operate_data->encode_handler == NULL)
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    
    if (m_busy) {
        return NRF_ERROR_BUSY;
    }
    m_busy = true;
    
    //Clean buffer;
    for(uint8_t i = 0; i < SEMS_IR_BUFFER_LEN; i++)
    {
        raw_buffer[i] = 0;
    }
    uint8_t buffer_length;
    ret_code_t err_code = p_operate_data->encode_handler(p_operate_data->p_data, raw_buffer, &buffer_length);
    if (err_code != NRF_SUCCESS)
        return err_code;
    if (buffer_length > 0) 
    {
        return sems_ir_row_send(raw_buffer, buffer_length);
    }
    return NRF_ERROR_INVALID_LENGTH;
    
}


static sems_ir_config ir_config;
static sems_operator_t sems_operator;

sems_operator_t* get_sems_ir_operator(nrf_drv_gpiote_pin_t ir_pin)
{
    ir_config = ir_pin;
    SEMS_OPERATOR_INIT(sems_operator,SEMS_IR_TAG, &ir_config, sems_ir_init, sems_ir_uninit, sems_ir_execute);
    return &sems_operator;
}

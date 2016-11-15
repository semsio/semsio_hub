#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "sems.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "boards.h"
#include "app_timer.h"
#include "fstorage.h"
#include "fds.h"
   
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "nrf_gpio.h"
#include "ble_advertising.h"
   
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "TSL2561.h"
#include "HTU21D.h"
#include "GPIOSensor.h"
#include "sensor_handler.h"
#include "sems_module.h"
#include "sems_actuator.h"
#include "sems_ir_actuator.h"
#include "pb_encode.h"

#include "ble_services.h"
#include "ble_battery_service.h"
#include "ble_action_service.h"
#include "ble_sensor_service.h"
#include "sensor_handler.h"
#include "app_scheduler.h"
#include "nordic_common.h"

#include "sems_ir_raw_encoder.h"
#include "sems_ir_nec_encoder.h"

#include "sems.pb.h"
#include "nrf_delay.h"
#include "test_sensor.h"

#define APP_TIMER_OP_QUEUE_SIZE         8                                           /**< Size of timer operation queues. */
#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define SCHED_MAX_EVENT_DATA_SIZE       sizeof(nrf_drv_gpiote_pin_t)
#define SCHED_QUEUE_SIZE                10

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    // Initialize timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
}

/**@brief Function for the core initialization.
 *
 * @details Initializes the ble module,sensor handler module.
 */
void sems_init(void)
{
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    
    ble_stack_init();
    gap_params_init();

    sems_advertising_init();
    sems_ble_services_init();
    conn_params_init();
    sensor_handler_init();
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

/**@brief Function for the Power manager.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

/**@brief Callback function for sensor event.
 *
 * @details This function will be called in case of a event in the Sensors.
 *
 * @param[in] p_sensor  Sensor which called the event.
 * @param[in] data      Sensor data when event happend.
 * @param[in] err_code  Is sensor read data currently, data is not trusted if err_code is not 0.
 */
static void sensor_event_handler(sems_sensor_t const* p_sensor, void* data, ret_code_t err_code)
{
    sems_gpio_event_data_t *event_data=data;
    sems_ble_advertising_once(p_sensor,&event_data->pin_val,sizeof(*(uint8_t*)data));
}

static sems_actuator_t *m_ir_actuator_prt;

/**@brief Function for sensor initialization.
 */
void sensor_init()
{
    uint32_t err_code;
    
    //init sensor control
    err_code=sems_module_init();
    APP_ERROR_CHECK(err_code);
    
    //create htu21d sensor,add to ble getter,start polling
    sems_sensor_t *p_htu21d = get_sems_htu21d_sensor();
    err_code = sems_sensor_init(p_htu21d);
    APP_ERROR_CHECK(err_code);
    sems_ble_add_sensor(p_htu21d,BLE_UPDATE_TYPE_GATT);
    err_code = sems_sensor_polling(p_htu21d, sems_ble_gatt_sensor_event_handler, 1000);
    APP_ERROR_CHECK(err_code);
    
    //create tsl2561 sensor,add to ble getter,start polling
    sems_sensor_t *p_tsl2561 = get_sems_tsl2561_default_sensor();
    err_code = sems_sensor_init(p_tsl2561);
    APP_ERROR_CHECK(err_code);
    sems_ble_add_sensor(p_tsl2561,BLE_UPDATE_TYPE_GATT);
    err_code = sems_sensor_polling(p_tsl2561, sems_ble_gatt_sensor_event_handler, 1000);
    APP_ERROR_CHECK(err_code);
    
    //create gpio sensor,register event,add to ble advertising once.
    sems_sensor_t *p_gpio20 = get_gpio_sensor(20, NRF_GPIO_PIN_PULLUP);
    err_code = sems_sensor_init(p_gpio20);
    APP_ERROR_CHECK(err_code);
    nrf_gpiote_polarity_t t = NRF_GPIOTE_POLARITY_HITOLO;
    err_code = sems_sensor_set_event_handler(p_gpio20, sensor_event_handler, &t);
    
    m_ir_actuator_prt = get_sems_ir_actuator(24);
    err_code = sems_actuator_init(m_ir_actuator_prt);
    APP_ERROR_CHECK(err_code);

}

/**@brief Callback function for ble action event.
 *
 * @details This function will be called from the BLE Stack when an action has received.
 *
 * @param[in] sems_ActionData  Received action data.
 */
void ble_action_handler(sems_ActionData *action_data)
{
    switch(action_data->id)
    {
      case 0x5000:{
          if(action_data->has_data==true && action_data->data.size > 0){
              sems_raw_data data;
              data.length = action_data->data.size / 2;
              uint16_t data_buffer[SEMS_IR_BUFFER_LEN];
              data.p_raw_data = data_buffer;
              
              for (int i = 0; i < action_data->data.size; i += 2)
                  data.p_raw_data[i/2] = action_data->data.bytes[i+1] | (uint16_t)action_data->data.bytes[i] << 8;
              sems_ir_operate_data_t ir_data;
              ir_data.p_data = &data;
              ir_data.encode_handler = sems_ir_raw_encode;
              ret_code_t err_code = sems_actuator_execute(m_ir_actuator_prt, &ir_data);
              if (err_code != NRF_SUCCESS)
              {
                  NRF_LOG_INFO("IR BUSY \n");
                  NRF_LOG_FLUSH();
              }
          }
          break;
      }
      case 0x5001:{
          if(action_data->has_data==true && action_data->data.size > 0){
              sems_nec_data data={0};
              data.address = action_data->data.bytes[0];
              data.command = action_data->data.bytes[1];
              
              sems_ir_operate_data_t ir_data;
              ir_data.p_data = &data;
              ir_data.encode_handler = sems_ir_nec_encode;
              ret_code_t err_code = sems_actuator_execute(m_ir_actuator_prt, &ir_data);
          }
          break;
      }
    }
}

/**@brief Function for application main entry.
 */
int main(void)
{
    timers_init();
    sems_init();
    
    // Initialize your sensor
    sensor_init();
    // Set your action handler
    sems_set_action_handler(ble_action_handler);
    sems_ble_start();

    
//  Enter main loop.
    for (;;)
    {
        app_sched_execute();
        if (NRF_LOG_PROCESS() == false)
        {
            power_manage();
        }
    }
}


/**
 * @}
 */

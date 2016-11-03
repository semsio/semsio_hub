#include "sensor_handler.h"
#include "ble_sensor_service.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "sems.pb.h"
#include "pb_encode.h"
#include "linkedlist.h"
#include "app_scheduler.h"
#include "nordic_common.h"

#define ADVERTISING_ONCE_QUEUE_SIZE 16
#define ADVERTISING_CLEAR_INTERVAL     APP_TIMER_TICKS(2000, APP_TIMER_PRESCALER)

APP_TIMER_DEF(m_ble_advertising_timer_id);

static list_node *gatt_sensor_list;
static list_node *broadcast_sensor_list;
static sems_SensorData sensor_data_pb;

static advertising_once_t once_queue[ADVERTISING_ONCE_QUEUE_SIZE];
static void sems_ble_update_data(ble_update_type type);
static uint8_t once_queue_index=0;
static uint8_t last_index=0;

static void ble_advertising_timeout_handler(void * p_context)
{
    if(last_index>0 && once_queue_index==0){
        NRF_LOG_INFO("clear called \n");
        NRF_LOG_FLUSH();
        sems_update_advertising_response_data(NULL,0);
    }
    last_index=once_queue_index;
    for(int i=0;i<once_queue_index;i++)
    {
        free(once_queue[i].data);
    }
    once_queue_index=0;
}

bool encode_sensordata(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    list_node *nownode=*arg;
    while(nownode!=NULL){
        if(!pb_encode_tag_for_field(stream,field))
        {
            continue;
        }
        sems_sensor_t *sensor=nownode->data;
        sensor_data_pb.type=sensor->sensor_tag;
        sensor_data_pb.data.size=sensor->data_size;
        memcpy(sensor_data_pb.data.bytes,sensor->p_sensor_data,sensor->data_size);
        if(!pb_encode_submessage(stream, sems_SensorData_fields, &sensor_data_pb))
        {
            continue;
        }
        nownode=nownode->next;
    }
    
    if(nownode == broadcast_sensor_list && once_queue_index>0)
    {
      for(int i=0;i<once_queue_index;i++)
      {
          if(!pb_encode_tag_for_field(stream,field))
          {
              continue;
          }
          sems_sensor_t const* sensor=once_queue[i].sensor;
          sensor_data_pb.type=sensor->sensor_tag;
          sensor_data_pb.data.size=once_queue[i].size;
          memcpy(sensor_data_pb.data.bytes,once_queue[i].data,once_queue[i].size);
          if(!pb_encode_submessage(stream, sems_SensorData_fields, &sensor_data_pb))
          {
              continue;
          }
      }
    }
    return true;
}

static void sems_ble_update_data(ble_update_type type)
{
    sems_SensorDataList sensor_data_list=sems_SensorDataList_init_default;
    uint8_t data_response[BLE_GATT_MAX_SIZE]={0};
    pb_ostream_t stream = pb_ostream_from_buffer(data_response, sizeof(data_response));
    sensor_data_list.list.funcs.encode=&encode_sensordata;
    if(type==BLE_UPDATE_TYPE_GATT)
        sensor_data_list.list.arg=gatt_sensor_list;
    else if(type==BLE_UPDATE_TYPE_ADVERTISING)
        sensor_data_list.list.arg=broadcast_sensor_list;
    
    if(pb_encode(&stream, sems_SensorDataList_fields, &sensor_data_list))
    {
      if(type==BLE_UPDATE_TYPE_GATT){
        if(stream.bytes_written<=BLE_GATT_MAX_SIZE)
          sems_ble_set_sensor_data((uint8_t*)&data_response,stream.bytes_written);
      }
      else if(type==BLE_UPDATE_TYPE_ADVERTISING){
        if(stream.bytes_written>0){
          if(stream.bytes_written<=BLE_BROADCAST_MAX_SIZE){
            sems_update_advertising_response_data((uint8_t*)&data_response,stream.bytes_written);
          }
        }
      }
    }
}

void sems_ble_update_gatt_data()
{
    sems_ble_update_data(BLE_UPDATE_TYPE_GATT);
}

void sems_ble_update_advertising_data()
{
    sems_ble_update_data(BLE_UPDATE_TYPE_ADVERTISING);
}

void sems_ble_gatt_sensor_event_handler()
{
    sems_ble_update_gatt_data();
}

void sems_ble_advertising_sensor_event_handler()
{
    sems_ble_update_advertising_data();
}

void sems_ble_advertising_once(sems_sensor_t const* sensor,void *data,uint16_t size)
{
    bool isupdate=false;
    for(int i=0;i<once_queue_index;i++)
    {
        advertising_once_t advertising_once=once_queue[i];
        if(advertising_once.sensor->sensor_tag==sensor->sensor_tag)
        {
            isupdate=true;
            free(advertising_once.data);
            advertising_once.data=(uint8_t*)malloc(size);
            memcpy(advertising_once.data,data,size);
            advertising_once.size=size;
            break;
        }
    }
    if(!isupdate){
        advertising_once_t advertising_once={0};
        advertising_once.sensor=sensor;
        advertising_once.data=(uint8_t*)malloc(size);
        memcpy(advertising_once.data,data,size);
        advertising_once.size=size;
        once_queue[once_queue_index]=advertising_once;
        once_queue_index++;
        sems_ble_update_advertising_data();
    }
}

void sems_ble_add_sensor(sems_sensor_t *sensor,ble_update_type type)
{
    if(type==BLE_UPDATE_TYPE_GATT)
    {
        if(gatt_sensor_list==NULL)
        {
            gatt_sensor_list=list_create(sensor);
        }else
        {
            list_insert_end(gatt_sensor_list,sensor);
        }
    }else if(type==BLE_UPDATE_TYPE_ADVERTISING){
        if(broadcast_sensor_list==NULL)
        {
            broadcast_sensor_list=list_create(sensor);
        }else
        {
            list_insert_end(broadcast_sensor_list,sensor);
        }
    }
}

int find_node(list_node *node,void *data)
{
    uint16_t sensor_tag=*(uint16_t*)data;
    sems_sensor_t *sensor=node->data;
    if(sensor->sensor_tag==sensor_tag)
      return true;
    else
      return false;
}

void sems_ble_remove_sensor(sems_sensor_t *sensor,ble_update_type type)
{
    if(type==BLE_UPDATE_TYPE_GATT)
    {
      list_remove_by_data(&gatt_sensor_list,sensor);
    }
    else if(type==BLE_UPDATE_TYPE_ADVERTISING)
    {
      list_remove_by_data(&broadcast_sensor_list,sensor);
    }
}

void sems_ble_remove_sensor_tag(uint16_t sensor_tag,ble_update_type type)
{
    if(type==BLE_UPDATE_TYPE_GATT)
    {
      list_node *node = list_find(gatt_sensor_list,&find_node,&sensor_tag);
      if(node!=NULL)
      {
          list_remove(&gatt_sensor_list,node);
      }
    }
    else if(type==BLE_UPDATE_TYPE_ADVERTISING)
    {
      list_node *node = list_find(broadcast_sensor_list,&find_node,&sensor_tag);
      if(node!=NULL)
      {
          list_remove(&broadcast_sensor_list,node);
      }
    }
}

uint32_t sensor_handler_init()
{
    uint32_t err_code;
    err_code = app_timer_create(&m_ble_advertising_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                ble_advertising_timeout_handler);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    err_code = app_timer_start(m_ble_advertising_timer_id, ADVERTISING_CLEAR_INTERVAL, NULL);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    return err_code;
}
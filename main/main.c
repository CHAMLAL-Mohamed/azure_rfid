/**
 * Name:            main.c
 * Description:     Source for entry point to application which 
 *                  send sensor telemetry and heap information to Azure
 *                  from ESP32 device
 * Initial Date:    11.01.2019
**/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "sdkconfig.h"
#include "freertos/stream_buffer.h"

#include "esp_common.h"


#include "rfid_uart.h"
#include "azure_hub.h"

typedef struct 
{
    int (*twin_callback)(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback);
    int (*direct_method_callback)(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback);
    int (*reported_status_callback)(int status_code, void* userContextCallback);
}azure_callbacks_t;



/******************************************************************************
 * FunctionName : board_init
 * Description  : Call functions to initialize ESP32 NVS, TCPIP Adapter/WIFI
 *                and Device Sensors
 * Parameters   : none
 * Returns      : void
*******************************************************************************/



/******************************************************************************
 * FunctionName : app_main
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void app_main()
{
    //Set LOG level for WIFI, UART and AZURE
    //Initialize IOT HUB-->initialize WIFI and connect device then create task to monitor azure events
    //Initialize RFID UART--> and start task to monitor events
    ESP_ERROR_CHECK(hub_init())
    
   
}


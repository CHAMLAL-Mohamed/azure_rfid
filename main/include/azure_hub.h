#ifndef _AZURE_HUB_H_
#define _AZURE_HUB_H_

#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_device_client_ll.h"
#include "iothub_client_options.h"
#include "iothub.h"
#include "iothub_message.h"
#include "parson.h"
#include "sdkconfig.h"
#include "iothubtransportmqtt.h"
#include "iothub_client_ll.h"
#include "certs.h"

#include "esp_log.h"

#include "iothub_client_version.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/shared_util_options.h"

#include "azure_prov_client/prov_transport_mqtt_client.h"


typedef struct 
{
    int (*twin_callback)(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback);
    int (*direct_method_callback)(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback);
    int (*reported_status_callback)(int status_code, void* userContextCallback);
}azure_callbacks_t;

esp_err_t azure_hub_init();

void azure_hub_start(azure_callbacks_t, QueueHandle_t dataQueue);
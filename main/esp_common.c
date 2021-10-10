#include "esp_common.h"

//Declare static prototype
static void  esp32_event_handler(void *ctx, esp_event_base_t event_base,int32_t event_id,void *event_data);

//WIFI settings
static EventGroupHandle_t wifi_event_grp;
static int wifi_retry_num = 0;
// static ip4_addr_t* esp32_ip_addr;


/******************************************************************************
 * FunctionName : esp32_event_handler
 * Description  : Callback to handle base level events from the ESP32 
 *                System Start, WIFI, etc. 
 * Parameters   : *ctx (void) 
 *                *event (system_event_t)
 * Returns      : esp_err_t
*******************************************************************************/
static void esp32_event_handler(void *ctx, esp_event_base_t event_base,
 int32_t event_id,void* event_data)
{
     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (wifi_retry_num < ESP_WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            wifi_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(wifi_event_grp, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        wifi_retry_num = 0;
        xEventGroupSetBits(wifi_event_grp, WIFI_CONNECTED_BIT);
    }
    
}
// static esp_err_t esp32_event_handler(void *ctx, system_event_t *event)
// {
//     switch(event->event_id) 
//     {
//         case SYSTEM_EVENT_STA_START:
//             esp_wifi_connect();
//             break;

//         case SYSTEM_EVENT_STA_GOT_IP:
//             ESP_LOGI(TAG, "got ip:%s",
//                     ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
//             esp32_ip_addr = &event->event_info.got_ip.ip_info.ip;	
//             wifi_retry_num = 0;
//             printf("Connected !!!");
//             xEventGroupSetBits(wifi_event_grp, WIFI_CONNECTED_BIT);
//             break;

//         case SYSTEM_EVENT_STA_DISCONNECTED:
//             ESP_LOGI(TAG,"Event State: Disconnected from WiFi");

//             if (wifi_retry_num < ESP_WIFI_MAXIMUM_RETRY) 
//             {
//                 esp_wifi_connect();
//                 xEventGroupClearBits(wifi_event_grp, WIFI_CONNECTED_BIT);
//                 wifi_retry_num++;
//                 ESP_LOGI(TAG,"Retry connecting to the WiFi AP");
//             }
//             else 
//             {
//                 ESP_LOGI(TAG,"Connection to the WiFi AP failure\n");
//             }
//             break;
            

//         default:
//             break;
//     }
    
//     return ESP_OK;
// }
/******************************************************************************
 * FunctionName : nvs_init
 * Description  : Initialize NVS
 * Parameters   : none
 * Returns      : esp_err_t
*******************************************************************************/
esp_err_t nvs_init()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || 
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    printf("NVS is initialized\n");
    return ESP_OK; 
}

/******************************************************************************
 * FunctionName : wifi_init
 * Description  : Initialize WiFI
 * Parameters   : none
 * Returns      : esp_err_t
*******************************************************************************/
esp_err_t wifi_init()
{
    //Initialize WiFi
    wifi_event_grp = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &esp32_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &esp32_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    //ESP_ERROR_CHECK(esp_event_loop_init(esp32_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	       .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "wifi_init finished.");
    ESP_LOGI(TAG, "Connected to WiFi AP-SSID:%s password:%s",
             ESP_WIFI_SSID, ESP_WIFI_PASS);
    //Added by Me
    // esp_wifi_set_storage(WIFI_STORAGE_RAM);
    EventBits_t bits = xEventGroupWaitBits(wifi_event_grp,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                ESP_WIFI_SSID, ESP_WIFI_PASS);
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
        vEventGroupDelete(wifi_event_grp);
        esp_restart();
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(wifi_event_grp);

    // wifi_config_t wifi_config = {
    //     .sta = {
    //         .ssid = ESP_WIFI_SSID,
    //         .password = ESP_WIFI_PASS
    //     },
    // };   

    //Check for errors

    

    
 
    return ESP_OK;
}
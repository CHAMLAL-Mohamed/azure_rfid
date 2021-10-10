
set(COMPONENT_SRCS "rfid_uart.c" "azure_hub.c" "main.c" 
                   "esp_common.c" 
                   )

list(APPEND COMPONENT_ADD_INCLUDEDIRS "." 
                                      "include")

register_component()
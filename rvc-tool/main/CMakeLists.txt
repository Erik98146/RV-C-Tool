idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_twai esp_timer esp_driver_gpio esp_wifi esp_http_server nvs_flash esp_netif esp_event spiffs
)

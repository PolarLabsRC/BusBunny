#include <string.h>

#include "wifiapdrv.h"

#include "esp_wifi.h"
#include "esp_log.h"

static const char *TAG = "wifiapdrv";

void wifiapdrv_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data)
{
  if (event_id == WIFI_EVENT_AP_STACONNECTED)
  {
    wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
    ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
             MAC2STR(event->mac), event->aid);
  }
  else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
  {
    wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
    ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
             MAC2STR(event->mac), event->aid);
  }
}

void wifiapdrv_create(ssidstr_t ssid, passwdstr_t passwd, uint8_t max_conn)
{
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &wifiapdrv_event_handler,
                                                      NULL,
                                                      NULL));


  wifi_config_t wifi_config_ap = {
      .ap = {
          .max_connection = max_conn,
          .authmode = WIFI_AUTH_WPA_WPA2_PSK,
      },
  };
  strncpy((char *)wifi_config_ap.sta.ssid, ssid.bytes, 32);
  strncpy((char *)wifi_config_ap.sta.password, passwd.bytes, 64);

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_ap));
  ESP_LOGI(TAG, "AccessPoint initialized");

  ESP_ERROR_CHECK(esp_wifi_start());
}
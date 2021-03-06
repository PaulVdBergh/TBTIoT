/*
 * To start debugger:
 * 	$ cd ~/esp/openocd-esp32
 * 	$ bin/openocd -s share/openocd/scripts -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp32-wrover.cfg
 */


#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "esp_log.h"
static char tag[] = "app_main";

#include "DCCGen.h"
//#include "Decoders.h"
#include "Z21Interface.h"

using namespace TBTIoT;

#include <string.h>

extern "C" void task_paho(void *ignore);

extern "C"
esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch(event->event_id)
	{
		case SYSTEM_EVENT_WIFI_READY:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_WIFI_READY");
			break;
		}

		case SYSTEM_EVENT_SCAN_DONE:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_SCAN_DONE");
			break;
		}

		case SYSTEM_EVENT_STA_START:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_START");
			break;
		}

		case SYSTEM_EVENT_STA_STOP:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_STOP");
			break;
		}

		case SYSTEM_EVENT_STA_CONNECTED:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_CONNECTED");
			break;
		}

		case SYSTEM_EVENT_STA_DISCONNECTED:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_DISCONNECTED");
			break;
		}

		case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
			break;
		}

		case SYSTEM_EVENT_STA_GOT_IP:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_GOT_IP");
			xTaskCreatePinnedToCore(&task_paho, "task_paho", 8048, NULL, 5, NULL, 0);
			Z21Interface* pZ21 = new Z21Interface();
			break;
		}

		case SYSTEM_EVENT_STA_LOST_IP:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_LOST_IP");
			break;
		}

		default:
		{
			ESP_LOGI(tag, "EventHandler received %i", event->event_id);
			break;
		}
	}
    return ESP_OK;
}

extern "C"
void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

    wifi_config_t sta_config;
    memcpy(&sta_config.sta.ssid, "devolo-8f4", strlen("devolo-8f4"));
    memcpy(&sta_config.sta.password, "AYBJMEITONIPHEBQ", strlen("AYBJMEITONIPHEBQ"));
    sta_config.sta.bssid_set = false;

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

//    Decoders* pDecoders = Decoders::getInstance();
    DCCGen* DccGenerator = new DCCGen();


//    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    int level = 0;
    while (true) {
//        gpio_set_level(GPIO_NUM_4, level);
        level = !level;
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }

}


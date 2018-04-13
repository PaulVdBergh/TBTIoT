#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"


#include "esp_log.h"
static char tag[] = "TBTIoTApp";

#include <string.h>

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


#include "TBTIoTWifi.h"
using namespace TBTIoT;

extern "C"
void app_main(void)
{
	TBTIoTWifi* pWifi = TBTIoTWifi::getInstance();

	vTaskDelay(10000 / portTICK_PERIOD_MS);
	pWifi->Scan();

    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    int level = 0;
    while (true) {
        gpio_set_level(GPIO_NUM_5, level);
        level = !level;
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}


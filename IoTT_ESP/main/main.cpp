/**
 * Copyright (C) 2018 Paul Van den Bergh <admin@paulvandenbergh.be>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * =====================================================================
 *	This file is part of the IoT&Trains project.  
 *	For more info see http://paulvandenbergh.be
 * =====================================================================
 */

/*
 * main.cpp
 *
 *  Created on: May 10, 2018
 *      Author: paulvdbergh
 */


#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "lwip/sockets.h"

#include "esp_log.h"

#include "DCCGen.h"

#include <string.h>

#include "EventGroup.h"

char RPi_SSID[] = "IoT_and_Trains";
char RPi_SSID_pwd[] = "IoTTESP32";
char RPi_SSID_ip[] = "192.168.4.1";
int RPi_SSID_port = 3003;

/*
#define RPI_SSID "devolo-8f4"
#define RPI_SSID_PWD "AYBJMEITONIPHEBQ"
*/

IoTT::EventGroup WifiEventGroup;

enum
{
	WIFI_RPI_FOUND = 0,
	WIFI_STA_START,
	WIFI_STA_CONNECTED,
	WIFI_STA_DISCONNECTED,
	WIFI_STA_GOT_IP,
	WIFI_STA_LOST_IP
};

esp_err_t event_handler(void *ctx, system_event_t *event)
{
	static char tag[] = "event_handler";
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

			uint16_t nbrAP(0);
			ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&nbrAP));

			if(0 < nbrAP)
			{
				WifiEventGroup.Set(BITPOS(WIFI_RPI_FOUND));
			}
			break;
		}

		case SYSTEM_EVENT_STA_START:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_START");
			WifiEventGroup.Set(BITPOS(WIFI_STA_START));
			break;
		}

		case SYSTEM_EVENT_STA_STOP:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_STOP");
			WifiEventGroup.Clear(BITPOS(WIFI_STA_START) | BITPOS(WIFI_STA_CONNECTED) | BITPOS(WIFI_STA_GOT_IP));
			break;
		}

		case SYSTEM_EVENT_STA_CONNECTED:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_CONNECTED");
			WifiEventGroup.Clear(BITPOS(WIFI_STA_DISCONNECTED));
			WifiEventGroup.Set(BITPOS(WIFI_STA_CONNECTED));
			break;
		}

		case SYSTEM_EVENT_STA_DISCONNECTED:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_DISCONNECTED");
			WifiEventGroup.Clear(BITPOS(WIFI_STA_CONNECTED) | BITPOS(WIFI_STA_GOT_IP));
			WifiEventGroup.Set(BITPOS(WIFI_STA_DISCONNECTED));

		    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 200);
		    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

		    ESP_ERROR_CHECK( esp_wifi_connect() );
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
			WifiEventGroup.Clear(BITPOS(WIFI_STA_LOST_IP));
			WifiEventGroup.Set(BITPOS(WIFI_STA_GOT_IP));

		    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
		    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

			break;
		}

		case SYSTEM_EVENT_STA_LOST_IP:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_LOST_IP");
			WifiEventGroup.Clear(BITPOS(WIFI_STA_GOT_IP));
			WifiEventGroup.Set(BITPOS(WIFI_STA_LOST_IP));
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

extern "C" void task_paho(void *ignore);

extern "C"
void app_main(void)
{
	//------------------------------
	//	LEDC

	ledc_timer_config_t ledc_timer;
	ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
	ledc_timer.freq_hz = 4;
	ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_timer.timer_num = LEDC_TIMER_0;

	ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel;
	ledc_channel.channel = LEDC_CHANNEL_0;
	ledc_channel.duty = 1023;
	ledc_channel.gpio_num = 4;
	ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_channel.timer_sel = LEDC_TIMER_0;

	ledc_channel_config(&ledc_channel);

	ledc_fade_func_install(0);

	//	LEDC
	//------------------------------

	static char tag[] = "app_main";
	EventBits_t status;
	nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_start());

    wifi_scan_config_t scan_config;
    memset(&scan_config, 0, sizeof(scan_config));
    scan_config.ssid = (uint8_t*)RPi_SSID;
    scan_config.bssid = nullptr;
    scan_config.channel = 0;
    scan_config.show_hidden = true;

    do
    {
    	ESP_LOGI(tag, "Scanning for Raspberry");
    	ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while(!(BITPOS(WIFI_RPI_FOUND) & WifiEventGroup.Get()));
    ESP_LOGI(tag, "Raspberry found");

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 800);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    ESP_ERROR_CHECK(esp_wifi_stop());
    do
    {
    	vTaskDelay(100 / portTICK_PERIOD_MS);
    	status = WifiEventGroup.Get();
    	ESP_LOGI(tag, "WifiEventGroup status = 0x%04x", status);
    }
    while(status & BITPOS(WIFI_STA_START));

    wifi_config_t sta_config;
    memset(&sta_config, 0, sizeof(sta_config));
    memcpy(&sta_config.sta.ssid, RPi_SSID, strlen(RPi_SSID));
    memcpy(&sta_config.sta.password, RPi_SSID_pwd, strlen(RPi_SSID_pwd));
    sta_config.sta.bssid_set = false;

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    WifiEventGroup.Wait(BITPOS(WIFI_STA_START));
    ESP_ERROR_CHECK( esp_wifi_connect() );
    WifiEventGroup.Wait(BITPOS(WIFI_STA_GOT_IP));

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 800);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    int sockfd;
    int connStatus;

    ESP_LOGI(tag, "preparing address");
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(RPi_SSID_port);
    if(inet_pton(AF_INET, RPi_SSID_ip, &serv_addr.sin_addr) <= 0)
    {
    	ESP_LOGE(tag, "Invalid address.");
    	return;
    }

    do
    {
    	ESP_LOGI(tag, "Creating socket");
    	sockfd = lwip_socket(AF_INET, SOCK_STREAM, 0);
    	ESP_LOGI(tag, "sockfd = %i", sockfd);
    	if(0 > sockfd)
    	{
    		return;
    	}

    	int optVal = 1;
		int result = lwip_setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
		ESP_LOGI(tag, "setsockopt(SO_REUSEADDR) returned %i", result);

		result = lwip_setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optVal, sizeof(optVal));
		ESP_LOGI(tag, "setsockopt(SO_REUSEPORT) returned %i", result);

    	ESP_LOGI(tag, "Connecting");
    	connStatus = lwip_connect(sockfd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr));
    	if(0 > connStatus)
    	{
    		perror("connect : ");
    		ESP_LOGE(tag, "Connection failed, retry...");
    		lwip_close_r(sockfd);
    		vTaskDelay(1000 / portTICK_PERIOD_MS);
    	}
    }
    while(0 > connStatus);

    ESP_LOGI(tag, "Sending");
    lwip_send(sockfd, "ESP32", strlen("ESP32"), 0);

    char buffer[1024];

    ESP_LOGI(tag, "Reading");
    int nbrRead = lwip_read(sockfd, buffer, 1024);
    buffer[nbrRead] = 0x00;

    ESP_LOGI(tag, "received %i bytes: %s\nClosing socket", nbrRead, buffer);

    lwip_close_r(sockfd);

    char* SSID = buffer;
    char* PWD  = strchr(buffer, ';');
    *PWD++ = 0x00;
    char* BrokerIP = strchr(PWD, ';');
    *BrokerIP++ = 0x00;

    ESP_LOGI(tag, "SSID = %s", SSID);
    ESP_LOGI(tag, "PWD = %s", PWD);
    ESP_LOGI(tag, "BorkerIP = %s", BrokerIP);

    ESP_LOGI(tag, "Disconnecting from RPi");
    ESP_ERROR_CHECK(esp_wifi_disconnect());

    do
    {
    	vTaskDelay(100 / portTICK_PERIOD_MS);
    	status = WifiEventGroup.Get();
    	ESP_LOGI(tag, "WifiEventGroup status = 0x%04x", status);
    }
    while(status & BITPOS(WIFI_STA_CONNECTED));

    ESP_LOGI(tag, "Stopping Wifi");
    ESP_ERROR_CHECK(esp_wifi_stop());
    do
    {
    	vTaskDelay(100 / portTICK_PERIOD_MS);
    	status = WifiEventGroup.Get();
    	ESP_LOGI(tag, "WifiEventGroup status = 0x%04x", status);
    }
    while(status & BITPOS(WIFI_STA_START));

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 200);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    ESP_LOGI(tag, "Connecting to %s", SSID);
    memset(&sta_config, 0, sizeof(sta_config));
    memcpy(&sta_config.sta.ssid, SSID, strlen(SSID));
    memcpy(&sta_config.sta.password, PWD, strlen(PWD));
    sta_config.sta.bssid_set = false;

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    WifiEventGroup.Wait(BITPOS(WIFI_STA_START));
    ESP_ERROR_CHECK( esp_wifi_connect() );
    WifiEventGroup.Wait(BITPOS(WIFI_STA_GOT_IP));

    ESP_LOGI(tag, "Connected to %s !!", SSID);

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    ESP_LOGI(tag, "Starting PAHO...");
	xTaskCreatePinnedToCore(&task_paho, "task_paho", 8048, NULL, 5, NULL, 0);

	ESP_LOGI(tag, "Starting DccGenerator...");
	TBTIoT::DCCGen* DccGenerator = new TBTIoT::DCCGen();

	//    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    int level = 0;
    while (true) {
//        gpio_set_level(GPIO_NUM_4, level);
        level = !level;
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }

    delete DccGenerator;
}




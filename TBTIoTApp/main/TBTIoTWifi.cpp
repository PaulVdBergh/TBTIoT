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
 *	This file is part of the TBTIoT project.  
 *	For more info see http://paulvandenbergh.be
 * =====================================================================
 */

/*
 * TBTIoTWifi.cpp
 *
 *  Created on: Apr 4, 2018
 *      Author: paulvdbergh
 */

#include "TBTIoTWifi.h"

#include <nvs_flash.h>

#include "esp_log.h"
static char tag[] = "TBTIoTWifi";

#include <string.h>

esp_err_t TBTIoTWifi_event_handler(void* ctx, system_event_t* event)
{
	switch(event->event_id)
	{
/*
	case SYSTEM_EVENT_WIFI_READY:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_WIFI_READY");
			break;
		}
*/
		case SYSTEM_EVENT_SCAN_DONE:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_SCAN_DONE");
			break;
		}

		case SYSTEM_EVENT_STA_START:
		{
			ESP_LOGI(tag, "SYSTEM_EVENT_STA_START");
		    ESP_ERROR_CHECK( esp_wifi_connect() );
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

			if(TBTIoT::TBTIoTWifi::getInstance()->getAutoConnect())
			{
				ESP_ERROR_CHECK( esp_wifi_connect() );
			}
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

namespace TBTIoT
{
	TBTIoTWifi* TBTIoTWifi::getInstance()
	{
		if(sm_pInstance)
		{
			return sm_pInstance;
		}
		return sm_pInstance = new TBTIoTWifi();
	}

	TBTIoTWifi::TBTIoTWifi()
	:	m_bAutoConnect(true)
	{
//	    static wifi_config_t m_sta_config;

		ESP_ERROR_CHECK(nvs_flash_init());
		tcpip_adapter_init();
		ESP_ERROR_CHECK(esp_event_loop_init(TBTIoTWifi_event_handler, NULL));
	    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
/*
	    memcpy(&m_sta_config.sta.ssid, "devolo-8f4", strlen("devolo-8f4"));
	    memcpy(&m_sta_config.sta.password, "AYBJMEITONIPHEBQ", strlen("AYBJMEITONIPHEBQ"));
	    m_sta_config.sta.bssid_set = false;

	    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &m_sta_config) );
	    ESP_ERROR_CHECK( esp_wifi_start() );
*/
	    Connect("devolo-8f4", "AYBJMEITONIPHEBQ");
//	    Connect("devolo-8f4", "");
//	    Connect("polder", "polder30");
	}

	TBTIoTWifi::~TBTIoTWifi()
	{
		ESP_ERROR_CHECK(esp_wifi_disconnect());
		ESP_ERROR_CHECK(esp_wifi_stop());
		ESP_ERROR_CHECK(esp_wifi_deinit());

		ESP_ERROR_CHECK(nvs_flash_deinit());
	}

	void TBTIoTWifi::Connect(const string ssid, const string password)
	{
		static wifi_config_t sta_config;

		bool prevAutoConnect = m_bAutoConnect;
		m_bAutoConnect = false;
		esp_wifi_disconnect();
		esp_wifi_stop();

		memcpy(&sta_config.sta.ssid, ssid.c_str(), ssid.length() < 32 ? ssid.length() : 32);
		memcpy(&sta_config.sta.password, password.c_str(), password.length() < 64 ? password.length() : 64);
		sta_config.sta.bssid_set = false;

	    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
	    ESP_ERROR_CHECK(esp_wifi_start());

	    m_bAutoConnect = prevAutoConnect;
	}

#define MAX_APs 32

	// From auth_mode code to string
	static char* getAuthModeName(wifi_auth_mode_t auth_mode) {

		char *names[] = {"OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "WPA2 ENTERPRISE", "MAX"};
		return names[auth_mode];
	}

	void TBTIoTWifi::Scan()
	{
		static wifi_scan_config_t scan_config;
		scan_config.ssid = 0;
		scan_config.bssid = 0;
		scan_config.channel = 0;
		scan_config.show_hidden = true;

		ESP_LOGI(tag, "Start scanning");

		ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

		uint16_t ap_num = MAX_APs;
		wifi_ap_record_t ap_records[MAX_APs];
		ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));

		printf("Found %d access points:\n", ap_num);
		for(int i = 0; i < ap_num; i++)
			printf("%32s | %7d | %4d | %16s\n",
			(char *)ap_records[i].ssid, ap_records[i].primary, ap_records[i].rssi,
			getAuthModeName(ap_records[i].authmode));
	}

	TBTIoTWifi*	TBTIoTWifi::sm_pInstance = nullptr;

} /* namespace TBTIoT */

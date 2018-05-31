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

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include <inttypes.h>

#include "MQTTClient.h"
#include "sdkconfig.h"

static char tag[] = "mqtt_paho";
static unsigned char sendBuf[1000];
static unsigned char readBuf[1000];
Network network;

MQTTClient client;

#include "IoTMQTTMessageQueueItem.h"


void task_paho(void *ignore) {
	ESP_LOGI(tag, "Starting ...");
	int rc;
	NetworkInit(&network);

	char szStatusTopic[256];

	union
	{
		uint64_t	llmac;
		uint8_t		mac[6];
	} mac_t;

	ESP_ERROR_CHECK(esp_efuse_mac_get_default(mac_t.mac));
	char szMac[21];
	snprintf(szMac, sizeof(szMac), "%" PRIu64, mac_t.llmac);

	MQTTString clientId = MQTTString_initializer;
	clientId.cstring = szMac;

	snprintf(szStatusTopic, sizeof(szStatusTopic), "IoTT/Devices/%s/Status", szMac);

	MQTTString willTopic = MQTTString_initializer;
	willTopic.cstring = szStatusTopic;

	MQTTString willMessage = MQTTString_initializer;
	willMessage.cstring = "Offline";

	MQTTPacket_willOptions willOptions = MQTTPacket_willOptions_initializer;
	willOptions.topicName = willTopic;
	willOptions.message = willMessage;
	willOptions.retained = true;
	willOptions.qos = QOS0;

	do
	{
		do
		{
			ESP_LOGI(tag, "::NetworkConnect  ...");
			rc = NetworkConnect(&network, "192.168.1.100", 1883);
		} while(rc);

		ESP_LOGI(tag, "::MQTTClientInit  ...");
		MQTTClientInit(&client, &network,
			1000,            // command_timeout_ms
			sendBuf,         //sendbuf,
			sizeof(sendBuf), //sendbuf_size,
			readBuf,         //readbuf,
			sizeof(readBuf)  //readbuf_size
		);

		MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
		data.clientID          = clientId;
		data.willFlag          = 1;
		data.MQTTVersion       = 4;
		data.keepAliveInterval = 0;
		data.cleansession      = 1;
		data.will			   = willOptions;

		ESP_LOGI(tag, "::MQTTConnect  ...");
		rc = MQTTConnect(&client, &data);
		if (rc != SUCCESS)
		{
			ESP_LOGE(tag, "MQTTConnect returned: %d", rc);
	        vTaskDelay(300 / portTICK_PERIOD_MS);
	        MQTTDisconnect(&client);
	        NetworkDisconnect(&network);
		}
	} while(rc != SUCCESS);

	MQTTMessage statusMessage;
	statusMessage.qos = QOS0;
	statusMessage.retained = true;
	statusMessage.dup = 0;
	statusMessage.id = 0;
	statusMessage.payload = "Online";
	statusMessage.payloadlen = strlen("Online");

	ESP_LOGI(tag, "::MQTTPublish ...");
	rc = MQTTPublish(&client, szStatusTopic, &statusMessage);
	if (rc != SUCCESS)
	{
		ESP_LOGE(tag, "::MQTTPublish: %d", rc);
	}

	ESP_LOGI(tag, "::MQTTSubscribe  ...");
	rc = MQTTSubscribe(&client, "#", QOS0, IoTMQTTMessageHandler_func);
	if (rc != SUCCESS)
	{
		ESP_LOGE(tag, "::MQTTSubscribe: %d", rc);
	}

	while(1)
	{
		rc = MQTTYield(&client, 1000);
		ESP_LOGI(tag, "MQTTYield returned %d", rc);
	}
	vTaskDelete(NULL);
}

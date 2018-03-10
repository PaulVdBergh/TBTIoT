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
	ESP_LOGI(tag, "::NetworkConnect  ...");
	NetworkConnect(&network, "192.168.1.105", 1883);
	ESP_LOGI(tag, "::MQTTClientInit  ...");
	MQTTClientInit(&client, &network,
		1000,            // command_timeout_ms
		sendBuf,         //sendbuf,
		sizeof(sendBuf), //sendbuf_size,
		readBuf,         //readbuf,
		sizeof(readBuf)  //readbuf_size
	);

  MQTTString clientId = MQTTString_initializer;
  clientId.cstring = "MYCLIENT1";

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.clientID          = clientId;
	data.willFlag          = 0;
	data.MQTTVersion       = 4;
	data.keepAliveInterval = 0;
	data.cleansession      = 1;

	ESP_LOGI(tag, "::MQTTConnect  ...");
	rc = MQTTConnect(&client, &data);
	if (rc != SUCCESS) {
		ESP_LOGE(tag, "MQTTConnect: %d", rc);
	}

	ESP_LOGI(tag, "::MQTTSubscribe  ...");
	rc = MQTTSubscribe(&client, "#", QOS0, IoTMQTTMessageHandler_func);
	if (rc != SUCCESS) {
		ESP_LOGE(tag, "::MQTTSubscribe: %d", rc);
	}

	while(1) {
		MQTTYield(&client, 1000);
	}
	vTaskDelete(NULL);
}

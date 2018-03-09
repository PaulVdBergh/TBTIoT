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
	data.MQTTVersion       = 3;
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

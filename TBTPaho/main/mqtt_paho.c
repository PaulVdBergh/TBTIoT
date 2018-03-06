#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include <inttypes.h>
#include <stdio.h>

#include "MQTTClient.h"
#include "sdkconfig.h"

static char tag[] = "mqtt_paho";
static unsigned char sendBuf[1000];
static unsigned char readBuf[1000];
Network network;

static void messageHandler_func(MessageData *md)
{
	ESP_LOGI(tag, "Subscription received!: %.*s", md->topicName->lenstring.len, md->topicName->lenstring.data);
	ESP_LOGI(tag, "\tpayload : %.*s", md->message->payloadlen, (char*)md->message->payload);
}

void task_paho(void *ignore)
{
	ESP_LOGI(tag, "Starting ...");
	int rc;
	MQTTClient client;
	NetworkInit(&network);
	ESP_LOGI(tag, "NetworkConnect  ...");
	NetworkConnect(&network, "192.168.1.105", 1883);
	ESP_LOGI(tag, "MQTTClientInit  ...");
	MQTTClientInit(&client, &network,
		1000,            // command_timeout_ms
		sendBuf,         //sendbuf,
		sizeof(sendBuf), //sendbuf_size,
		readBuf,         //readbuf,
		sizeof(readBuf)  //readbuf_size
	);

	union
	{
		uint64_t	llmac;
		uint8_t 	mac[6];
	} mac_t;
	ESP_ERROR_CHECK(esp_efuse_mac_get_default(mac_t.mac));
	char szMac[21];
	snprintf(szMac, sizeof(szMac), "%"PRIu64, mac_t.llmac);

	MQTTString clientId = MQTTString_initializer;
//	clientId.cstring = "MYCLIENT1";
	clientId.cstring = szMac;

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.clientID          = clientId;
	data.willFlag          = 0;
	data.MQTTVersion       = 4;
	data.keepAliveInterval = 0;
	data.cleansession      = 1;

	ESP_LOGI(tag, "MQTTConnect  ...");
	rc = MQTTConnect(&client, &data);
	if (rc != SUCCESS)
	{
		ESP_LOGE(tag, "MQTTConnect: %d", rc);
	}

	ESP_LOGI(tag, "MQTTSubscribe  ...");
	rc = MQTTSubscribe(&client, "#", QOS0, messageHandler_func);
	if (rc != SUCCESS)
	{
		ESP_LOGE(tag, "MQTTSubscribe: %d", rc);
	}
	while(1)
	{
		MQTTYield(&client, 1000);
	}
	vTaskDelete(NULL);
}

/*
 * IoTMQTTMessageQueueItem.cpp
 *
 *  Created on: Mar 8, 2018
 *      Author: paulvdbergh
 */

#include "IoTMQTTMessageQueueItem.h"

#include <esp_log.h>
#include "string.h"

QueueHandle_t IoTMQTTMessageQueue(xQueueCreate(5, sizeof(IoTMQTTMessageQueueItem)));

void IoTMQTTMessageHandler_func(MessageData* md)
{
	static char tag[] = "IoTMQTTMessageHandler_func";
	ESP_LOGI(tag, "Subscription received!: %.*s", md->topicName->lenstring.len, md->topicName->lenstring.data);

	IoTMQTTMessageQueueItem Item;

	memcpy(Item.m_Topic, md->topicName->lenstring.data, md->topicName->lenstring.len);
	Item.m_Topic[md->topicName->lenstring.len] = 0x00;
	Item.m_QoS = md->message->qos;
	Item.m_retained = md->message->retained;
	Item.m_dup = md->message->dup;
	Item.m_id = md->message->id;
	memcpy(Item.m_payload, md->message->payload, md->message->payloadlen);
	Item.m_payloadlen = md->message->payloadlen;

	xQueueReset(IoTMQTTMessageQueue);

	if(pdPASS == xQueueSendToBack(IoTMQTTMessageQueue, &Item, 0))
	{
		ESP_LOGI(tag, "Item queued");
	}
	else
	{
		ESP_LOGI(tag, "Item failed to queue");
	}
}

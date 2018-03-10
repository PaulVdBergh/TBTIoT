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

	if(pdPASS == xQueueSendToBack(IoTMQTTMessageQueue, &Item, 0))
	{
		ESP_LOGI(tag, "Item queued");
	}
	else
	{
		ESP_LOGE(tag, "Item failed to queue");
	}
}

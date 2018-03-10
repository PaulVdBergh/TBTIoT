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
 * IoTMQTTMessageQueueItem.h
 *
 *  Created on: Mar 8, 2018
 *      Author: paulvdbergh
 */

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include "../MQTTClient-C/src/linux/MQTTClient.h"

#ifndef MAIN_IOTMQTTMESSAGEQUEUEITEM_H_
#define MAIN_IOTMQTTMESSAGEQUEUEITEM_H_

#ifndef MAX_TOPIC_SIZE
#define MAX_TOPIC_SIZE 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern QueueHandle_t IoTMQTTMessageQueue;
extern SemaphoreHandle_t IoRMQTTMessageQueueMutex;
extern void IoTMQTTMessageHandler_func(MessageData* md);

struct IoTMQTTMessageQueueItem
{
	char	m_Topic[MAX_TOPIC_SIZE];
	enum QoS		m_QoS;
	unsigned char m_retained;
	unsigned char m_dup;
	unsigned short m_id;
	uint8_t	m_payload[256];
	size_t m_payloadlen;
};

#ifdef __cplusplus
}
#endif

#endif /* MAIN_IOTMQTTMESSAGEQUEUEITEM_H_ */

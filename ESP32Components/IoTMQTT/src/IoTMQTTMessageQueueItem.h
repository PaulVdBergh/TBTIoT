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

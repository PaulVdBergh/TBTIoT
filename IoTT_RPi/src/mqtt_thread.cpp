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
 * mqtt_thread.cpp
 *
 *  Created on: May 16, 2018
 *      Author: paulvdbergh
 */

#include "mqtt_thread.h"

#include "mqttSettings.h"
#include "mqttMessageQueueItem.h"

#include <cstdlib>
#include <cstring>
#include <thread>
#include <chrono>
#include <mqtt/MQTTAsync.h>

#include <mqueue.h>
#include <sys/stat.h>

static int finished = 0;
static mqd_t mqttSubMQHandle;
static mqd_t mqttPubMQHandle;

/**
 *
 * @param context
 * @param response
 */
static void onSubscribe(void* context, MQTTAsync_successData* response)
{
	printf("Subscribe succeeded\n");
}

/**
 *
 * @param context
 * @param response
 */
static void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	printf("Subscribe failed, rc = %d ['%s']\n", response ? response->code : 0, response ? response->message : "--");
}

/**
 *
 * @param context
 * @param response
 */
static void onConnect(void* context, MQTTAsync_successData* response)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	int rc;

	printf("Successful connection.\n");

	printf("Subscribing to topic '%s'\n\tfor client '%s'\n\tusing Qos%d\n", IoTT::mqttTopic.c_str(), IoTT::mqttClientID.c_str(), IoTT::mqttQOS);

	opts.onSuccess = onSubscribe;
	opts.onFailure = onSubscribeFailure;
	opts.context = client;

	if(MQTTASYNC_SUCCESS != (rc = MQTTAsync_subscribe(client, IoTT::mqttTopic.c_str(), IoTT::mqttQOS, &opts)))
	{
		printf("Failed to start subscribe, return code = %d\n", rc);
	}
}

/**
 *
 * @param context
 * @param response
 */
static void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
	printf("Connect failed, rc = %d ['%s']\n", response ? response->code : 0, response ? response->message : "--");
	finished = 1;
}

/**
 *
 * @param context
 * @param cause
 */
static void connlost(void* context, char* cause)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	int rc;

	printf("Connection lost\n\tcause = %s", cause);

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	if(MQTTASYNC_SUCCESS != (rc = MQTTAsync_connect(client, &conn_opts)))
	{
		printf("Failed to start connect, return code = %d\n", rc);
		finished = 1;
	}
}

/**
 *
 * @param context
 * @param topicName
 * @param topicLen
 * @param message
 * @return
 */
static int msgArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message)
{
	int i;
	char* payloadptr = (char*)message->payload;

	printf("Message arrived\n\ttopic : %s\n\tmessage : ", topicName);

	for(i = 0; i < message->payloadlen; i++)
	{
		putchar(*payloadptr++);
	}
	putchar('\n');

	IoTT::mqttMessageQueueItem msg(topicName,
			message->qos,
			message->retained,
			message->dup,
			message->msgid,
			(uint8_t*)message->payload,
			message->payloadlen);

	int rc = mq_send(mqttSubMQHandle, (const char*)&msg, sizeof(msg), 0);
	if(0 != rc)
	{
		perror("mq_send failed");
	}

	MQTTAsync_freeMessage(&message);
	MQTTAsync_free(topicName);
	return 1;
}

/**
 * The thread function for MQTT connectivity
 *
 * @param
 */
void* mqtt_thread_func(void*)
{
	MQTTAsync	client;
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	int rc;

	if(-1 == (mqttSubMQHandle = mq_open(IoTT::mqttSubMQName.c_str(), (O_CREAT | O_WRONLY), S_IRWXU, &IoTT::mqttMsgQueueAttribs)))
	{
		printf("Failed to open/create mqttSubMQHandle\n");
		perror("error");
	}

	if(-1 == (mqttPubMQHandle = mq_open(IoTT::mqttPubMQName.c_str(), (O_CREAT | O_RDONLY), S_IRWXU, &IoTT::mqttMsgQueueAttribs)))
	{
		printf("Failed to open/create mqttPubMQHandle\n");
		perror("error");
	}

	rc = MQTTAsync_create(&client,
			IoTT::mqttAddress.c_str(),
			IoTT::mqttClientID.c_str(),
			MQTTCLIENT_PERSISTENCE_NONE,
			nullptr);

	rc = MQTTAsync_setCallbacks(client, client, connlost, msgArrived, nullptr);

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.onSuccess = onConnect;
	conn_opts.onFailure = onConnectFailure;
	conn_opts.context = client;

	if(MQTTASYNC_SUCCESS != (rc = MQTTAsync_connect(client, &conn_opts)))
	{
		printf("Failed to start connect, return code = %d\n", rc);
	}

	IoTT::mqttMessageQueueItem rcvMsg;

	while(!finished)
	{
		ssize_t size = mq_receive(mqttPubMQHandle, (char*)&rcvMsg, sizeof(IoTT::mqttMessageQueueItem), nullptr);
		if(0 < size)
		{
			MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
			if(MQTTASYNC_SUCCESS != (rc = MQTTAsync_sendMessage(client, rcvMsg.getTopic(), rcvMsg.getMessage(), &opts)))
			{
				printf("Failed to sendMessage, return code = %d\n", rc);
			}
		}
		else
		{
			perror("mq_receive");
		}
	}

	mq_close(mqttPubMQHandle);
	mq_close(mqttSubMQHandle);

	return nullptr;
}

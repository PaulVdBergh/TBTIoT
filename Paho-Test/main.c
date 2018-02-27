/*
 * Copyright (C) 2017 Paul Van den Bergh <admin@paulvandenbergh.be>
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
 *	For more info see http://paulvandenbergh.be/TBT
 * =====================================================================
 */

/*
 * main.cpp
 *
 *  Created on: Feb 19, 2018
 *      Author: paulvdbergh
 */

#include "MQTTClient.h"

#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ADDRESS "tcp://localhost:1883"

int main(int argc, char* argv[])
{
	char hostName[HOST_NAME_MAX];
	gethostname(hostName, HOST_NAME_MAX);

	int rc;

	MQTTClient client;
	MQTTClient_init_options initOpts = MQTTClient_init_options_initializer;

	MQTTClient_global_init(&initOpts);

	MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
	if((rc = MQTTClient_create(&client, ADDRESS, hostName, MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS))
	{
		printf("MQTTClient_create(...) failed.  Return code = %i\n", rc);
		exit(-1);
	}

	opts.keepAliveInterval = 20;
	opts.cleansession = 2;

	if((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("MQQTClient_connect(...) failed.  Return code = %i\n", rc);
		exit(-1);
	}

	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	pubmsg.payload = malloc(255);
	pubmsg.qos = 0;
	pubmsg.retained = 0;

	MQTTClient_deliveryToken token;

	uint32_t count = 1000000;

	while(--count)
	{
		sprintf(pubmsg.payload, "Hello World n° %i !!", count);
		pubmsg.payloadlen = strlen(pubmsg.payload);

		if((rc = MQTTClient_publishMessage(client, "TestTopic", &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
		{
			printf("MQTTClient_publishMessage(...) failed.  Return code = %i\n", rc);
			exit(-1);
		}

//		rc = MQTTClient_waitForCompletion(client, token, 10000L);

//		sleep(1);
	}

	free(pubmsg.payload);

	return 0;
}


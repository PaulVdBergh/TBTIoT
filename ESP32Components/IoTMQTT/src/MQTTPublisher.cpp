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
 * MQTTPublisher.cpp
 *
 *  Created on: Mar 10, 2018
 *      Author: paulvdbergh
 */

#include "MQTTPublisher.h"

extern MQTTClient client;

#include <esp_log.h>
static char tag[] = "MQTTPublisher";

namespace TBTIoT
{

	MQTTPublisher::MQTTPublisher(const string& topic)
	:	m_Topic(topic)
	{
		ESP_LOGI(tag, "In Constructor : topic = %s", topic.c_str());
	}

	MQTTPublisher::~MQTTPublisher()
	{
	// TODO Auto-generated destructor stub
	}

	int MQTTPublisher::Publish(const string& payload, QoS qos, bool retained)
	{
		MQTTMessage Message;
		Message.qos = qos;
		Message.retained = retained;
		Message.dup = 0;
		Message.id = 0;
		Message.payload = const_cast<char*>(payload.c_str());
		Message.payloadlen = payload.length();

		ESP_LOGI(tag, "Publishing \"%s\" in topic \"%s\"", payload.c_str(), m_Topic.c_str());
		return MQTTPublish(&client, m_Topic.c_str(), &Message);
	}

	int MQTTPublisher::Publish(void* payload, size_t payloadlen, QoS qos, bool retained)
	{
		MQTTMessage Message;
		Message.qos = qos;
		Message.retained = retained;
		Message.dup = 0;
		Message.id = 0;
		Message.payload = payload;
		Message.payloadlen = payloadlen;

		ESP_LOGI(tag, "Publishing \"binary\" in topic \"%s\"", m_Topic.c_str());
		return MQTTPublish(&client, m_Topic.c_str(), &Message);
	}

	int MQTTPublisher::Publish(const char* payload, QoS qos, bool retained)
	{
		MQTTMessage Message;
		Message.qos = qos;
		Message.retained = retained;
		Message.dup = 0;
		Message.id = 0;
		Message.payload = const_cast<char*>(payload);
		Message.payloadlen = strlen(payload);

		ESP_LOGI(tag, "Publishing \"%s\" in topic \"%s\"", payload, m_Topic.c_str());
		return MQTTPublish(&client, m_Topic.c_str(), &Message);
	}

	int MQTTPublisher::Publish(const uint8_t& uint8Val, QoS qos, bool retained)
	{
		char szBuffer[5];
		snprintf(szBuffer, 5, "%i", uint8Val);

		return Publish(szBuffer, qos, retained);
	}

	int MQTTPublisher::Publish(const uint16_t& uint16Val, QoS qos, bool retained)
	{
		char szBuffer[7];
		snprintf(szBuffer, 7, "%d", uint16Val);

		return Publish(szBuffer, qos, retained);
	}

	int MQTTPublisher::Publish(const bool& boolVal, QoS qos, bool retained)
	{
		ESP_LOGI(tag, "Publishing \"%s\" in topic \"%s\"", boolVal ? "true" : "false", m_Topic.c_str());

		return Publish(boolVal ? "true" : "false", qos, retained);
	}

} /* namespace TBTIoT */

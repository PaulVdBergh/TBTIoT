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
 * MqttPublisher.cpp
 *
 *  Created on: May 22, 2018
 *      Author: paulvdbergh
 */

#include "MqttPublisher.h"
#include "mqttMessageQueueItem.h"

#include <stdlib.h>
#include <cstring>

#include <sstream>
using namespace std;

#include "mqttSettings.h"

namespace IoTT
{

	/**
	 *
	 * @param topic
	 */
	MqttPublisher::MqttPublisher(const string& topic)
	:	m_Topic(topic)
	,	m_QueueHandle(-1)
	{
		m_QueueHandle = mq_open(IoTT::mqttPubMQName.c_str(), (O_CREAT | O_WRONLY), S_IRWXU, &IoTT::mqttMsgQueueAttribs);

	}

	/**
	 *
	 */
	MqttPublisher::~MqttPublisher()
	{
		mq_close(m_QueueHandle);
	}

	/**
	 *
	 * @param payload
	 * @param qos
	 * @param retained
	 * @return
	 */
	int MqttPublisher::Publish(const string& payload, int qos, bool retained)
	{
		return Publish((void*)payload.c_str(), payload.length(), qos, retained);
	}

	/**
	 *
	 * @param payload
	 * @param qos
	 * @param retained
	 * @return
	 */
	int MqttPublisher::Publish(const char* payload, int qos, bool retained)
	{
		return Publish((void*)payload, strlen(payload), qos, retained);
	}

	/**
	 *
	 * @param uint8Val
	 * @param qos
	 * @param retained
	 * @return
	 */
	int MqttPublisher::Publish(const uint8_t& uint8Val, int qos, bool retained)
	{
		char	szBuffer[33];
		snprintf(szBuffer, 33, "%i", uint8Val);
		printf("MqttPublisher::Publish(const uint8_t& '%i') will publish '%s'\n", uint8Val, szBuffer);
		return Publish(szBuffer, qos, retained);
	}

	/**
	 *
	 * @param boolVal
	 * @param qos
	 * @param retained
	 * @return
	 */
	int MqttPublisher::Publish(const bool& boolVal, int qos, bool retained)
	{
		stringstream ss;
		ss << (boolVal ? "true" : "false");
		return Publish(ss.str(), qos, retained);
	}

	/**
	 *
	 * @param payload
	 * @param payloadLen
	 * @param qos
	 * @param retained
	 * @return
	 */
	int MqttPublisher::Publish(void* payload, size_t payloadLen, int qos, bool retained)
	{
		mqttMessageQueueItem item(
				m_Topic.c_str(),
				qos,
				retained,
				0,
				0,
				(uint8_t*)payload,
				payloadLen);

		int rc = mq_send(m_QueueHandle, (const char*)&item, sizeof(item), 0);
		return rc;
	}

} /* namespace IoTT */

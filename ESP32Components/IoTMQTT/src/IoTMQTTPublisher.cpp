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
 * IoTMQTTPublisher.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: paulvdbergh
 */

#include "IoTMQTTPublisher.h"

namespace TBTIoT
{

/*	IoTMQTTPublisher::IoTMQTTPublisher(const char* topic)
	:	IoTMQTTPublisher(string(topic))
	{}
*/
	IoTMQTTPublisher::IoTMQTTPublisher(const string& topic)
	:	m_Topic(topic)
	,	m_Interface(IoTMQTT::getInstance())
	{}

	IoTMQTTPublisher::~IoTMQTTPublisher()
	{}

	int IoTMQTTPublisher::Publish(const string& payload,
			const QoS qos, /* = QOS0 */
			const unsigned char retained /* = false */)
	{
		MQTTMessage message;
		message.payload = const_cast<char*>(payload.c_str());
		message.payloadlen = payload.size();
		message.qos = qos;
		message.retained = retained;

		return m_Interface->Publish(m_Topic.c_str(), &message);
	}

	int IoTMQTTPublisher::Publish(char* payload,
			const QoS qos, /* = QOS0 */
			const unsigned char retained /* = false */)
	{
		MQTTMessage message;
		message.payload = payload;
		message.payloadlen = strlen(payload);
		message.qos = qos;
		message.retained = retained;

		return m_Interface->Publish(m_Topic.c_str(), &message);
	}

	int IoTMQTTPublisher::Publish(void* payload,
			const size_t payloadlen,
			const QoS qos, /* = QOS0 */
			const unsigned char retained /* = false */)
	{
		MQTTMessage message;
		message.payload = payload;
		message.payloadlen = payloadlen;
		message.qos = qos;
		message.retained = retained;

		return m_Interface->Publish(m_Topic.c_str(), &message);
	}


} /* namespace TBTIoT */

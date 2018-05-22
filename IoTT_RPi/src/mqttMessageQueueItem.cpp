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
 * mqttMessageQueueItem.cpp
 *
 *  Created on: May 21, 2018
 *      Author: paulvdbergh
 */

#include "mqttMessageQueueItem.h"

#include <cstring>

namespace IoTT
{

	mqttMessageQueueItem::mqttMessageQueueItem(
			const char*	topic,
			int			qos,
			uint8_t		retained,
			uint8_t		dup,
			uint16_t	id,
			uint8_t*	payload,
			size_t		payloadLen)
	:	m_QOS(qos)
	,	m_Retained(retained)
	,	m_Dup(dup)
	,	m_Id(id)
	,	m_PayloadLen(payloadLen)
	{
		strncpy(m_Topic, topic, MAX_TOPIC_SIZE);
		m_Topic[MAX_TOPIC_SIZE] = 0x00;

		memset(m_Payload, 0, MAX_PAYLOAD_SIZE);
		if(payload && payloadLen)
		{
			memcpy(m_Payload, payload, payloadLen > MAX_PAYLOAD_SIZE ? MAX_PAYLOAD_SIZE : payloadLen);
		}
	}

	mqttMessageQueueItem::~mqttMessageQueueItem()
	{
		// TODO Auto-generated destructor stub
	}

	MQTTAsync_message* mqttMessageQueueItem::getMessage()
	{
		static MQTTAsync_message msg;
		msg = MQTTAsync_message_initializer;
		msg.payload = m_Payload;
		msg.payloadlen = m_PayloadLen;
		msg.qos = m_QOS;
		msg.retained = m_Retained;
		msg.dup = m_Dup;

		return &msg;
	}

} /* namespace IoTT */

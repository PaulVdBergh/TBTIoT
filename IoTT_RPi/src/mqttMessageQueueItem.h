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
 * mqttMessageQueueItem.h
 *
 *  Created on: May 21, 2018
 *      Author: paulvdbergh
 */

#ifndef SRC_MQTTMESSAGEQUEUEITEM_H_
#define SRC_MQTTMESSAGEQUEUEITEM_H_

#include <stddef.h>
#include <stdint.h>
#include <mqtt/MQTTAsync.h>

#define MAX_TOPIC_SIZE 256
#define MAX_PAYLOAD_SIZE 256

namespace IoTT
{

	class mqttMessageQueueItem
	{
		public:
			mqttMessageQueueItem(
					const char*	topic = "",
					int			qos = 0,
					uint8_t		retained = 0,
					uint8_t		dup = 0,
					uint16_t	id = 0,
					uint8_t*	payload = nullptr,
					size_t		payloadLen = 0);

			virtual ~mqttMessageQueueItem();

			const char* getTopic(void) const
			{
				return m_Topic;
			}

			const size_t getPayloadLen(void) const
			{
				return m_PayloadLen;
			}

			const uint8_t* getPayload(void) const
			{
				return m_Payload;
			}

			MQTTAsync_message* getMessage(void);

		protected:

		private:
			char		m_Topic[MAX_TOPIC_SIZE];
			int			m_QOS;
			uint8_t		m_Retained;
			uint8_t		m_Dup;
			uint16_t	m_Id;
			uint8_t		m_Payload[MAX_PAYLOAD_SIZE];
			size_t		m_PayloadLen;
	};

} /* namespace IoTT */

#endif /* SRC_MQTTMESSAGEQUEUEITEM_H_ */

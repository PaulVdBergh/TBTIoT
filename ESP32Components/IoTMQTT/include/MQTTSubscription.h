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
 * MQTTSubscription.h
 *
 *  Created on: Mar 7, 2018
 *      Author: paulvdbergh
 */

#ifndef IOTMQTT_INCLUDE_MQTTSUBSCRIPTION_H_
#define IOTMQTT_INCLUDE_MQTTSUBSCRIPTION_H_

//#include "MQTTClient.h"
#include <string>

#include "../src/IoTMQTTMessageQueueItem.h"
using namespace std;

namespace TBTIoT
{

	class MQTTSubscription
	{
		public:
			typedef enum
			{
				PRIORITY_HIGHEST = 0,
				PRIORITY_HIGH,
				PRIORITY_NORMAL,
				PRIORITY_LOW,
				PRIORITY_LOWEST
			} Priority;

			MQTTSubscription(const string& topic, Priority priority = PRIORITY_NORMAL);
			virtual ~MQTTSubscription();

			bool isTopicMatched(IoTMQTTMessageQueueItem* pItem);

			virtual void OnNewData(IoTMQTTMessageQueueItem* pItem);

			Priority getPriority(void) { return m_Priority; }

			const string& getTopic(void) { return m_Topic; }

		protected:
			const string	m_Topic;
			const Priority	m_Priority;

		private:

	};

} /* namespace TBTIoT */

#endif /* IOTMQTT_INCLUDE_MQTTSUBSCRIPTION_H_ */

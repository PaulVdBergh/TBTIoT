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
 * IoTMQTTSubscription.h
 *
 *  Created on: Mar 5, 2018
 *      Author: paulvdbergh
 */

#ifndef COMPONENTS_MQTTCLIENT_C_SRC_IOTMQTTSUBSCRIPTION_H_
#define COMPONENTS_MQTTCLIENT_C_SRC_IOTMQTTSUBSCRIPTION_H_

#include "IoTMQTT.h"

#include <string>

using namespace std;

namespace TBTIoT
{

	class IoTMQTTSubscription
	{
		public:
			IoTMQTTSubscription(const char* topic, QoS qos);
			virtual ~IoTMQTTSubscription();

			char* 		getTopic(void)	{ return const_cast<char*>(m_Topic.c_str()); }
			int*		getpQoS(void)	{ return (int*)&m_QoS; }

			virtual void OnTopicNotification(MessageData* md) = 0;

		protected:
			string	m_Topic;
			QoS		m_QoS;

		private:

	};

} /* namespace TBTIoT */

#endif /* COMPONENTS_MQTTCLIENT_C_SRC_IOTMQTTSUBSCRIPTION_H_ */

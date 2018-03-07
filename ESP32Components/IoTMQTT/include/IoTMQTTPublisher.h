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
 * IoTMQTTPublisher.h
 *
 *  Created on: Mar 6, 2018
 *      Author: paulvdbergh
 */

#ifndef COMPONENTS_MQTTCLIENT_C_SRC_IOTMQTTPUBLISHER_H_
#define COMPONENTS_MQTTCLIENT_C_SRC_IOTMQTTPUBLISHER_H_

#include "IoTMQTT.h"

#include <string>

using namespace std;

namespace TBTIoT
{

	class IoTMQTTPublisher
	{
		public:
//			IoTMQTTPublisher(const char* topic);
			IoTMQTTPublisher(const string& topic);

			int Publish(const string& payload,
					const QoS qos = QOS0,
					const unsigned char retained = false);

			int Publish(char* payload,
					const QoS qos = QOS0,
					const unsigned char retained = false);

			int Publish(void* payload,
					const size_t payloadlen,
					const QoS qos = QOS0,
					const unsigned char retained = false);

			virtual ~IoTMQTTPublisher();

		protected:
			const string	m_Topic;

		private:
			IoTMQTT*	m_Interface;

	};

} /* namespace TBTIoT */

#endif /* COMPONENTS_MQTTCLIENT_C_SRC_IOTMQTTPUBLISHER_H_ */

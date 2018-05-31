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
 * MQTTPublisher.h
 *
 *  Created on: Mar 10, 2018
 *      Author: paulvdbergh
 */

#ifndef IOTMQTT_INCLUDE_MQTTPUBLISHER_H_
#define IOTMQTT_INCLUDE_MQTTPUBLISHER_H_

#include "../MQTTClient-C/src/linux/MQTTClient.h"

#include <string>
using namespace std;

namespace TBTIoT
{

	class MQTTPublisher
	{
		public:
			MQTTPublisher(const string& topic);
			virtual ~MQTTPublisher();

			int Publish(const string& payload, QoS qos = QOS0, bool retained = false);
			int Publish(const char* payload, QoS qos = QOS0, bool retained = false);
			int Publish(const uint8_t& uint8Val, QoS qos = QOS0, bool retained = false);
			int Publish(const uint16_t& uint8Val, QoS qos = QOS0, bool retained = false);
			int Publish(const bool& boolVal, QoS qos = QOS0, bool retained = false);

			int Publish(void* payload, size_t payloadlen, QoS qos = QOS0, bool retained = false);

		protected:
			string	m_Topic;

		private:
	};

} /* namespace TBTIoT */

#endif /* IOTMQTT_INCLUDE_MQTTPUBLISHER_H_ */

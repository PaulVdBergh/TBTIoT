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
 * MqttPublisher.h
 *
 *  Created on: May 22, 2018
 *      Author: paulvdbergh
 */

#ifndef SRC_MQTTPUBLISHER_H_
#define SRC_MQTTPUBLISHER_H_

#include <mqueue.h>

#include <string>
using namespace std;

namespace IoTT
{

	class MqttPublisher
	{
		public:
			MqttPublisher(const string& topic);
			virtual ~MqttPublisher();

			int Publish(const string& payload, int qos = 0, bool retained = false);
			int Publish(const char* payload, int qos = 0, bool retained = false);
			int Publish(const uint8_t& uint8Val, int qos = 0, bool retained = false);
			int Publish(const bool& boolVal, int qos = 0, bool retained = false);

			int Publish(void* payload, size_t payloadlen, int qos = 0, bool retained = false);

		protected:
			const string	m_Topic;
			mqd_t			m_QueueHandle;

		private:

	};

} /* namespace IoTT */

#endif /* SRC_MQTTPUBLISHER_H_ */

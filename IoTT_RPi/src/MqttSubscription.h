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
 * MqttSubscription.h
 *
 *  Created on: May 21, 2018
 *      Author: paulvdbergh
 */

#ifndef SRC_MQTTSUBSCRIPTION_H_
#define SRC_MQTTSUBSCRIPTION_H_

#include "mqttMessageQueueItem.h"

#include <string>

using namespace std;

namespace IoTT
{

	class MqttSubscription
	{
		public:
			MqttSubscription(const char* topic);
			virtual ~MqttSubscription();

			bool isTopicMatched(const mqttMessageQueueItem& item);

			virtual void OnNewData(const mqttMessageQueueItem& item) = 0;

		protected:
			const string	m_Topic;

		private:
	};

} /* namespace IoTT */

#endif /* SRC_MQTTSUBSCRIPTION_H_ */

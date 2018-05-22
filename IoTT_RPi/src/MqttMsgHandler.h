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
 * MqttMsgHandler.h
 *
 *  Created on: May 21, 2018
 *      Author: paulvdbergh
 */

#ifndef SRC_MQTTMSGHANDLER_H_
#define SRC_MQTTMSGHANDLER_H_

#include "MqttSubscription.h"

#include <mutex>
#include <thread>
#include <vector>

using namespace std;

namespace IoTT
{

	class MqttMsgHandler
	{
		public:
			static MqttMsgHandler* getInstance(void);
			virtual ~MqttMsgHandler();

			void RegisterSubscription(MqttSubscription* pSub);
			void UnRegisterSubscription(MqttSubscription* pSub);

		protected:
			MqttMsgHandler();

		private:
			void threadFunc(void);

			class ExecSub
			{
				public:
					ExecSub(mqttMessageQueueItem& item);
					void operator()(MqttSubscription* pSub);

				private:
					mqttMessageQueueItem& m_Item;
			};

			thread	m_Thread;

			vector<MqttSubscription*> m_Subscriptions;
			recursive_mutex m_MSubscriptions;

			static MqttMsgHandler*	sm_pInstance;
	};

} /* namespace IoTT */

#endif /* SRC_MQTTMSGHANDLER_H_ */

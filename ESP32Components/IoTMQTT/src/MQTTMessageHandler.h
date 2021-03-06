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
 * MQTTMessageHandler.h
 *
 *  Created on: Mar 8, 2018
 *      Author: paulvdbergh
 */

#ifndef IOTMQTT_SRC_MQTTMESSAGEHANDLER_H_
#define IOTMQTT_SRC_MQTTMESSAGEHANDLER_H_

#include <algorithm>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "MQTTSubscription.h"
#include "IoTMQTTMessageQueueItem.h"
using namespace std;

extern MQTTClient client;

namespace TBTIoT
{

	class BackupSubscription;	//	forward declaration

	class MQTTMessageHandler
	{
		public:
			static MQTTMessageHandler* getInstance(void);
			virtual ~MQTTMessageHandler();

			void RegisterSubscription(MQTTSubscription* pSub);
			void unRegisterSubscription(MQTTSubscription* pSub);

			void updateBackup(IoTMQTTMessageQueueItem* pItem);

		protected:
			MQTTMessageHandler();
			void threadFunc(void);

		private:
			BackupSubscription*	m_pBackupSubscription;

			thread	m_thread;

			vector<MQTTSubscription*>	m_Subscriptions;
			recursive_mutex	m_MSubscriptions;

			map<string, IoTMQTTMessageQueueItem> m_BackupMessages;
			recursive_mutex m_MBackupMessages;

			static MQTTMessageHandler*	sm_pInstance;
	};

} /* namespace TBTIoT */

#endif /* IOTMQTT_SRC_MQTTMESSAGEHANDLER_H_ */

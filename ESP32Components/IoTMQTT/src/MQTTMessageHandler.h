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

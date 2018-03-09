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
			MQTTSubscription(const string& topic);
			virtual ~MQTTSubscription();

			bool isTopicMatched(IoTMQTTMessageQueueItem* pItem);

			virtual void OnNewData(IoTMQTTMessageQueueItem* pItem);

		protected:
			const string	m_Topic;

		private:

	};

} /* namespace TBTIoT */

#endif /* IOTMQTT_INCLUDE_MQTTSUBSCRIPTION_H_ */

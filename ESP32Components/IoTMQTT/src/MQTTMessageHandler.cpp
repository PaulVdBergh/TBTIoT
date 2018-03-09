/*
 * MQTTMessageHandler.cpp
 *
 *  Created on: Mar 8, 2018
 *      Author: paulvdbergh
 */

#include "MQTTMessageHandler.h"

#include "esp_log.h"
static char tag[] = "MQTTMessageHandler";

namespace TBTIoT
{
	MQTTMessageHandler* MQTTMessageHandler::getInstance()
	{
		if(nullptr == sm_pInstance)
		{
			sm_pInstance = new MQTTMessageHandler();
		}
		return sm_pInstance;
	}

	MQTTMessageHandler::MQTTMessageHandler()
	{
		ESP_LOGI(tag, "Starting threadFunc");
		m_thread = thread([this]{threadFunc();});
	}

	MQTTMessageHandler::~MQTTMessageHandler()
	{
		// TODO Auto-generated destructor stub
	}

	void MQTTMessageHandler::RegisterSubscription(MQTTSubscription* pSub)
	{
		lock_guard<recursive_mutex> lock(m_MSubscriptions);
		m_Subscriptions.push_back(pSub);
	}

	void MQTTMessageHandler::unRegisterSubscription(MQTTSubscription* pSub)
	{
		lock_guard<recursive_mutex> lock(m_MSubscriptions);
		auto thisOne = find(m_Subscriptions.begin(), m_Subscriptions.end(), pSub);
		if(m_Subscriptions.end() != thisOne)
		{
			m_Subscriptions.erase(thisOne);
		}
	}

	void MQTTMessageHandler::threadFunc(void)
	{
		ESP_LOGI(tag, "Started threadFunc");

		IoTMQTTMessageQueueItem QueueItem;

		struct exec_t
		{
			IoTMQTTMessageQueueItem* m_pItem;
			exec_t(IoTMQTTMessageQueueItem* pItem) : m_pItem(pItem) {}
			void operator()(MQTTSubscription* pS)
			{
				if(pS->isTopicMatched(m_pItem))
				{
					pS->OnNewData(m_pItem);
				}
			}
		} exec(&QueueItem);

		while(true)
		{
			ESP_LOGI(tag, "Waiting for Item...");
			if(pdPASS == xQueueReceive(IoTMQTTMessageQueue, &QueueItem, portMAX_DELAY))
			{
				ESP_LOGI(tag, "Item received");
				lock_guard<recursive_mutex> lock(m_MSubscriptions);
				for_each(m_Subscriptions.begin(), m_Subscriptions.end(), exec);
			}
		}
	}

	MQTTMessageHandler* MQTTMessageHandler::sm_pInstance = nullptr;

} /* namespace TBTIoT */

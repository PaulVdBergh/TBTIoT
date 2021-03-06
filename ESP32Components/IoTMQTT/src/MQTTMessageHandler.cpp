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
 * MQTTMessageHandler.cpp
 *
 *  Created on: Mar 8, 2018
 *      Author: paulvdbergh
 */

#include "MQTTMessageHandler.h"

#include <algorithm>

#include "esp_log.h"
static char tag[] = "MQTTMessageHandler";

namespace TBTIoT
{
	class BackupSubscription : public MQTTSubscription
	{
		public:
			BackupSubscription(MQTTMessageHandler* pHandler)
			:	MQTTSubscription("#", PRIORITY_LOWEST)
			,	m_pHandler(pHandler)
			{

			}

			void OnNewData(IoTMQTTMessageQueueItem* pItem)
			{
				static char tag[] = "BackupSubscription";
				ESP_LOGI(tag, "OnNewData : topic = %s", pItem->m_Topic);
				m_pHandler->updateBackup(pItem);
			}

		protected:
			MQTTMessageHandler*	m_pHandler;

		private:

	};


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

	bool sortfunc(MQTTSubscription* pA, MQTTSubscription* pB)
	{
		return pA->getPriority() < pB->getPriority();
	};

	void MQTTMessageHandler::RegisterSubscription(MQTTSubscription* pSub)
	{
		ESP_LOGI(tag, "Registering subscription [%d].", (uint32_t)pSub);

		bool bNewSubscription = false;
		{
			lock_guard<recursive_mutex> lock(m_MSubscriptions);
			auto thisone = find(m_Subscriptions.begin(), m_Subscriptions.end(), pSub);
			bNewSubscription = (m_Subscriptions.end() == thisone);
			if(bNewSubscription)
			{
				ESP_LOGI(tag, "Pushing new subscription...");
				m_Subscriptions.push_back(pSub);
				sort(m_Subscriptions.begin(), m_Subscriptions.end(), sortfunc);
			}
		}
		if(bNewSubscription)
		{
			lock_guard<recursive_mutex> lock1(m_MBackupMessages);
			for(auto itempair : m_BackupMessages)
			{
				if(pSub->isTopicMatched(&(itempair.second)))
				{
					pSub->OnNewData(&(itempair.second));
				}
			}
		}
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
	void MQTTMessageHandler::updateBackup(IoTMQTTMessageQueueItem* pItem)
	{
		string topic(pItem->m_Topic);
		lock_guard<recursive_mutex> lock(m_MBackupMessages);
		m_BackupMessages[topic] = *pItem;
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
				ESP_LOGI(tag, "Testing Subscription Topic %s against %s", pS->getTopic().c_str(), m_pItem->m_Topic);
				if(pS->isTopicMatched(m_pItem))
				{
					ESP_LOGI(tag, "Test Succeeded !!");
					pS->OnNewData(m_pItem);
				}
				else
				{
					ESP_LOGI(tag, "Test Failed !!");
				}
			}
		} exec(&QueueItem);

		vTaskDelay(1 / portTICK_PERIOD_MS);

		m_pBackupSubscription = new BackupSubscription(this);

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

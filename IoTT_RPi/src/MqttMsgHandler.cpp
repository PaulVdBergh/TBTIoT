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
 * MqttMsgHandler.cpp
 *
 *  Created on: May 21, 2018
 *      Author: paulvdbergh
 */

#include "MqttMsgHandler.h"

#include "Decoders.h"
#include "mqttSettings.h"
#include "mqttMessageQueueItem.h"

#include <algorithm>

#include <mqueue.h>

using namespace std;

namespace IoTT
{
	/**
	 *
	 * @return
	 */
	MqttMsgHandler* MqttMsgHandler::getInstance()
	{
		return sm_pInstance ? sm_pInstance : sm_pInstance = new MqttMsgHandler();
	}

	/**
	 *
	 */
	MqttMsgHandler::MqttMsgHandler()
	{
		m_Thread = thread([this]{threadFunc();});
	}

	/**
	 *
	 */
	MqttMsgHandler::~MqttMsgHandler()
	{
		// TODO Auto-generated destructor stub
	}

	/**
	 *
	 * @param pSub
	 */
	void MqttMsgHandler::RegisterSubscription(MqttSubscription* pSub)
	{
		lock_guard<recursive_mutex> lock(m_MSubscriptions);
		auto thisone = find(m_Subscriptions.begin(), m_Subscriptions.end(), pSub);
		if(m_Subscriptions.end() == thisone)
		{
			m_Subscriptions.push_back(pSub);
		}
	}

	/**
	 *
	 * @param pSub
	 */
	void MqttMsgHandler::UnRegisterSubscription(MqttSubscription* pSub)
	{
		lock_guard<recursive_mutex> lock(m_MSubscriptions);
		auto thisone = find(m_Subscriptions.begin(), m_Subscriptions.end(), pSub);
		if(m_Subscriptions.end() != thisone)
		{
			m_Subscriptions.erase(thisone);
		}
	}

	/**
	 *
	 */
	void MqttMsgHandler::threadFunc()
	{
		mqd_t mqttSubMQHandle;

		if(-1 == (mqttSubMQHandle = mq_open(IoTT::mqttSubMQName.c_str(), (O_CREAT | O_RDONLY), S_IRWXU, &IoTT::mqttMsgQueueAttribs)))
		{
			printf("Failed to open/create mqttSubMQHandle\n");
			perror("error");
		}

		mqttMessageQueueItem rcvMsg;

		while(true)
		{
			ssize_t size = mq_receive(mqttSubMQHandle, (char*)&rcvMsg, sizeof(mqttMessageQueueItem), nullptr);

			printf("MqttMsgHandler received message :\n");
			printf("\tTopic = %s\n", rcvMsg.getTopic());
			printf("\tpayload = %s\n", (const char*)rcvMsg.getPayload());

			if(0 < size)
			{
				ExecSub exec(rcvMsg);
				exec(Decoders::getInstance());
				lock_guard<recursive_mutex> lock(m_MSubscriptions);
				for(auto sub : m_Subscriptions) exec(sub);
			}
		}

		mq_close(mqttSubMQHandle);
	}

	/**
	 *
	 * @param item
	 */
	MqttMsgHandler::ExecSub::ExecSub(mqttMessageQueueItem& item)
	:	m_Item(item)
	{

	}

	/**
	 *
	 * @param pSub
	 */
	void MqttMsgHandler::ExecSub::operator()(MqttSubscription* pSub)
	{
		if(pSub->isTopicMatched(m_Item))
		{
			pSub->OnNewData(m_Item);
		}
	}

	MqttMsgHandler* MqttMsgHandler::sm_pInstance = nullptr;

} /* namespace IoTT */

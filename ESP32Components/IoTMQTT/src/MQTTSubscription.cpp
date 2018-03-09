/*
 * MQTTSubscription.cpp
 *
 *  Created on: Mar 7, 2018
 *      Author: paulvdbergh
 */

#include "MQTTSubscription.h"

#include <string.h>

#include <esp_log.h>
static char tag[] = "MQTTSubscription";

#include "MQTTMessageHandler.h"

namespace TBTIoT
{

	MQTTSubscription::MQTTSubscription(const string& topic)
	:	m_Topic(topic)
	{
		MQTTMessageHandler::getInstance()->RegisterSubscription(this);
		ESP_LOGI(tag, "Registration complete for topic %s", (char*)m_Topic.c_str());
	}

	MQTTSubscription::~MQTTSubscription()
	{
		MQTTMessageHandler::getInstance()->unRegisterSubscription(this);
	}

	bool MQTTSubscription::isTopicMatched(IoTMQTTMessageQueueItem* pItem)
	{
	    char* curf = const_cast<char*>(m_Topic.c_str());
	    char* curn = pItem->m_Topic;
	    char* curn_end = curn + strlen(curn);

	    while (*curf && curn < curn_end)
	    {
	        if (*curn == '/' && *curf != '/')
	            break;
	        if (*curf != '+' && *curf != '#' && *curf != *curn)
	            break;
	        if (*curf == '+')
	        {   // skip until we meet the next separator, or end of string
	            char* nextpos = curn + 1;
	            while (nextpos < curn_end && *nextpos != '/')
	                nextpos = ++curn + 1;
	        }
	        else if (*curf == '#')
	            curn = curn_end - 1;    // skip until end of string
	        curf++;
	        curn++;
	    };

	    return (curn == curn_end) && (*curf == '\0');
	}

	void MQTTSubscription::OnNewData(IoTMQTTMessageQueueItem* pItem)
	{
		ESP_LOGI(tag, "In OnNewData :\n\tTopic = %s\n\tMessage = %*s", pItem->m_Topic, pItem->m_payloadlen, pItem->m_payload);
	}

} /* namespace TBTIoT */

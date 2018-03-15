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

	MQTTSubscription::MQTTSubscription(const string& topic, Priority priority /* = PRIORITY_NORMAL */)
	:	m_Topic(topic)
	,	m_Priority(priority)
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

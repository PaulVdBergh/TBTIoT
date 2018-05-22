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
 * MqttSubscription.cpp
 *
 *  Created on: May 21, 2018
 *      Author: paulvdbergh
 */

#include "MqttSubscription.h"
#include "MqttMsgHandler.h"

#include <cstring>

namespace IoTT
{

	MqttSubscription::MqttSubscription(const char* topic)
	:	m_Topic(topic)
	{
		MqttMsgHandler::getInstance()->RegisterSubscription(this);
	}

	MqttSubscription::~MqttSubscription()
	{
		MqttMsgHandler::getInstance()->UnRegisterSubscription(this);
	}

	bool MqttSubscription::isTopicMatched(const mqttMessageQueueItem& item)
	{
	    char* curf = const_cast<char*>(m_Topic.c_str());
	    char* curn = const_cast<char*>(const_cast<mqttMessageQueueItem&>(item).getTopic());
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

} /* namespace IoTT */

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
 * DCCDecoders.cpp
 *
 *  Created on: Mar 10, 2018
 *      Author: paulvdbergh
 */

#include "Decoders.h"

#include "AccessoryDecoder.h"
#include "LocDecoder.h"

#include <cstring>

#include <esp_log.h>
static char tag[] = "Decoders";

namespace TBTIoT
{
	Decoders* Decoders::getInstance()
	{
		return sm_Instance ? sm_Instance : sm_Instance = new Decoders;
	}

	Decoders::Decoders(const string& MQTTTopicBasePath)
	:	MQTTSubscription(MQTTTopicBasePath + "#")
	,	m_TopicBasePath(MQTTTopicBasePath)
	{
		ESP_LOGI(tag, "Decoders constructed");
	}

	Decoders::~Decoders()
	{
	// TODO Auto-generated destructor stub
	}

	void Decoders::OnNewData(IoTMQTTMessageQueueItem* pItem)
	{
		ESP_LOGI(tag, "OnNewData : topic = %s, m_TopicBasePath = %s", pItem->m_Topic, m_TopicBasePath.c_str());

		if(0 == strncmp(m_TopicBasePath.c_str(), pItem->m_Topic, m_TopicBasePath.length()))
		{
			char* szAddress = &(pItem->m_Topic[m_TopicBasePath.length()]);
			ESP_LOGI(tag, "szAddress = %s", szAddress);
			DCCAddress_t DccAddress = atoi(szAddress);
			if(DccAddress)
			{
				Decoder* pDecoder = getDecoder(DccAddress);
				if(pDecoder)
				{
					pDecoder->onNewMQTTData(szAddress, (char*)pItem->m_payload);
				}
			}
		}
	}

	Decoder* Decoders::getDecoder(const DCCAddress_t& address)
	{
		ESP_LOGI(tag, "getDecoder(%i)", address);
		if(!address)
		{
			return nullptr;
		}

		lock_guard<recursive_mutex> lock(sm_MDecoders);

		auto pair = sm_Decoders.find(address);
		if(sm_Decoders.end() != pair)
		{
			return pair->second;
		}

		if((0x0000 < address) && (address < 0x0080))
		{
			//	LocDecoder with 7-bit addressing
			return sm_Decoders[address] = new LocDecoder(address);
		}
		else if((0xC07F < address) && (address < 0xE800))
		{
			//	LocDecoder with 14-bit addressing
			return sm_Decoders[address] = new LocDecoder(address);
		}
		else if((0x7FFF < address) && (address < 0xC07F))
		{
			//	AccessoryDecoder
			return sm_Decoders[address] = new AccessoryDecoder(address);
		}
		return nullptr;
	}

	Decoders*	Decoders::sm_Instance = nullptr;

} /* namespace TBTIoT */

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
 * Decoders.cpp
 *
 *  Created on: May 21, 2018
 *      Author: paulvdbergh
 */

#include "Decoders.h"
#include "AccessoryDecoder.h"
#include "LocDecoder.h"

#include <cstring>

#include <algorithm>

using namespace std;

namespace IoTT
{
	Decoders* Decoders::getInstance()
	{
		return sm_pInstance ? sm_pInstance : sm_pInstance = new Decoders();
	}

	Decoders::Decoders(const string& MqttTopicBasePath)
	:	MqttSubscription((MqttTopicBasePath + "#").c_str())
	,	m_TopicBasePath(MqttTopicBasePath)
	{
	// TODO Auto-generated constructor stub
	}

	Decoders::~Decoders()
	{
		lock_guard<recursive_mutex> lock(m_MDecoders);

		for(auto decoderPair : m_Decoders) delete decoderPair.second;
	}

	Decoder* Decoders::getDecoder(const DCCAddress_t& address)
	{
		if(!address)
		{
			return nullptr;
		}

		lock_guard<recursive_mutex> lock(m_MDecoders);

		auto pair = m_Decoders.find(address);
		if(m_Decoders.end() != pair)
		{
			return pair->second;
		}

		if((0x0000 < address) && (address < 0x0080))
		{
			//	LocDecoder with 7-bit addressing
			return m_Decoders[address] = new LocDecoder(address);
		}
		else if((0xC07F < address) && (address < 0xE800))
		{
			//	LocDecoder with 14-bit addressing
			return m_Decoders[address] = new LocDecoder(address);
		}
		else if((0x7FFF < address) && (address < 0xC07F))
		{
			//	AccessoryDecoder
			return m_Decoders[address] = new AccessoryDecoder(address);
		}
		return nullptr;
	}

	void Decoders::OnNewData(const mqttMessageQueueItem& item)
	{
		printf("Decoders::OnNewData : comparing interface ['%s']\n\tto item ['%s']\n", m_TopicBasePath.c_str(), item.getTopic());

		if(0 == strncmp(m_TopicBasePath.c_str(), item.getTopic(), m_TopicBasePath.length()))
		{
			printf("\tComparison sucseeded !!\n");

			const char* szAddress = &(item.getTopic()[m_TopicBasePath.length()]);
			DCCAddress_t DccAddress = atoi(szAddress);

			printf("\tDccAddress = %d\n", DccAddress);

			if(DccAddress)
			{
				Decoder* pDecoder = getDecoder(DccAddress);
				if(pDecoder)
				{
					pDecoder->onNewMqttData(szAddress, item.getPayloadLen(), item.getPayload());
				}
			}
		}
	}

	Decoders* Decoders::sm_pInstance = nullptr;

} /* namespace IoTT */

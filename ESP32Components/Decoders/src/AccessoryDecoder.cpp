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
 * AccessoryDecoder.cpp
 *
 *  Created on: Mar 11, 2018
 *      Author: paulvdbergh
 */

#include "AccessoryDecoder.h"
#include "Accessory.h"
#include "MQTTPublisher.h"

#include <cstdio>
#include <cstring>
using namespace std;

#include <esp_log.h>
static char tag[] = "AccessoryDecoder";

namespace TBTIoT
{

	AccessoryDecoder::AccessoryDecoder(const DCCAddress_t& address)
	:	Decoder(address)
	{
		ESP_LOGI(tag, "AccessoryDecoder(%i)", address);
		for(uint8_t port = 0; port < 4; port++)
		{
			m_pAccessories[port] = new Accessory(this, port);
		}
	}

	AccessoryDecoder::~AccessoryDecoder()
	{
		for(auto item : m_pAccessories)
		{
			delete item;
		}
	}

	bool AccessoryDecoder::getNextDCCCommand(uint8_t* pBuffer)
	{
		static uint8_t _currentAccessory = 0;
		uint8_t currentAccessory = _currentAccessory;
		do
		{
			if(m_pAccessories[currentAccessory++]->getDCCMessage(pBuffer))
			{
				_currentAccessory = currentAccessory % 4;
				return true;
			}
			currentAccessory %= 4;
		}
		while(currentAccessory != _currentAccessory);

		return false;
	}

	uint8_t AccessoryDecoder::getState(const uint8_t port)
	{
		return m_pAccessories[port]->getUDPState();
	}

	void AccessoryDecoder::setDesiredState(const uint8_t& port, const uint8_t& outputNbr, const uint8_t& state)
	{
		char szTopic[256];
		snprintf(szTopic, 256, "%sDesired/%i/%i", m_BaseTopic.c_str(), port, outputNbr);
		MQTTPublisher(szTopic).Publish(state);
	}

	void AccessoryDecoder::onNewMQTTData(const string& topic, const string& payload)
	{
		ESP_LOGI(tag, "onNewMQTTData(%s, %s)", topic.c_str(), payload.c_str());
		if(m_DCCAddress == atoi(topic.c_str()))
		{
			char* szAction = strchr(topic.c_str(), '/');
			if(szAction && *(++szAction))
			{
				if(0 == strncmp("Desired", szAction, strlen("Desired")))
				{
					char* szPort = strchr(szAction, '/');
					if(szPort && *(++szPort))
					{
						uint8_t port = atoi(szPort);
						ESP_LOGI(tag, "\tPort = \"%i\"", port);
						char* szOutput = strchr(szPort, '/');
						if(szOutput && *(++szOutput))
						{
							uint8_t outputNbr = atoi(szOutput);
							ESP_LOGI(tag, "\tOutputNbr = \"%i\"", outputNbr);

							uint8_t value = atoi(payload.c_str());

							m_pAccessories[port]->setState(outputNbr, value);
						}
					}
				}
			}
		}
	}

} /* namespace TBTIoT */

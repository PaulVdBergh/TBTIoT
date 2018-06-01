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
 * AccessoryDecoder.cpp
 *
 *  Created on: May 22, 2018
 *      Author: paulvdbergh
 */

#include "AccessoryDecoder.h"

#include "Accessory.h"

#include <cstring>
#include <sstream>
using namespace std;

namespace IoTT
{

	/**
	 *
	 * @param address
	 */
	AccessoryDecoder::AccessoryDecoder(const DCCAddress_t& address)
	:	Decoder(address)
	{
		for(uint8_t port = 0; port < 4; port++)
		{
			m_pAccessories[port] = new Accessory(this, port);
		}
	}

	/**
	 *
	 */
	AccessoryDecoder::~AccessoryDecoder()
	{
		for(auto item : m_pAccessories) delete item;
	}

	/**
	 *
	 * @param port
	 * @return
	 */
	const uint8_t& AccessoryDecoder::getState(const uint8_t& port)
	{
		return m_pAccessories[port]->getUDPState();
	}

	/**
	 *
	 * @param port
	 * @param outputNbr
	 * @param state
	 */
	void AccessoryDecoder::setDesiredState(const uint8_t& port, const uint8_t& outputNbr, const uint8_t& state)
	{
		stringstream ss;
		ss << m_BaseTopic << "Desired/" << (int)port << "/" << (int)outputNbr;
/*
		char szTopic[256];
		snprintf(szTopic, 256, "%sDesired/%i/%i", m_BaseTopic.c_str(), port, outputNbr);
		MqttPublisher(szTopic).Publish(state);
*/
		MqttPublisher(ss.str()).Publish(state);
	}

	/**
	 *
	 * @param topic
	 * @param payloadLen
	 * @param payload
	 */
	void AccessoryDecoder::onNewMqttData(const string& topic, const size_t payloadLen, const uint8_t* payload)
	{
		if(m_DCCAddress == atoi(topic.c_str()))
		{
			char* szAction = const_cast<char*>(strchr(topic.c_str(), '/'));
			if(szAction && *(++szAction))
			{
				if(0 == strncmp("Desired", szAction, strlen("Desired")))
				{
					char* szPort = strchr(szAction, '/');
					if(szPort && *(++szPort))
					{
						uint8_t port = atoi(szPort);
						char* szOutput = strchr(szPort, '/');
						if(szOutput && *(++szOutput))
						{
							uint8_t outputNbr = atoi(szOutput);

							uint8_t value = atoi((const char*)payload);

							m_pAccessories[port]->setState(outputNbr, value);
						}
					}
				}
			}
		}
	}

} /* namespace IoTT */

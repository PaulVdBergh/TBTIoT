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
 * LocDecoder.cpp
 *
 *  Created on: May 22, 2018
 *      Author: paulvdbergh
 */

#include "LocDecoder.h"

#include <sstream>
#include <cstring>

using namespace std;

namespace IoTT
{

#define LOCMODE_DCC 0
#define LOCMODE_MM	1

	/**
	 *
	 * @param address
	 */
	LocDecoder::LocDecoder(const DCCAddress_t& address)
	:	Decoder(address)
	,	m_SpeedPublisher(m_BaseTopic + "Speed")
	,	m_BusyPublisher(m_BaseTopic + "Busy")
	,	m_SpeedStepsPublisher(m_BaseTopic + "SpeedSteps")
	,	m_LocoDrive14Publisher(m_BaseTopic + "LocoDrive14")
	,	m_LocoDrive27Publisher(m_BaseTopic + "LocoDrive27")
	,	m_LocoDrive28Publisher(m_BaseTopic + "LocoDrive28")
	,	m_LocoDrive128Publisher(m_BaseTopic + "LocoDrive128")
	,	m_DirectionPublisher(m_BaseTopic + "Direction")
	,	m_DualTractionPublisher(m_BaseTopic + "DualTraction")
	,	m_SmartSearchPublisher(m_BaseTopic + "SmartSearch")
	,	m_FunctionGroup1Publisher(m_BaseTopic + "FunctionGroup1")
	,	m_FunctionGroup2Publisher(m_BaseTopic + "FunctionGroup2")
	,	m_FunctionGroup3Publisher(m_BaseTopic + "FunctionGroup3")
	,	m_FunctionGroup4Publisher(m_BaseTopic + "FunctionGroup4")
	,	m_FunctionGroup5Publisher(m_BaseTopic + "FunctionGroup5")
	,	m_LocMode(LOCMODE_DCC)
	{
		stringstream ss;
		ss << "IoTT/Decoders/" << m_DCCAddress << "/";
		m_BaseTopic = ss.str();

		m_LocInfo.Addr_MSB = (m_DCCAddress >> 8) & 0x3F;
		m_LocInfo.Addr_LSB = (m_DCCAddress & 0xFF);
		setSpeedSteps(4);
	}

	/**
	 *
	 */
	LocDecoder::~LocDecoder()
	{
		// TODO Auto-generated destructor stub
	}

	/**
	 *
	 * @param topic
	 * @param payloadLen
	 * @param payload
	 */
	void LocDecoder::onNewMqttData(const string& topic, const size_t payloadLen, const uint8_t* payload)
	{
		printf("LocDecoder::onNewMqttData : comparing LocAddress [%d] to data [%d]\n", m_DCCAddress, atoi(topic.c_str()));

		if(m_DCCAddress == atoi(topic.c_str()))
		{
			printf("\tComparison succeeded...\n");

			const char* szAttribute = strchr(topic.c_str(), '/');
			const char* szValue = (const char*)payload;

			printf("\tszAttribute = '%s'\n\tszValue = '%s'\n", szAttribute, szValue);

			if(		(szAttribute && *(++szAttribute))
				&&	(nullptr == strchr(szAttribute, '/'))
				&&	(szValue)
				&&	(*szValue))
			{
				if(0 == strcmp("Speed", szAttribute))
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					m_LocInfo.DB3 &= ~(0x7F);
					m_LocInfo.DB3 |= (atoi(szValue) & 0x7F);
					return;
				}
				else if(0 == strcmp("Direction", szAttribute))
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					strcmp("true", szValue) ? m_LocInfo.DB3 |= 0x80 : m_LocInfo.DB3 &= ~(0x80);
					return;
				}
				else if(0 == strcmp("Busy", szAttribute))
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					strcmp("true", szValue) ? m_LocInfo.DB2 &= ~(0x08) : m_LocInfo.DB2 |= 0x08;
					return;
				}
				else if(0 == strcmp("SpeedSteps", szAttribute))
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					m_LocInfo.DB2 &= ~(0x07);
					m_LocInfo.DB2 |= (0x07 & atoi(szValue));
					return;
				}
				else if(0 == strcmp("LocoDrive14", szAttribute)
					||	0 == strcmp("Locodrive27", szAttribute)
					||	0 == strcmp("LocoDrive28", szAttribute)
					||	0 == strcmp("LocoDrive128", szAttribute)
				  )
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					m_LocInfo.DB3 = atoi(szValue);
					return;
				}
				else if(0 == strcmp("DualTraction", szAttribute))
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					strcmp("true", szValue) ? m_LocInfo.DB4 &= ~(0x40) : m_LocInfo.DB4 |= 0x40;
					return;
				}
				else if(0 == strcmp("SmartSearch", szAttribute))
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					strcmp("true", szValue) ? m_LocInfo.DB4 &= ~(0x40) : m_LocInfo.DB4 |= 0x40;
					return;
				}
				else if(0 == strcmp("FunctionGroup1", szAttribute))
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					m_LocInfo.DB4 &= ~(0x1F);
					m_LocInfo.DB4 |= (0x1F & atoi(szValue));
					return;
				}
				else if(0 == strcmp("FunctionGroup2", szAttribute))
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					m_LocInfo.DB5 &= ~(0x0F);
					m_LocInfo.DB5 |= (0x0F & atoi(szValue));
					return;
				}
				else if(0 == strcmp("FunctionGroup3", szAttribute))
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					m_LocInfo.DB5 &= ~(0xF0);
					m_LocInfo.DB5 |= ((0xF0 & atoi(szValue)));
					return;
				}
				else if(0 == strcmp("FunctionGroup4", szAttribute))
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					m_LocInfo.DB6 = atoi(szValue);
					return;
				}
				else if(0 == strcmp("FunctionGroup5", szAttribute))
				{
					lock_guard<recursive_mutex> lock(m_MDecoder);
					m_LocInfo.DB7 = atoi(szValue);
					return;
				}
			}
		}
	}

	/**
	 *
	 * @param newMode
	 * @return
	 */
	uint8_t LocDecoder::setLocMode(const uint8_t& newMode)
	{
		if(256 > m_DCCAddress)
		{
			m_LocMode = newMode;
		}
		else
		{
			m_LocMode = LOCMODE_DCC;
		}
		return m_LocMode;
	}

	/**
	 *
	 * @param pMsg
	 */
	void LocDecoder::getLANLocInfo(uint8_t* pMsg)
	{
		lock_guard<recursive_mutex> guard(m_MDecoder);
		m_LocInfo.XOR = 0;
		for(uint8_t i = 4; i < (*((uint8_t*)&m_LocInfo.DataLen) - 1); i++)
		{
			m_LocInfo.XOR ^= ((uint8_t*)&m_LocInfo)[i];
		}
		memcpy(pMsg, &m_LocInfo, *((uint8_t*)&m_LocInfo.DataLen));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setSpeed(const uint8_t& newValue)
	{
		m_SpeedPublisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setBusy(const bool& newValue)
	{
		m_BusyPublisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setSpeedSteps(const uint8_t& newValue)
	{
		m_SpeedStepsPublisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setLocoDrive14(const uint8_t& newValue)
	{
		m_LocoDrive14Publisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setLocoDrive27(const uint8_t& newValue)
	{
		m_LocoDrive27Publisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setLocoDrive28(const uint8_t& newValue)
	{
		m_LocoDrive28Publisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setLocoDrive128(const uint8_t& newValue)
	{
		m_LocoDrive128Publisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setDirection(const bool& newValue)
	{
		m_DirectionPublisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setDualTraction(const bool& newValue)
	{
		m_DualTractionPublisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setSmartSearch(const bool& newValue)
	{
		m_SmartSearchPublisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setFunctionGroup1(const uint8_t& newValue)
	{
		m_FunctionGroup1Publisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setFunctionGroup2(const uint8_t& newValue)
	{
		m_FunctionGroup2Publisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setFunctionGroup3(const uint8_t& newValue)
	{
		m_FunctionGroup3Publisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setFunctionGroup4(const uint8_t& newValue)
	{
		m_FunctionGroup4Publisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setFunctionGroup5(const uint8_t& newValue)
	{
		m_FunctionGroup5Publisher.Publish(newValue);
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setLight(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x10 : getFunctionGroup1() & ~(0x10));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF0(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x10 : getFunctionGroup1() & ~(0x10));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF1(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x01 : getFunctionGroup1() & ~(0x01));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF2(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x02 : getFunctionGroup1() & ~(0x02));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF3(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x04 : getFunctionGroup1() & ~(0x04));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF4(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x08 : getFunctionGroup1() & ~(0x08));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF5(const bool& newValue)
	{
		setFunctionGroup2(newValue ? getFunctionGroup2() | 0x01 : getFunctionGroup2() & ~(0x01));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF6(const bool& newValue)
	{
		setFunctionGroup2(newValue ? getFunctionGroup2() | 0x02 : getFunctionGroup2() & ~(0x02));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF7(const bool& newValue)
	{
		setFunctionGroup2(newValue ? getFunctionGroup2() | 0x04 : getFunctionGroup2() & ~(0x04));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF8(const bool& newValue)
	{
		setFunctionGroup2(newValue ? getFunctionGroup2() | 0x08 : getFunctionGroup2() & ~(0x08));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF9(const bool& newValue)
	{
		setFunctionGroup3(newValue ? getFunctionGroup3() | 0x10 : getFunctionGroup3() & ~(0x10));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF10(const bool& newValue)
	{
		setFunctionGroup3(newValue ? getFunctionGroup3() | 0x20 : getFunctionGroup3() & ~(0x20));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF11(const bool& newValue)
	{
		setFunctionGroup3(newValue ? getFunctionGroup3() | 0x40 : getFunctionGroup3() & ~(0x40));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF12(const bool& newValue)
	{
		setFunctionGroup3(newValue ? getFunctionGroup3() | 0x80 : getFunctionGroup3() & ~(0x80));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF13(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x01 : getFunctionGroup4() & ~(0x01));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF14(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x02 : getFunctionGroup4() & ~(0x02));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF15(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x04 : getFunctionGroup4() & ~(0x04));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF16(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x08 : getFunctionGroup4() & ~(0x08));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF17(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x10 : getFunctionGroup4() & ~(0x10));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF18(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x20 : getFunctionGroup4() & ~(0x20));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF19(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x40 : getFunctionGroup4() & ~(0x40));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF20(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x80 : getFunctionGroup4() & ~(0x80));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF21(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x01 : getFunctionGroup5() & ~(0x01));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF22(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x02 : getFunctionGroup5() & ~(0x02));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF23(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x04 : getFunctionGroup5() & ~(0x04));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF24(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x08 : getFunctionGroup5() & ~(0x08));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF25(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x10 : getFunctionGroup5() & ~(0x10));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF26(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x20 : getFunctionGroup5() & ~(0x20));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF27(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x40 : getFunctionGroup5() & ~(0x40));
	}

	/**
	 *
	 * @param newValue
	 */
	void LocDecoder::setF28(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x80 : getFunctionGroup5() & ~(0x80));
	}

	/**
	 *
	 * @return
	 */
	uint8_t	LocDecoder::getSpeed(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3 & 0x7F;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getBusy(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB2 & 0x08;
	}

	/**
	 *
	 * @return
	 */
	uint8_t	LocDecoder::getSpeedSteps(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB2 & 0x07;
	}

	/**
	 *
	 * @return
	 */
	uint8_t	LocDecoder::getLocoDrive14(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3;
	}

	/**
	 *
	 * @return
	 */
	uint8_t	LocDecoder::getLocoDrive27(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3;
	}

	/**
	 *
	 * @return
	 */
	uint8_t	LocDecoder::getLocoDrive28(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3;
	}

	/**
	 *
	 * @return
	 */
	uint8_t	LocDecoder::getLocoDrive128(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getDirection(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3 & 0x80;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getDualTraction(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x40;
	}

	/**
	 *
	 * @return
	 */
	uint8_t	LocDecoder::getFunctionGroup1(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x1F;
	}

	/**
	 *
	 * @return
	 */
	uint8_t	LocDecoder::getFunctionGroup2(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x0F;
	}

	/**
	 *
	 * @return
	 */
	uint8_t	LocDecoder::getFunctionGroup3(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return (m_LocInfo.DB5 & 0xF0) >> 4;
	}

	/**
	 *
	 * @return
	 */
	uint8_t	LocDecoder::getFunctionGroup4(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6;
	}

	/**
	 *
	 * @return
	 */
	uint8_t	LocDecoder::getFunctionGroup5(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getLight(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x10;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF0(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x10;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF1(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x01;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF2(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x02;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF3(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x04;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF4(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x08;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF5(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x01;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF6(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x02;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF7(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x04;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF8(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x08;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF9(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x10;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF10(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x20;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF11(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x40;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF12(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x80;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF13(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x01;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF14(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x02;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF15(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x04;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF16(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x08;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF17(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x10;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF18(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x20;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF19(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x40;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF20(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x80;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF21(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x01;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF22(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x02;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF23(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x04;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF24(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x08;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF25(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x10;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF26(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x20;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF27(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x40;
	}

	/**
	 *
	 * @return
	 */
	bool LocDecoder::getF28(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x80;
	}

} /* namespace IoTT */

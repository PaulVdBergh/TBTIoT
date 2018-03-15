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
 * LocDecoder.cpp
 *
 *  Created on: Mar 11, 2018
 *      Author: paulvdbergh
 */

#include "LocDecoder.h"

#include <cstdio>
#include <cstring>

#include <esp_log.h>
static char tag[] = "LocDecoder";

namespace TBTIoT
{

#define LOCMODE_DCC	0
#define LOCMODE_MM	1

	LocDecoder::LocDecoder(const DCCAddress_t& address)
	:	Decoder(address)
	,	m_DCCState(0)
	,	m_LocMode(LOCMODE_DCC)
	{
		ESP_LOGI(tag, "LocDecoder(%i)", address);

		char szTopic[256];
		snprintf(szTopic, 256, "TBTIoT/Decoders/%i/", m_DCCAddress);
		m_BaseTopic = string(szTopic);

		m_LocInfo.Addr_MSB = (m_DCCAddress >> 8) & 0x3F;
		m_LocInfo.Addr_LSB = (m_DCCAddress & 0xFF);
		setSpeedSteps(4);
	}

	LocDecoder::~LocDecoder()
	{
	}

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

	bool LocDecoder::getNextDCCCommand(uint8_t* pBuffer)
	{
		switch(m_DCCState)
		{
			case 0:	// Speed Message
			{
//				m_DCCState++;
				return getDCCSpeedMessage(pBuffer);
			}

			default:
			{
				m_DCCState = 0;
			}
		}	//	switch(m_DCCState)
		return false;
	}

	uint8_t* LocDecoder::insertDCCAddress(uint8_t* pMsg)
	{
		uint8_t* pCurrent = &pMsg[1];
		if(m_DCCAddress < 128)
		{
			*pCurrent++ = m_DCCAddress & 0xFF;
		}
		else
		{
			*pCurrent++ = (m_DCCAddress >> 8);
			*pCurrent++ = m_DCCAddress & 0xFF;
		}
		return pCurrent;
	}

	bool LocDecoder::getDCCSpeedMessage(uint8_t* pBuffer)
	{
		uint8_t* pCurrent = insertDCCAddress(pBuffer);

		switch(getSpeedSteps())
		{
			case 0:
			case 1:
			{
				//	14 speed steps + light
				*pCurrent++ =	0x40
							|	(getDirection() ? 0x20 : 0x00)
							|	(getSpeed() & 0x0F)
							|	(getLight() ? 0x10 : 0x00);
				break;
			}

			case 2:
			{
				//	28 speed steps
				*pCurrent++ = 	0x40
							|	(getDirection() ? 0x20 : 0x00)
							|	((getSpeed() >> 1) & 0x0F)
							|	((getSpeed() & 0x01) ? 0x10 : 0x00);
				break;
			}

			case 4:
			{
				//	128 speed steps
				*pCurrent++ = 0x3F;
				*pCurrent++ =	(getSpeed() & 0x7F)
							|	(getDirection() ? 0x80 : 0x00);
			}
		}
		pBuffer[0] = (pCurrent - pBuffer);
		insertXOR(pBuffer);
		return true;
	}

	void LocDecoder::onNewMQTTData(const string& topic, const string& payload)
	{
		ESP_LOGI(tag, "onNewMQTTData(%s, %s)", topic.c_str(), payload.c_str());
		if(m_DCCAddress == atoi(topic.c_str()))
		{
			char* szAttribute = strchr(topic.c_str(), '/');
			if(szAttribute && *(++szAttribute))
			{
				if(nullptr == strchr(szAttribute, '/'))
				{
					const char* szValue = payload.c_str();
					ESP_LOGI(tag, "attribute = %s, value = %s", szAttribute, szValue);

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
					else
					{
						ESP_LOGW(tag, "Unknown attribute \"%s\" received.", szAttribute);
					}
				}
			}
		}
	}

	void LocDecoder::getLANLocInfo(uint8_t* pMsg)
	{
		lock_guard<recursive_mutex> guard(m_MDecoder);
		m_LocInfo.XOR = 0;
		for(uint8_t i = 4; i < *((uint8_t*)&m_LocInfo.DataLen); i++)
		{
			m_LocInfo.XOR ^= ((uint8_t*)&m_LocInfo)[i];
		}
		memcpy(pMsg, &m_LocInfo, *((uint8_t*)&m_LocInfo.DataLen));
	}

	void LocDecoder::setSpeed(const uint8_t& newValue)
	{
		MQTTPublisher(m_BaseTopic + "Speed").Publish(newValue);
	}

	void LocDecoder::setBusy(const bool& newValue)
	{
		MQTTPublisher(m_BaseTopic + "Busy").Publish(newValue);
	}

	void LocDecoder::setSpeedSteps(const uint8_t& newValue)
	{
		MQTTPublisher(m_BaseTopic + "SpeedSteps").Publish(newValue);
	}

	void LocDecoder::setLocoDrive14(const uint8_t& newValue)
	{
		MQTTPublisher(m_BaseTopic + "LocoDrive14").Publish(newValue);
	}

	void LocDecoder::setLocoDrive27(const uint8_t& newValue)
	{
		MQTTPublisher(m_BaseTopic + "LocoDrive27").Publish(newValue);
	}

	void LocDecoder::setLocoDrive28(const uint8_t& newValue)
	{
		MQTTPublisher(m_BaseTopic + "LocoDrive28").Publish(newValue);
	}

	void LocDecoder::setLocoDrive128(const uint8_t& newValue)
	{
		MQTTPublisher(m_BaseTopic + "LocoDrive128").Publish(newValue);
	}

	void LocDecoder::setDirection(const bool& newValue)
	{
		MQTTPublisher(m_BaseTopic + "Direction").Publish(newValue);
	}

	void LocDecoder::setDualTraction(const bool& newValue)
	{
		MQTTPublisher(m_BaseTopic + "DualTraction").Publish(newValue);
	}

	void LocDecoder::setSmartSearch(const bool& newValue)
	{
		MQTTPublisher(m_BaseTopic + "SmartSearch").Publish(newValue);
	}

	void LocDecoder::setFunctionGroup1(const uint8_t& newValue)
	{
		MQTTPublisher(m_BaseTopic + "FunctionGroup1").Publish(newValue);
	}

	void LocDecoder::setFunctionGroup2(const uint8_t& newValue)
	{
		MQTTPublisher(m_BaseTopic + "FunctionGroup2").Publish(newValue);
	}

	void LocDecoder::setFunctionGroup3(const uint8_t& newValue)
	{
		MQTTPublisher(m_BaseTopic + "FunctionGroup3").Publish(newValue);
	}

	void LocDecoder::setFunctionGroup4(const uint8_t& newValue)
	{
		MQTTPublisher(m_BaseTopic + "FunctionGroup4").Publish(newValue);
	}

	void LocDecoder::setFunctionGroup5(const uint8_t& newValue)
	{
		MQTTPublisher(m_BaseTopic + "FunctionGroup5").Publish(newValue);
	}

	void LocDecoder::setLight(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x10 : getFunctionGroup1() & ~(0x10));
	}

	void LocDecoder::setF0(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x10 : getFunctionGroup1() & ~(0x10));
	}

	void LocDecoder::setF1(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x01 : getFunctionGroup1() & ~(0x01));
	}

	void LocDecoder::setF2(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x02 : getFunctionGroup1() & ~(0x02));
	}

	void LocDecoder::setF3(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x04 : getFunctionGroup1() & ~(0x04));
	}

	void LocDecoder::setF4(const bool& newValue)
	{
		setFunctionGroup1(newValue ? getFunctionGroup1() | 0x08 : getFunctionGroup1() & ~(0x08));
	}

	void LocDecoder::setF5(const bool& newValue)
	{
		setFunctionGroup2(newValue ? getFunctionGroup2() | 0x01 : getFunctionGroup2() & ~(0x01));
	}

	void LocDecoder::setF6(const bool& newValue)
	{
		setFunctionGroup2(newValue ? getFunctionGroup2() | 0x02 : getFunctionGroup2() & ~(0x02));
	}

	void LocDecoder::setF7(const bool& newValue)
	{
		setFunctionGroup2(newValue ? getFunctionGroup2() | 0x04 : getFunctionGroup2() & ~(0x04));
	}

	void LocDecoder::setF8(const bool& newValue)
	{
		setFunctionGroup2(newValue ? getFunctionGroup2() | 0x08 : getFunctionGroup2() & ~(0x08));
	}

	void LocDecoder::setF9(const bool& newValue)
	{
		setFunctionGroup3(newValue ? getFunctionGroup3() | 0x10 : getFunctionGroup3() & ~(0x10));
	}

	void LocDecoder::setF10(const bool& newValue)
	{
		setFunctionGroup3(newValue ? getFunctionGroup3() | 0x20 : getFunctionGroup3() & ~(0x20));
	}

	void LocDecoder::setF11(const bool& newValue)
	{
		setFunctionGroup3(newValue ? getFunctionGroup3() | 0x40 : getFunctionGroup3() & ~(0x40));
	}

	void LocDecoder::setF12(const bool& newValue)
	{
		setFunctionGroup3(newValue ? getFunctionGroup3() | 0x80 : getFunctionGroup3() & ~(0x80));
	}

	void LocDecoder::setF13(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x01 : getFunctionGroup4() & ~(0x01));
	}

	void LocDecoder::setF14(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x02 : getFunctionGroup4() & ~(0x02));
	}

	void LocDecoder::setF15(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x04 : getFunctionGroup4() & ~(0x04));
	}

	void LocDecoder::setF16(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x08 : getFunctionGroup4() & ~(0x08));
	}

	void LocDecoder::setF17(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x10 : getFunctionGroup4() & ~(0x10));
	}

	void LocDecoder::setF18(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x20 : getFunctionGroup4() & ~(0x20));
	}

	void LocDecoder::setF19(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x40 : getFunctionGroup4() & ~(0x40));
	}

	void LocDecoder::setF20(const bool& newValue)
	{
		setFunctionGroup4(newValue ? getFunctionGroup4() | 0x80 : getFunctionGroup4() & ~(0x80));
	}

	void LocDecoder::setF21(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x01 : getFunctionGroup5() & ~(0x01));
	}

	void LocDecoder::setF22(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x02 : getFunctionGroup5() & ~(0x02));
	}

	void LocDecoder::setF23(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x04 : getFunctionGroup5() & ~(0x04));
	}

	void LocDecoder::setF24(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x08 : getFunctionGroup5() & ~(0x08));
	}

	void LocDecoder::setF25(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x10 : getFunctionGroup5() & ~(0x10));
	}

	void LocDecoder::setF26(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x20 : getFunctionGroup5() & ~(0x20));
	}

	void LocDecoder::setF27(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x40 : getFunctionGroup5() & ~(0x40));
	}

	void LocDecoder::setF28(const bool& newValue)
	{
		setFunctionGroup5(newValue ? getFunctionGroup5() | 0x80 : getFunctionGroup5() & ~(0x80));
	}

	uint8_t	LocDecoder::getSpeed(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3 & 0x7F;
	}

	bool LocDecoder::getBusy(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB2 & 0x08;
	}

	uint8_t	LocDecoder::getSpeedSteps(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB2 & 0x07;
	}

	uint8_t	LocDecoder::getLocoDrive14(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3;
	}

	uint8_t	LocDecoder::getLocoDrive27(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3;
	}

	uint8_t	LocDecoder::getLocoDrive28(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3;
	}

	uint8_t	LocDecoder::getLocoDrive128(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3;
	}

	bool LocDecoder::getDirection(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB3 & 0x80;
	}

	bool LocDecoder::getDualTraction(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x40;
	}

	uint8_t	LocDecoder::getFunctionGroup1(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x1F;
	}

	uint8_t	LocDecoder::getFunctionGroup2(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x0F;
	}

	uint8_t	LocDecoder::getFunctionGroup3(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return (m_LocInfo.DB5 & 0xF0) >> 4;
	}

	uint8_t	LocDecoder::getFunctionGroup4(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6;
	}

	uint8_t	LocDecoder::getFunctionGroup5(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7;
	}

	bool LocDecoder::getLight(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x10;
	}

	bool LocDecoder::getF0(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x10;
	}

	bool LocDecoder::getF1(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x01;
	}

	bool LocDecoder::getF2(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x02;
	}

	bool LocDecoder::getF3(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x04;
	}

	bool LocDecoder::getF4(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB4 & 0x08;
	}

	bool LocDecoder::getF5(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x01;
	}

	bool LocDecoder::getF6(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x02;
	}

	bool LocDecoder::getF7(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x04;
	}

	bool LocDecoder::getF8(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x08;
	}

	bool LocDecoder::getF9(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x10;
	}

	bool LocDecoder::getF10(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x20;
	}

	bool LocDecoder::getF11(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x40;
	}

	bool LocDecoder::getF12(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB5 & 0x80;
	}

	bool LocDecoder::getF13(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x01;
	}

	bool LocDecoder::getF14(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x02;
	}

	bool LocDecoder::getF15(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x04;
	}

	bool LocDecoder::getF16(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x08;
	}

	bool LocDecoder::getF17(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x10;
	}

	bool LocDecoder::getF18(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x20;
	}

	bool LocDecoder::getF19(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x40;
	}

	bool LocDecoder::getF20(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB6 & 0x80;
	}

	bool LocDecoder::getF21(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x01;
	}

	bool LocDecoder::getF22(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x02;
	}

	bool LocDecoder::getF23(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x04;
	}

	bool LocDecoder::getF24(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x08;
	}

	bool LocDecoder::getF25(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x10;
	}

	bool LocDecoder::getF26(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x20;
	}

	bool LocDecoder::getF27(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x40;
	}

	bool LocDecoder::getF28(void)
	{
		lock_guard<recursive_mutex> lock(m_MDecoder);
		return m_LocInfo.DB7 & 0x80;
	}

} /* namespace TBTIoT */

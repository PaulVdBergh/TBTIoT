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
 * Accessory.cpp
 *
 *  Created on: Mar 16, 2018
 *      Author: paulvdbergh
 */

#include "Accessory.h"

namespace TBTIoT
{

#define ACCESSORYREPEATCOUNT 5

	Accessory::Accessory(AccessoryDecoder* pAccessoryDecoder, uint8_t port)
	:	m_pAccessoryDecoder(pAccessoryDecoder)
	,	m_Port(port)
	,	m_CurrentState{0, 0}
	,	m_DesiredState{0, 0}
	,	m_UDPState(0)

	{
		// TODO Auto-generated constructor stub

	}

	Accessory::~Accessory()
	{
	// TODO Auto-generated destructor stub
	}

	uint8_t Accessory::getUDPState()
	{
		return m_UDPState;
	}

	void Accessory::setUDPState(const uint8_t newState)
	{
		if(m_UDPState != newState)
		{
/* TODO MQTTPublisher
			m_UDPState = newState;
			m_pAccessoryDecoder->
*/
		}
	}

	bool Accessory::getDCCMessage(uint8_t* pBuffer)
	{
		bool retval = false;
		pBuffer[0] = 4;
		pBuffer[1] = 0x80 | (m_pAccessoryDecoder->getDCCAddress() & 0x003F);
		pBuffer[2] = 0x80 | ((~(m_pAccessoryDecoder->getDCCAddress() & 0x01C0) >> 2) & 0x70) | ((m_Port & 0x03) << 1);
		if((m_DesiredState[0] == 0) && (m_CurrentState[0] != 0))
		{
			retval = true;
			m_CurrentState[0]--;
		}
		else if((m_DesiredState[1] == 0) && (m_CurrentState[1] != 0))
		{
			pBuffer[2] |= 0x01;
			retval = true;
			m_CurrentState[1]--;

		}
		else if((m_DesiredState[0] == ACCESSORYREPEATCOUNT) && (m_CurrentState[0] != ACCESSORYREPEATCOUNT))
		{
			pBuffer[2] |= 0x08;
			retval = true;
			if(++m_CurrentState[0] == ACCESSORYREPEATCOUNT)
			{
				setUDPState(1);
			}
		}
		else if((m_DesiredState[1] == ACCESSORYREPEATCOUNT) && (m_CurrentState[1] != ACCESSORYREPEATCOUNT))
		{
			pBuffer[2] |= 0x09 ;
			retval = true;
			if(++m_CurrentState[1] == ACCESSORYREPEATCOUNT)
			{
				setUDPState(2);
			}
		}

		pBuffer[3] = pBuffer[1] ^ pBuffer[2];

return retval;
	}

} /* namespace TBTIoT */

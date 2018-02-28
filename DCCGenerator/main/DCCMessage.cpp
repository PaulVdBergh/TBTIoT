/*
 * Copyright (C) 2017 Paul Van den Bergh <admin@paulvandenbergh.be>
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
 *	For more info see http://paulvandenbergh.be/TBT
 * =====================================================================
 */

/*
 * DCCMessage.cpp
 *
 *  Created on: Feb 26, 2018
 *      Author: paulvdbergh
 */

#include "DCCMessage.h"

#include <cstring>

namespace TBTIoT
{

	DCCMessage::DCCMessage(const uint8_t* pmsg)
	:	m_pItems(nullptr)
	,	m_itemCount(0)
	{
		if(!sm_bInitialized)
		{
			(const_cast<rmt_item32_t*>(&DCC_ZERO_BIT))->duration0 = ZERO_PULSE_LENGTH;
			(const_cast<rmt_item32_t*>(&DCC_ZERO_BIT))->duration1 = ZERO_PULSE_LENGTH;
			(const_cast<rmt_item32_t*>(&DCC_ZERO_BIT))->level0 = 1;
			(const_cast<rmt_item32_t*>(&DCC_ZERO_BIT))->level1 = 0;

			(const_cast<rmt_item32_t*>(&DCC_ONE_BIT))->duration0 = ONE_PULSE_LENGTH;
			(const_cast<rmt_item32_t*>(&DCC_ONE_BIT))->duration1 = ONE_PULSE_LENGTH;
			(const_cast<rmt_item32_t*>(&DCC_ONE_BIT))->level0 = 1;
			(const_cast<rmt_item32_t*>(&DCC_ONE_BIT))->level1 = 0;
		}

		if(pmsg && pmsg[0])
		{
			m_itemCount = ((pmsg[0]) * 9) + 1;
			rmt_item32_t* p = m_pItems = new rmt_item32_t[m_itemCount];

			for(uint8_t i = 1; i < (pmsg[0] + 1); i++)
			{
				*p++ = DCC_ZERO_BIT;
				for(uint8_t j = 0x80; j; j >>= 1)
				{
					*p++ = (pmsg[i] & j) ? DCC_ONE_BIT : DCC_ZERO_BIT;
				}
			}
			*p = DCC_ONE_BIT;
		}
		else
		{
			throw("DCCMessage::DCCMessage");
		}
	}

	DCCMessage::~DCCMessage()
	{
		delete [] m_pItems;
	}

	const rmt_item32_t DCCMessage::DCC_ZERO_BIT;
	const rmt_item32_t DCCMessage::DCC_ONE_BIT;
	bool DCCMessage::sm_bInitialized = false;

} /* namespace TBTIoT */

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

	DCCMessage::DCCMessage(uint8_t* pmsg)
	:	m_pMsg(nullptr)
	{
		if(pmsg[0] > 0)
		{
			m_pMsg = new uint8_t[pmsg[0]];
			memcpy(m_pMsg, pmsg, pmsg[0]);
		}
		else
		{
			throw("DCCMessage::DCCMessage");
		}
	}

	DCCMessage::~DCCMessage()
	{
		delete [] m_pMsg;
	}

	const uint8_t& DCCMessage::operator[](int x)
	{
		if(m_pMsg && (x < m_pMsg[0]))
		{
			return m_pMsg[x];
		}
		throw("DCCMessage::operator[]");
	}

} /* namespace TBTIoT */

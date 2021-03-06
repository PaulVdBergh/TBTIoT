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
 * Decoder.cpp
 *
 *  Created on: Mar 10, 2018
 *      Author: paulvdbergh
 */

#include "Decoder.h"

#include <cstdio>
using namespace std;

namespace TBTIoT
{

	Decoder::Decoder(const DCCAddress_t& address)
	:	m_DCCAddress(address)
	{
	// TODO Auto-generated constructor stub

		char szTopic[256];
		snprintf(szTopic, 256, "IoTT/Decoders/%i/", m_DCCAddress);
		m_BaseTopic = string(szTopic);
	}

	Decoder::~Decoder()
	{
	// TODO Auto-generated destructor stub
	}

	void Decoder::insertXOR(uint8_t* pMsg)
	{
		uint8_t* pXOR = pMsg + (pMsg[0]);

		*pXOR = 0;
		for(uint8_t* pIndex = &pMsg[1]; pIndex < pXOR; pIndex++)
		{
			*pXOR ^= *pIndex;
		}
	}

} /* namespace TBTIoT */

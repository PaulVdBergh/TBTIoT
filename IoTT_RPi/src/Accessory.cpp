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
 * Accessory.cpp
 *
 *  Created on: May 22, 2018
 *      Author: paulvdbergh
 */

#include "Accessory.h"

namespace IoTT
{

	Accessory::Accessory(AccessoryDecoder* pDecoder, const uint8_t& port)
	:	m_pAccessoryDecoder(pDecoder)
	,	m_Port(port)
	,	m_UDPState(0)
	,	m_CurrentState{0, 0}
	,	m_DesiredState{0, 0}
	{
	// TODO Auto-generated constructor stub
	}

	Accessory::~Accessory()
	{
	// TODO Auto-generated destructor stub
	}

	void Accessory::setState(const uint8_t& outputNbr, const uint8_t& state)
	{
		m_DesiredState[outputNbr] = state;
	}

} /* namespace IoTT */

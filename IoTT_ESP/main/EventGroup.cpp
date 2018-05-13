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
 * EventGroup.cpp
 *
 *  Created on: May 11, 2018
 *      Author: paulvdbergh
 */

#include "EventGroup.h"

namespace IoTT
{

	EventGroup::EventGroup()
	:	m_Handle(xEventGroupCreate())
	{

	}

	EventGroup::~EventGroup()
	{
		vEventGroupDelete(m_Handle);
	}

	EventBits_t EventGroup::Wait(
			const EventBits_t BitsToWaitFor,
			const bool ClearOnExit,
			const bool WaitForAllBits,
			TickType_t TicksToWait)
	{
		return xEventGroupWaitBits(
				m_Handle,
				BitsToWaitFor,
				ClearOnExit ? pdTRUE : pdFALSE,
				WaitForAllBits ? pdTRUE : pdFALSE,
				TicksToWait);
	}

	EventBits_t EventGroup::Set(const EventBits_t BitsToSet)
	{
		return xEventGroupSetBits(m_Handle, BitsToSet);
	}

	EventBits_t EventGroup::Clear(const EventBits_t BitsToClear)
	{
		return xEventGroupClearBits(m_Handle, BitsToClear);
	}

	EventBits_t EventGroup::Get()
	{
		return xEventGroupGetBits(m_Handle);
	}

	EventBits_t EventGroup::Sync(
			const EventBits_t BitsToSet,
			const EventBits_t BitsToWaitFor,
			TickType_t TicksToWait)
	{
		return xEventGroupSync(m_Handle, BitsToSet, BitsToWaitFor, TicksToWait);
	}

} /* namespace IoTT */

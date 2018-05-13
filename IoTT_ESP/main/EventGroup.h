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
 * EventGroup.h
 *
 *  Created on: May 11, 2018
 *      Author: paulvdbergh
 */

#ifndef MAIN_EVENTGROUP_H_
#define MAIN_EVENTGROUP_H_

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#define BITPOS(x) (1 << (x))

namespace IoTT
{

	/**
	 *
	 */
	class EventGroup
	{
		public:
			/**
			 * EventGroup Constructor
			 */
			EventGroup();

			/**
			 * EventGroup Destructor
			 */
			virtual ~EventGroup();

			/**
			 *  Read bits within an RTOS event group, optionally entering the Blocked state
			 *  (with a timeout) to wait for a bit or group of bits to become set.
			 *
			 *  This function cannot be called from an interrupt.
			 *
			 * @param BitsToWaitFor	: A bitwise value that indicates the bit or bits to
			 * test inside the event group. For example, to wait for bit 0 and/or
			 * bit 2 set BitsToWaitFor to 0x05.
			 * To wait for bits 0 and/or bit 1 and/or bit 2 set uxBitsToWaitFor to 0x07. Etc.
			 * BitsToWaitFor must not be set to 0.
			 *
			 * @param ClearOnExit	: If ClearOnExit is set to true then any bits set in the
			 * value passed as the BitsToWaitFor parameter will be cleared in the event group
			 * before Wait() returns if Wait() returns for any reason other than a timeout.
			 * The timeout value is set by the TicksToWait parameter.
			 * If ClearOnExit is set to false then the bits set in the event group are not altered
			 * when the call to Wait() returns.
			 *
			 * @param WaitForAllBits	: WaitForAllBits is used to create either a logical AND test
			 * (where all bits must be set) or a logical OR test (where one or more bits must be set)
			 * as follows:
			 * 	- If WaitForAllBits is set to true then Wait() will return when either
			 * 	all the bits set in the value passed as the BitsToWaitFor parameter are set in the
			 * 	event group or the specified block time expires.
			 * 	- If WaitForAllBits is set to false then Wait() will return when any
			 * 	of the bits set in the value passed as the BitsToWaitFor parameter are set in the
			 * 	event group or the specified block time expires.
			 *
			 * @param TicksToWait	: The maximum amount of time (specified in 'ticks') to wait for
			 * one/all (depending on the WaitForAllBits value) of the bits specified by BitsToWaitFor
			 * to become set.
			 *
			 * @return	: The value of the event group at the time either the event bits being waited
			 * for became set, or the block time expired. The current value of the event bits in an
			 * event group will be different to the returned value if a higher priority task or interrupt
			 * changed the value of an event bit between the calling task leaving the Blocked state and
			 * exiting the Wait() function.
			 * Test the return value to know which bits were set. If Wait() returned because its timeout
			 * expired then not all the bits being waited for will be set. If Wait() returned because
			 * the bits it was waiting for were set then the returned value is the event group value before
			 * any bits were automatically cleared because the ClearOnExit parameter was set to true.
			 */
			EventBits_t Wait(
					const EventBits_t BitsToWaitFor,
					const bool ClearOnExit = false,
					const bool WaitForAllBits = false,
					TickType_t TicksToWait = portMAX_DELAY);
			EventBits_t	Set(const EventBits_t BitsToSet);
			EventBits_t	Clear(const EventBits_t BitsToClear);
			EventBits_t	Get(void);
			EventBits_t Sync(
					const EventBits_t BitsToSet,
					const EventBits_t BitsToWaitFor,
					TickType_t TicksToWait = portMAX_DELAY);

		protected:
			EventGroupHandle_t	m_Handle;

		private:

	};

} /* namespace IoTT */

#endif /* MAIN_EVENTGROUP_H_ */

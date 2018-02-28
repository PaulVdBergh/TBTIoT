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
 * DCCMessage.h
 *
 *  Created on: Feb 26, 2018
 *      Author: paulvdbergh
 */

#ifndef MAIN_DCCMESSAGE_H_
#define MAIN_DCCMESSAGE_H_

#include "driver/rmt.h"
#include <stdint.h>

namespace TBTIoT
{
#define ZERO_PULSE_LENGTH (8640)
#define ONE_PULSE_LENGTH (4640)

#define PREAMBLE_NBR_CYCLES 16
#define PREAMBLE_WAIT_TIME ((TickType_t)1000000000)

	class DCCMessage
	{
		public:
			DCCMessage(const uint8_t* pmsg);
			virtual ~DCCMessage();

			rmt_item32_t* 	getItems(void) { return m_pItems; }
			uint16_t		getItemCount(void) { return m_itemCount; }

			static const rmt_item32_t	DCC_ZERO_BIT;
			static const rmt_item32_t	DCC_ONE_BIT;

		protected:
			rmt_item32_t*	m_pItems;
			uint16_t		m_itemCount;

			static bool		sm_bInitialized;

		private:

	};

} /* namespace TBTIoT */

#endif /* MAIN_DCCMESSAGE_H_ */

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

#include <stdint.h>

namespace TBTIoT
{

	class DCCMessage
	{
		public:
			DCCMessage(uint8_t* pmsg);
			virtual ~DCCMessage();

			const uint8_t& operator[](int x);

		protected:
			uint8_t*		m_pMsg;

		private:

	};

} /* namespace TBTIoT */

#endif /* MAIN_DCCMESSAGE_H_ */

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
 * Accessory.h
 *
 *  Created on: Mar 16, 2018
 *      Author: paulvdbergh
 */

#ifndef DECODERS_SRC_ACCESSORY_H_
#define DECODERS_SRC_ACCESSORY_H_

#include "AccessoryDecoder.h"

namespace TBTIoT
{

	class Accessory
	{
		public:
			Accessory(AccessoryDecoder* pAccessoryDecoder, const uint8_t port);
			virtual ~Accessory();

			uint8_t getUDPState(void);
			void	setUDPState(const uint8_t newState);
			bool	getDCCMessage(uint8_t* pBuffer);
			void	setState(const uint8_t outputNbr, const uint8_t state);

			AccessoryDecoder* getDecoder(void) { return m_pAccessoryDecoder; }
			const uint8_t getPort(void) { return m_Port; }

		protected:

		private:
			AccessoryDecoder*	m_pAccessoryDecoder;
			uint8_t				m_Port;
			uint8_t				m_CurrentState[2];
			uint8_t				m_DesiredState[2];
			uint8_t				m_UDPState;

	};

} /* namespace TBTIoT */

#endif /* DECODERS_SRC_ACCESSORY_H_ */

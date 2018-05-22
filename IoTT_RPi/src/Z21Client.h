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
 * Z21Client.h
 *
 *  Created on: May 22, 2018
 *      Author: paulvdbergh
 */

#ifndef SRC_Z21CLIENT_H_
#define SRC_Z21CLIENT_H_

#include "Z21Interface.h"
#include "LocDecoder.h"

namespace IoTT
{

	class Z21Client
	{
		public:
			Z21Client(Z21Interface* pinterface, const sockaddr_in& address);
			virtual ~Z21Client();

			void	broadcastPowerStateChange(const bool& newState);
			void	broadcastLocInfoChanged(LocDecoder* pLoc);
//			void	broadcastAccessoryInfoChanged(Accessory* pAccessory);
			void	broadcastEmergencyStop(void);
			void	broadcastOvercurrent(void);

			const sockaddr_in&	getAddress(void) { return m_Address; }
			const uint32_t&		getBroadcastFlags(void);
			void				setBroadcastFlags(uint32_t& newFlags) { m_BroadcastFlags = newFlags; }

		protected:

		private:
			Z21Interface*		m_pInterface;
			const sockaddr_in	m_Address;
			uint32_t			m_BroadcastFlags;

	};

} /* namespace IoTT */

#endif /* SRC_Z21CLIENT_H_ */

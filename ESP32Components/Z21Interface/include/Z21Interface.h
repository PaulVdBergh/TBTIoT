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
 * Z21Interface.h
 *
 *  Created on: Feb 28, 2018
 *      Author: paulvdbergh
 */

#ifndef MAIN_Z21INTERFACE_H_
#define MAIN_Z21INTERFACE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
	void* 	addZ21Interface(uint16_t port);
	void	removeZ21Interface(void* interface);
#ifdef __cplusplus
}

#include "lwip/sockets.h"

#include "esp_log.h"
#include <vector>
#include <mutex>
#include <thread>
using namespace std;

#include "LocDecoder.h"

namespace TBTIoT
{
	class Z21Client;	//	forward declaration

	/**
	 * Class Z21Interface represents the interface for UDP clients
	 * using the Z21 Lan Protocol.
	 *
	 * # Basics
	 * ## Communication:
	 * Communication with the Z21 protocol over UDP port 21105 or 21106.
	 * Control applications on the client (PC, App, ...) should primarily use port 21105.
	 * The communication is always asynchronous, i.e. between a request and the
	 * corresponding response other broadcast messages can occur.
	 *
	 * It is expected that each client communicates with the Z21 once per minute, otherwise
	 * it will be removed from the list of active participants. If possible, a client
	 * should log off when exiting with the LAN_LOGOFF command.
	 *
	 * ## Z21 Datagram layout
	 * A Z21 datagram, i.e. a request or response is structured as follows:
	 *
	 <table>
	 <caption>Z21 Datagram Layout</caption>
	 <tr><th>DataLen<th>Header<th>Data
	 <tr><td>2 Bytes<td>2 Bytes<td>n Bytes
	 </table>
	 *	- <b>DataLen</b> (little endian):
	 *
	 *		Total length over the entire data set including DataLen, Header and Data,
	 *		i.e. DataLen = 2 + 2 + n.
	 *	- <b>Header</b> (little endian):
	 *
	 *		Describes the command or protocol group.
	 *	- <b>Data</b>:
	 *
	 *		Structure and length depend on command. Exact description see respective command.
	 *
	 * Unless otherwise stated, the byte order is little-endian, i.e. first the low byte, then
	 * the high byte.
	 *
	 */
	class Z21Interface
	{
		public:
			///	Z21Interface constructor
			Z21Interface(in_port_t port = 21105);

			///	Z21Interface destructor
			virtual ~Z21Interface();

			ssize_t sendToSocket(const uint8_t* pMsg, sockaddr* address);

			void	broadcastPowerStateChange(const bool& newState);
			void	broadcastLocInfoChanged(LocDecoder* pLoc);
			void	broadcastEmergencyStop(void);
			void	broadcastOvercurrent(void);

		protected:
			Z21Client*	findClient(const sockaddr_in& address);
			bool		removeClient(Z21Client* pClient);

			Decoder*	findDecoder(DCCAddress_t address) { return nullptr; }

		private:
			void			threadFunc(void);

			in_port_t			m_port;
			thread				m_thread;
			int					m_fdStop;
			int					m_fdsock_me;
			recursive_mutex		m_Mfdsock_me;
			sockaddr_in			m_sockaddr_me;
			vector<Z21Client*>	m_Clients;
			recursive_mutex		m_MClients;
	};

} /* namespace TBTIoT */

#endif // __cplusplus

#endif /* MAIN_Z21INTERFACE_H_ */

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

namespace TBTIoT
{
	class Z21Client;	//	forward declaration
	class Z21Interface
	{
		public:
			Z21Interface(in_port_t port = 21105);
			virtual ~Z21Interface();

			ssize_t sendToSocket(const uint8_t* pMsg, sockaddr* address);

		protected:

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

			static const char*	m_TAG;
	};

} /* namespace TBTIoT */

#endif // __cplusplus

#endif /* MAIN_Z21INTERFACE_H_ */

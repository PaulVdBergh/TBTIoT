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
 * TBTIoTWifi.h
 *
 *  Created on: Apr 4, 2018
 *      Author: paulvdbergh
 */

#ifndef MAIN_TBTIOTWIFI_H_
#define MAIN_TBTIOTWIFI_H_

#include "esp_event.h"
#include "esp_event_loop.h"
#include <esp_wifi.h>

#include <string>
using namespace std;

extern "C"
esp_err_t TBTIoTWifi_event_handler(void* ctx, system_event_t* event);

namespace TBTIoT
{

	class TBTIoTWifi
	{
		public:
			static TBTIoTWifi* getInstance(void);
			virtual ~TBTIoTWifi();

			void Connect(const string ssid, const string password);
			void Scan(void);

			bool getAutoConnect(void) { return m_bAutoConnect; }

		protected:
			TBTIoTWifi();

		private:
			bool	m_bAutoConnect;

			static TBTIoTWifi*	sm_pInstance;

	};

} /* namespace TBTIoT */

#endif /* MAIN_TBTIOTWIFI_H_ */

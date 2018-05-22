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
 * Decoder.h
 *
 *  Created on: May 21, 2018
 *      Author: paulvdbergh
 */

#ifndef SRC_DECODER_H_
#define SRC_DECODER_H_

#include <stdint.h>
#include <mutex>
#include <string>

using namespace std;

namespace IoTT
{
	typedef uint16_t DCCAddress_t;

	class Decoder
	{
		public:
			Decoder(const DCCAddress_t& address);
			virtual ~Decoder();

			const DCCAddress_t&	getDCCAddress(void) { return m_DCCAddress; }

			virtual void onNewMqttData(const string& topic, const size_t payloadLen, const uint8_t* payload) = 0;

		protected:
			const DCCAddress_t	m_DCCAddress;
			string				m_BaseTopic;
			recursive_mutex		m_MDecoder;

		private:

	};

} /* namespace IoTT */

#endif /* SRC_DECODER_H_ */

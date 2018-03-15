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
 * Decoder.h
 *
 *  Created on: Mar 10, 2018
 *      Author: paulvdbergh
 */

#ifndef DECODERS_SRC_DECODER_H_
#define DECODERS_SRC_DECODER_H_

#include <stdint.h>

#include <mutex>
#include <string>
using namespace std;

namespace TBTIoT
{
	typedef uint16_t DCCAddress_t;

	class Decoder
	{
		public:
			Decoder(const DCCAddress_t& address);
			virtual ~Decoder();

			const DCCAddress_t&	getDCCAddress(void) { return m_DCCAddress; }

			virtual bool getNextDCCCommand(uint8_t* pBuffer) = 0;
			virtual void onNewMQTTData(const string& topic, const string& payload) = 0;

		protected:
			void insertXOR(uint8_t* pMsg);

			uint8_t				m_DCCCommandBuffer[64];
			const DCCAddress_t	m_DCCAddress;
			recursive_mutex		m_MDecoder;

		private:

	};

} /* namespace TBTIoT */

#endif /* DECODERS_SRC_DECODER_H_ */

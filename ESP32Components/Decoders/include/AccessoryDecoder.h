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
 * AccessoryDecoder.h
 *
 *  Created on: Mar 11, 2018
 *      Author: paulvdbergh
 */

#ifndef DECODERS_INCLUDE_ACCESSORYDECODER_H_
#define DECODERS_INCLUDE_ACCESSORYDECODER_H_

#include "Decoder.h"

namespace TBTIoT
{

	class AccessoryDecoder: public Decoder
	{
		public:
			AccessoryDecoder(const DCCAddress_t& address);
			virtual ~AccessoryDecoder();

			virtual void onNewMQTTData(const string& topic, const string& payload);

		protected:

		private:

	};

} /* namespace TBTIoT */

#endif /* DECODERS_INCLUDE_ACCESSORYDECODER_H_ */

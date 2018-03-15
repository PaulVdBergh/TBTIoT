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
 * DCCDecoders.h
 *
 *  Created on: Mar 10, 2018
 *      Author: paulvdbergh
 */

#ifndef DECODERS_INCLUDE_DECODERS_H_
#define DECODERS_INCLUDE_DECODERS_H_

#include "MQTTSubscription.h"
#include "../src/Decoder.h"

#include <map>
#include <mutex>
#include <string>
using namespace std;

#ifndef DCCDECODERS_TOPICBASEPATH
#define DCCDECODERS_TOPICBASEPATH "TBTIoT/Decoders/"
#endif

namespace TBTIoT
{

	typedef map<DCCAddress_t, Decoder*> DecodersMap_t;
	typedef DecodersMap_t::iterator		DecodersMapIterator_t;

	class Decoders : public MQTTSubscription
	{
		public:
			static Decoders* getInstance();

			virtual ~Decoders();

			Decoder* getNextDecoder(Decoder* currentDecoder);

			Decoder* getDecoder(const DCCAddress_t& address);

		protected:
			Decoders(const string& MQTTTopicBasePath = DCCDECODERS_TOPICBASEPATH);

			virtual void OnNewData(IoTMQTTMessageQueueItem* pItem);

		private:
			string				m_TopicBasePath;

			static Decoders*	sm_Instance;

			DecodersMap_t		m_Decoders;
			recursive_mutex		m_MDecoders;
	};

} /* namespace TBTIoT */

#endif /* DECODERS_INCLUDE_DECODERS_H_ */

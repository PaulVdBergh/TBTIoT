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
 * Decoders.h
 *
 *  Created on: May 21, 2018
 *      Author: paulvdbergh
 */

#ifndef SRC_DECODERS_H_
#define SRC_DECODERS_H_

#include "MqttSubscription.h"

#include "Decoder.h"

#include <map>
#include <mutex>
using namespace std;

namespace IoTT
{

	typedef map<DCCAddress_t, Decoder*> DecodersMap_t;
	typedef DecodersMap_t::iterator		DecodersMapIterator_t;

	class Decoders: public virtual MqttSubscription
	{
		public:
			static Decoders* getInstance();
			virtual ~Decoders();

			Decoder* getDecoder(const DCCAddress_t& address);

			virtual void OnNewData(const mqttMessageQueueItem& item) override;

		protected:
			Decoders(const string& MqttTopicBasePath = "IoTT/Decoders/");

		private:
			static Decoders*	sm_pInstance;
			string				m_TopicBasePath;
			DecodersMap_t		m_Decoders;
			recursive_mutex		m_MDecoders;
	};

} /* namespace IoTT */

#endif /* SRC_DECODERS_H_ */

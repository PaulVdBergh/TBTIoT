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
 * mqttactionListener.h
 *
 *  Created on: May 19, 2018
 *      Author: paulvdbergh
 */

#ifndef SRC_MQTTACTIONLISTENER_H_
#define SRC_MQTTACTIONLISTENER_H_

#include <cstring>
#include <mqtt/async_client.h>

namespace IoTT
{

	class mqtt_actionListener: public virtual mqtt::iaction_listener
	{
		public:
			mqtt_actionListener(const std::string& name);

		protected:

		private:
			void on_failure(const mqtt::token& tok) override;
			void on_success(const mqtt::token& tok) override;

			std::string m_name;
	};

} /* namespace IoTT */

#endif /* SRC_MQTTACTIONLISTENER_H_ */

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
 * mqttcallback.h
 *
 *  Created on: May 19, 2018
 *      Author: paulvdbergh
 */

#ifndef MQTTCALLBACK_H_
#define MQTTCALLBACK_H_

#include <mqtt/async_client.h>

#include "mqttactionListener.h"

namespace IoTT
{

	class mqtt_callback: public virtual mqtt::callback, public virtual mqtt::iaction_listener
	{
		public:
			mqtt_callback(mqtt::async_client& cli, mqtt::connect_options& connOpts);

		protected:

		private:
			void reconnect(void);
			void on_failure(const mqtt::token& tok);
			void on_success(const mqtt::token& tok);
			void connection_lost(const std::string& cause) override;
			void message_arrived(mqtt::const_message_ptr msg) override;
			void delivery_complete(mqtt::delivery_token_ptr token) override;

			int		m_nretry;
			mqtt::async_client& m_cli;
			mqtt::connect_options& m_connOpts;
			mqtt_actionListener	m_subListener;
	};

} /* namespace IoTT */

#endif /* MQTTCALLBACK_H_ */

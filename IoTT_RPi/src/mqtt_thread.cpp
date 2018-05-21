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
 * mqtt_thread.cpp
 *
 *  Created on: May 16, 2018
 *      Author: paulvdbergh
 */

#include "mqtt_thread.h"

#include "mqttSettings.h"

#include <cstdlib>
#include <cstring>
#include <thread>
#include <chrono>
#include <mqtt/async_client.h>

#include "mqttcallback.h"



void* mqtt_thread_func(void*)
{
	mqtt::connect_options connOpts;
	connOpts.set_keep_alive_interval(20);
	connOpts.set_clean_session(true);

	mqtt::async_client client(MQTT_ADDRESS, MQTT_CLIENTID);

	IoTT::mqtt_callback cb(client, connOpts);
	client.set_callback(cb);

	try
	{
		connect_token = client.connect(connOpts, nullptr, cb);
	}
	catch(const mqtt::exception&)
	{

		std::cerr << "\nERROR: Unable to connect to MQTT server: '"
			<< MQTT_ADDRESS << "'" << std::endl;
		//	TODO recovery
	}

	while(true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(2500));
	}

	return nullptr;
}

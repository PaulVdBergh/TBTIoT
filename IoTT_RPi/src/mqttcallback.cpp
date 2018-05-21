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
 * mqttcallback.cpp
 *
 *  Created on: May 19, 2018
 *      Author: paulvdbergh
 */

#include "mqttcallback.h"

#include "mqttSettings.h"

namespace IoTT
{

	mqtt_callback::mqtt_callback(mqtt::async_client& cli, mqtt::connect_options& connOpts)
	:	m_nretry(0)
	,	m_cli(cli)
	,	m_connOpts(connOpts)
	,	m_subListener("Subscription")
	{

	}

	void mqtt_callback::reconnect()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(2500));
		try
		{
			m_cli.connect(m_connOpts, nullptr, *this);
		}
		catch (const mqtt::exception& exc)
		{
			std::cerr << "Error : " << exc.what() << std::endl;
			exit(1);
		}
	}

	void mqtt_callback::on_failure(const mqtt::token& tok)
	{
		std::cout << "Connection failed" << std::endl;
		if(++m_nretry > MQTT_RETRY_ATTEMPTS)
		{
			exit(1);
		}
		reconnect();
	}

	void mqtt_callback::on_success(const mqtt::token& tok)
	{
		std::cout << "\nConnection success"
			<< " with token " << tok.get_message_id() << std::endl;

		std::cout << "\nSubscribing to topic '" << MQTT_TOPIC << "'\n"
			<< "\tfor client " << MQTT_CLIENTID
			<< " using QoS" << MQTT_QOS
			<< std::endl;

		if(connect_token->get_message_id() == tok.get_message_id())
		{
			m_cli.subscribe(MQTT_TOPIC, MQTT_QOS, nullptr, m_subListener);
		}
	}

	void mqtt_callback::connection_lost(const std::string& cause)
	{
		std::cout << "\nConnection lost" << std::endl;
		if (!cause.empty())
		{
			std::cout << "\tcause: " << cause << std::endl;
		}

		std::cout << "Reconnecting..." << std::endl;
		m_nretry = 0;
		reconnect();
	}

	void mqtt_callback::message_arrived(mqtt::const_message_ptr msg)
	{
		std::cout << "Message arrived" << std::endl;
		std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
		std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
	}

	void mqtt_callback::delivery_complete(mqtt::delivery_token_ptr token)
	{

	}

} /* namespace IoTT */

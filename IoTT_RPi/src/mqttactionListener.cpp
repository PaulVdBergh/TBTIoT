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
 * mqttactionListener.cpp
 *
 *  Created on: May 19, 2018
 *      Author: paulvdbergh
 */

#include "mqttactionListener.h"

#include <iostream>

namespace IoTT
{

	mqtt_actionListener::mqtt_actionListener(const std::string& name)
	:	m_name(name)
	{
		// TODO Auto-generated constructor stub
	}

	void mqtt_actionListener::on_failure(const mqtt::token& tok)
	{
		std::cout << m_name << " failure";
		if(tok.get_message_id() != 0)
		{
			std::cout << " for token [" << tok.get_message_id() << "]";
		}
		std::cout << std::endl;
	}

	void mqtt_actionListener::on_success(const mqtt::token& tok)
	{
		std::cout << m_name << " success";
		if(tok.get_message_id() != 0)
		{
			std::cout << " for token [" << tok.get_message_id() << "]";
		}
		auto top = tok.get_topics();
		if(top && !top->empty())
		{
			std::cout << "\ttoken topic : '" << (*top)[0] << "', ...";
		}
		std::cout << std::endl;
	}

} /* namespace IoTT */

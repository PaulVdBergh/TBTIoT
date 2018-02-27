/*
 * Copyright (C) 2017 Paul Van den Bergh <admin@paulvandenbergh.be>
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
 *	For more info see http://paulvandenbergh.be/TBT
 * =====================================================================
 */

/*
 * main.cpp
 *
 *  Created on: Feb 22, 2018
 *      Author: paulvdbergh
 */

#include <string>
#include <mqtt/client.h>

#define ADDRESS "tcp://localhost:1833"
#define CLIENT_ID "Paul"
#define QOS 1

int main(int argc, char* argv[])
{
	const std::string TOPIC { "hello" };
	const std::string PAYLOAD1 { "Hello World!" };

	const char* PAYLOAD2 = "Hi there!";

// Create a client

	mqtt::client cli(ADDRESS, CLIENT_ID);

	mqtt::connect_options connOpts;
	connOpts.set_keep_alive_interval(20);
	connOpts.set_clean_session(true);

	try
	{
		// Connect to the client

		cli.connect(connOpts);

		// Publish using a message pointer.

		auto msg = mqtt::make_message(TOPIC, PAYLOAD1);
		msg->set_qos(QOS);

		cli.publish(msg);

		// Now try with itemized publish.

		cli.publish(TOPIC, PAYLOAD2, strlen(PAYLOAD2), 0, false);

		// Disconnect

		cli.disconnect();
	}
	catch (const mqtt::exception& exc)
	{
		std::cerr << "Error: " << exc.what() << " [" << exc.get_reason_code()
				<< "]" << std::endl;
		return 1;
	}
	return 0;
}


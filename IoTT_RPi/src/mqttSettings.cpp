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
 * mqttSettings.cpp
 *
 *  Created on: May 21, 2018
 *      Author: paulvdbergh
 */

#include "mqttSettings.h"
#include "mqttMessageQueueItem.h"

namespace IoTT
{
	string mqttAddress = "tcp://localhost:1883";
	string mqttClientID = "IoTT_RPi";
	string mqttTopic = "#";

	int mqttQOS = 0;
	int mqttTimeout = 10000L;

	string mqttSubMQName = "/IoTT_RPi_Sub";
	string mqttPubMQName = "/IoTT_RPi_Pub";

	mq_attr mqttMsgQueueAttribs =
	{
		0, 		//	mq_flags
		16, 	//	mq_maxmsg
		sizeof(IoTT::mqttMessageQueueItem), 	//	mq_msgsize
		0		//	mq_curmsg
	};

} /* namespace IoTT */

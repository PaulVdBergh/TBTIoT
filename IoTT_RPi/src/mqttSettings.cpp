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
 *  Created on: May 19, 2018
 *      Author: paulvdbergh
 */

#include "mqttSettings.h"

const int MQTT_RETRY_ATTEMPTS = 5;
const std::string MQTT_ADDRESS("tcp://localhost:1883");
const std::string MQTT_CLIENTID("IoTT_RPi");
const std::string MQTT_TOPIC("#");
const int MQTT_QOS  = 1;

mqtt::token_ptr connect_token = nullptr;

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
 * main.cpp
 *
 *  Created on: May 10, 2018
 *      Author: paulvdbergh
 */

#include <unistd.h>
#include <stdio.h>

#include <pthread.h>
#include "config_thread.h"
#include "mqtt_thread.h"

#include "Z21Interface.h"
#include "MqttMsgHandler.h"

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char const *argv[])
{
	pthread_t conf_thread;
	pthread_t mqtt_thread;
	int retVal;

	retVal = pthread_create(&conf_thread, NULL, config_thread_func, NULL);
	if(0 != retVal)
	{
		printf("pthread_create(config_thread_func) failed with exit code %i.\n", retVal);
	}

	retVal = pthread_create(&mqtt_thread, NULL, mqtt_thread_func, NULL);
	if(0 != retVal)
	{
		printf("pthread_create(mqtt_thread_func) failed with exit code %i.\n", retVal);
	}

	IoTT::Z21Interface* pZ21 = new IoTT::Z21Interface();

	while(true)
	{
		sleep(1);
	}

	delete pZ21;
}

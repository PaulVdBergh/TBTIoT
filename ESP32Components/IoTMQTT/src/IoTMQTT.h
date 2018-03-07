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
 *	This file is part of the TBTIoT project.  
 *	For more info see http://paulvandenbergh.be
 * =====================================================================
 */

/*
 * IoTMQTT.h
 *
 *  Created on: Mar 5, 2018
 *      Author: paulvdbergh
 */

#ifndef COMPONENTS_MQTTCLIENT_C_SRC_IOTMQTT_H_
#define COMPONENTS_MQTTCLIENT_C_SRC_IOTMQTT_H_

#if defined(__cplusplus)
 extern "C" {
#endif

#include "MQTTLinux.h"
#include "MQTTPacket.h"
#include "stdio.h"

#if defined(__cplusplus)
}
#endif

#include <mutex>
#include <thread>
#include <vector>

using namespace std;

enum QoS
{
	QOS0,
	QOS1,
	QOS2
};

/* all failure return codes must be negative */
enum returnCode
{
	BUFFER_OVERFLOW = -2,
	FAILURE = -1,
	SUCCESS = 0
};

/* The Platform specific header must define the Network and Timer structures and functions
 * which operate on them.
 *
typedef struct Network
{
	int (*mqttread)(Network*, unsigned char* read_buffer, int, int);
	int (*mqttwrite)(Network*, unsigned char* send_buffer, int, int);
} Network;*/

/* The Timer structure must be defined in the platform specific header,
 * and have the following functions to operate on it.  */
extern void TimerInit(Timer*);
extern char TimerIsExpired(Timer*);
extern void TimerCountdownMS(Timer*, unsigned int);
extern void TimerCountdown(Timer*, unsigned int);
extern int TimerLeftMS(Timer*);

typedef struct MQTTMessage
{
    enum QoS qos;
    unsigned char retained;
    unsigned char dup;
    unsigned short id;
    void *payload;
    size_t payloadlen;
} MQTTMessage;

typedef struct MessageData
{
    MQTTMessage* message;
    MQTTString* topicName;
} MessageData;

#ifndef MQTT_BROKER_IP
#define MQTT_BROKER_IP "192.168.1.105"
#endif

#ifndef MQTT_BROKER_PORT
#define MQTT_BROKER_PORT 1883
#endif

namespace TBTIoT
{
	class IoTMQTTSubscription;	//	Forward declaration

	class IoTMQTT
	{
		public:
			static IoTMQTT* getInstance();
			virtual ~IoTMQTT();

			int Publish(const char* topic, MQTTMessage* message);
			int Subscribe(IoTMQTTSubscription* subscription);
			int Unsubscribe(IoTMQTTSubscription* subscription);
			int Yield(int timeout_ms);

		protected:
			IoTMQTT();

			int Connect(MQTTPacket_connectData* options);
			int Disconnect();
			int getNextPacketId();
			int sendPacket(int length, Timer* timer);
			int	decodePacket(int* value, int timeout);
			int	readPacket(Timer* timer);
			int deliverMessage(MQTTString* topicName, MQTTMessage* message);
			int keepalive();
			int cycle(Timer* timer);
			int waitfor(int packet_type, Timer* timer);

			static char isTopicMatched(char* topicFilter, MQTTString* topicName);
			static void NewMessageData(MessageData* md, MQTTString* aTopicName, MQTTMessage* aMessage);

		private:
			MQTTPacket_connectData	m_ConnectOptions;
			Network*		ipstack;
			unsigned int	command_timeout_ms;
			unsigned char*	buf;
			size_t			buf_size;
			unsigned char*	readbuf;
			size_t			readbuf_size;
			int				isconnected;
			char			ping_outstanding;
			unsigned int	next_packetid;
			unsigned int	keepAliveInterval;
			Timer			ping_timer;

			void			threadFunc(void);
			thread			m_thread;
//			recursive_mutex	m_Mutex;

			vector<IoTMQTTSubscription*>	m_Subscriptions;

			static IoTMQTT*	sm_Instance;
	};

} /* namespace TBTIoT */

#endif /* COMPONENTS_MQTTCLIENT_C_SRC_IOTMQTT_H_ */

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
 * IoTMQTT.cpp
 *
 *  Created on: Mar 5, 2018
 *      Author: paulvdbergh
 */

#include "IoTMQTT.h"
#include "IoTMQTTSubscription.h"

#include <inttypes.h>
//#include "esp_err.h"

#include <algorithm>
#include <iterator>

using namespace std;

namespace TBTIoT
{

#define MAX_PACKET_ID 65535 /* according to the MQTT specification - do not change! */

	IoTMQTT::IoTMQTT()
	:	m_ConnectOptions(MQTTPacket_connectData_initializer)
	,	ipstack(new Network)
	,	command_timeout_ms(1000)
	,	buf(new unsigned char[1024])
	,	buf_size(1024)
	,	readbuf(new unsigned char[1024])
	,	readbuf_size(1024)
	,	isconnected(0)
	,	ping_outstanding(0)
	,	next_packetid(1)
	,	keepAliveInterval(0)
	{
		NetworkInit(ipstack);
		NetworkConnect(ipstack, (char*)MQTT_BROKER_IP, MQTT_BROKER_PORT);
		::TimerInit(&ping_timer);

		union
		{
			uint64_t	llmac;
			uint8_t 	mac[6];
		} mac_t;

		ESP_ERROR_CHECK(esp_efuse_mac_get_default(mac_t.mac));
		char szMac[21];
		snprintf(szMac, sizeof(szMac), "%" PRIu64, mac_t.llmac);

		MQTTString clientId = MQTTString_initializer;
		clientId.cstring = szMac;

		m_ConnectOptions.clientID = clientId;
		m_ConnectOptions.MQTTVersion = 4;
		m_ConnectOptions.cleansession = 1;
		m_ConnectOptions.keepAliveInterval = 0;
		m_ConnectOptions.willFlag = 0;

		Connect(&m_ConnectOptions);

	    m_thread = thread([this]{threadFunc();});
	}

	IoTMQTT::~IoTMQTT()
	{
		Disconnect();
		delete [] readbuf;
		delete [] buf;
		delete ipstack;
	}

	int IoTMQTT::Connect(MQTTPacket_connectData* options)
	{
		Timer connect_timer;
		int rc = FAILURE;
		MQTTPacket_connectData default_options = MQTTPacket_connectData_initializer;
		int len = 0;

		lock_guard<recursive_mutex> guard(m_Mutex);

		if(!isconnected)
		{
			TimerInit(&connect_timer);
			TimerCountdownMS(&connect_timer, command_timeout_ms);

			if (options == 0)
			{
			    options = &default_options; /* set default options if none were supplied */
			}

			keepAliveInterval = options->keepAliveInterval;
			TimerCountdown(&ping_timer, keepAliveInterval);
			if ((len = MQTTSerialize_connect(buf, buf_size, options)) > 0)
			{
				if ((rc = sendPacket(len, &connect_timer)) == SUCCESS)   // send the connect packet
				{
					// this will be a blocking call, wait for the connack
					if (waitfor(CONNACK, &connect_timer) == CONNACK)
					{
						unsigned char connack_rc = 255;
						unsigned char sessionPresent = 0;
						if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, readbuf, readbuf_size) == 1)
						{
							rc = connack_rc;
						}
						else
						{
							rc = FAILURE;
						}
					}
					else
					{
						rc = FAILURE;
					}
				}
			}
		}
		if(rc == SUCCESS)
		{
			isconnected = 1;
		}
		return rc;
	}

	int IoTMQTT::Publish(const char* topicName, MQTTMessage* message)
	{
	    int rc = FAILURE;
	    Timer timer;
	    MQTTString topic = MQTTString_initializer;
	    topic.cstring = (char *)topicName;
	    int len = 0;

		lock_guard<recursive_mutex> guard(m_Mutex);

	    if(isconnected)
	    {
	    	TimerInit(&timer);
	    	TimerCountdownMS(&timer, command_timeout_ms);

	    	if(QOS1 == message->qos || QOS2 == message->qos)
	    	{
	    		message->id = getNextPacketId();
	    	}

	    	len = MQTTSerialize_publish(buf, buf_size, 0, message->qos, message->retained, message->id,
	    			topic, (unsigned char*)message->payload, message->payloadlen);
	    	if(0 < len)
	    	{
	    		rc = sendPacket(len, &timer);
	    		if(SUCCESS == rc)
	    		{
	    			if(message->qos == QOS1)
	    			{
	    				if(waitfor(PUBACK, &timer) == PUBACK)
	    				{
	    					unsigned short mypacketid;
	    					unsigned char dup, type;
	    					if(1 != MQTTDeserialize_ack(&type, &dup, &mypacketid, readbuf, readbuf_size))
	    					{
	    						rc = FAILURE;
	    					}
	    				}
	    				else
	    				{
	    					rc = FAILURE;
	    				}
	    			}
	    			else if(message->qos == QOS2)
	    			{
	    				if(waitfor(PUBCOMP, &timer) == PUBCOMP)
	    				{
	    					unsigned short mypacketid;
	    					unsigned char dup, type;
	    					if(1 != MQTTDeserialize_ack(&type, &dup, &mypacketid, readbuf, readbuf_size))
	    					{
	    						rc = FAILURE;
	    					}
	    				}
	    				else
	    				{
	    					rc = FAILURE;
	    				}
	    			}
	    		}
	    	}
	    }
	    return rc;
	}

	int IoTMQTT::Subscribe(IoTMQTTSubscription* subscription)
	{
	    int rc = FAILURE;
	    Timer timer;
	    int len = 0;
	    MQTTString topic = MQTTString_initializer;
	    topic.cstring = subscription->getTopic();

		lock_guard<recursive_mutex> guard(m_Mutex);

	    if(isconnected)
	    {
	    	TimerInit(&timer);
	    	TimerCountdownMS(&timer, command_timeout_ms);

	    	len = MQTTSerialize_subscribe(buf, buf_size, 0, getNextPacketId(), 1, &topic, subscription->getpQoS() );
	    	if(0 < len)
	    	{
	    		rc = sendPacket(len, &timer);
	    		if(SUCCESS == rc)
	    		{
	    			if(waitfor(SUBACK, &timer) == SUBACK)
	    			{
	    				int count = 0, grantedQoS = -1;
	    				unsigned short mypacketid;
	    				if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, readbuf, readbuf_size) == 1)
	    				{
	    					rc = grantedQoS;
	    				}
	    				if(0x80 != rc)
	    				{
	    					m_Subscriptions.push_back(subscription);
	    				}
	    			}
	    			else
	    			{
	    				rc = FAILURE;
	    			}
	    		}
	    	}
	    }
	    return rc;
	}

	int IoTMQTT::Unsubscribe(IoTMQTTSubscription* subscription)
	{
		int rc = FAILURE;
		Timer timer;
		MQTTString topic = MQTTString_initializer;
		topic.cstring = subscription->getTopic();
		int len = 0;

		lock_guard<recursive_mutex> guard(m_Mutex);

		auto result = find(begin(m_Subscriptions), end(m_Subscriptions), subscription);
		if(end(m_Subscriptions) != result)
		{
			m_Subscriptions.erase(result);
		}

		class predicate
		{
			public:
				predicate(const char* needle) : m_needle(needle) {}
				bool operator()(IoTMQTTSubscription* sub)
				{
					return (0 == strcmp(m_needle, sub->getTopic()));
				}
			protected:
				const char* m_needle;
		};

		predicate pred(subscription->getTopic());

		if(0 == count_if(begin(m_Subscriptions), end(m_Subscriptions), pred))
		{
			if (isconnected)
			{
				TimerInit(&timer);
				TimerCountdownMS(&timer, command_timeout_ms);

				if ((len = MQTTSerialize_unsubscribe(buf, buf_size, 0, getNextPacketId(), 1, &topic)) > 0)
				{
					if ((rc = sendPacket(len, &timer)) == SUCCESS) // send the subscribe packet
					{
						if (waitfor(UNSUBACK, &timer) == UNSUBACK)
						{
							unsigned short mypacketid; // should be the same as the packetid above
							if (MQTTDeserialize_unsuback(&mypacketid, readbuf, readbuf_size) == 1)
							{
								rc = 0;
							}
						}
						else
						{
							rc = FAILURE;
						}
					}
				}
			}
		}
		else
		{
			rc = 0;
		}
		return rc;
	}

	int IoTMQTT::Disconnect()
	{
	    int rc = FAILURE;
	    Timer timer;     // we might wait for incomplete incoming publishes to complete
	    int len = 0;

		lock_guard<recursive_mutex> guard(m_Mutex);

	    TimerInit(&timer);
	    TimerCountdownMS(&timer, command_timeout_ms);

		len = MQTTSerialize_disconnect(buf, buf_size);
	    if (len > 0)
	        rc = sendPacket(len, &timer);	// send the disconnect packet

	    isconnected = 0;

	    return rc;
	}

	int IoTMQTT::Yield(int timeout_ms)
	{
	    int rc = SUCCESS;
	    Timer timer;

	    TimerInit(&timer);
	    TimerCountdownMS(&timer, timeout_ms);

		do
	    {
	        if (cycle(&timer) == FAILURE)
	        {
	            rc = FAILURE;
	            break;
	        }
		} while (!TimerIsExpired(&timer));

	    return rc;
	}

	int IoTMQTT::getNextPacketId()
	{
		return next_packetid = (next_packetid == MAX_PACKET_ID) ? 1 : next_packetid + 1;
	}

	int IoTMQTT::sendPacket(int length, Timer* timer)
	{
	    int rc = FAILURE,
	        sent = 0;

	    while (sent < length && !TimerIsExpired(timer))
	    {
	        rc = ipstack->mqttwrite(ipstack, &buf[sent], length, TimerLeftMS(timer));
	        if (rc < 0)  // there was an error writing the data
	            break;
	        sent += rc;
	    }
	    if (sent == length)
	    {
	        TimerCountdown(&ping_timer, keepAliveInterval); // record the fact that we have successfully sent the packet
	        rc = SUCCESS;
	    }
	    else
	        rc = FAILURE;
	    return rc;
	}

	int IoTMQTT::decodePacket(int* value, int timeout)
	{
	    unsigned char i;
	    int multiplier = 1;
	    int len = 0;
	    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

	    *value = 0;
	    do
	    {
	        int rc = MQTTPACKET_READ_ERROR;

	        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
	        {
	            rc = MQTTPACKET_READ_ERROR; /* bad data */
	            goto exit;
	        }
	        rc = ipstack->mqttread(ipstack, &i, 1, timeout);
	        if (rc != 1)
	            goto exit;
	        *value += (i & 127) * multiplier;
	        multiplier *= 128;
	    } while ((i & 128) != 0);
	exit:
	    return len;
	}

	int IoTMQTT::readPacket(Timer* timer)
	{
	    int rc = FAILURE;
	    MQTTHeader header = {0};
	    int len = 0;
	    int rem_len = 0;

	    /* 1. read the header byte.  This has the packet type in it */
	    if (ipstack->mqttread(ipstack, readbuf, 1, TimerLeftMS(timer)) != 1)
	        goto exit;

	    len = 1;
	    /* 2. read the remaining length.  This is variable in itself */
	    decodePacket(&rem_len, TimerLeftMS(timer));
	    len += MQTTPacket_encode(readbuf + 1, rem_len); /* put the original remaining length back into the buffer */

	    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
	    if (rem_len > 0 && (ipstack->mqttread(ipstack, readbuf + len, rem_len, TimerLeftMS(timer)) != rem_len))
	        goto exit;

	    header.byte = readbuf[0];
	    rc = header.bits.type;
	exit:
	    return rc;
	}


	int IoTMQTT::deliverMessage(MQTTString* topicName, MQTTMessage* message)
	{
	    int rc = FAILURE;

	    struct predicate
	    {
			predicate(MQTTString* needle) : m_pNeedle(needle) {}
			bool operator()(IoTMQTTSubscription* psub)
			{
				return (MQTTPacket_equals(m_pNeedle, psub->getTopic()) || isTopicMatched(psub->getTopic(), m_pNeedle));
			}
			MQTTString* m_pNeedle;
	    } pred(topicName);

	    auto bound = partition(begin(m_Subscriptions), end(m_Subscriptions), pred);

	    MessageData md;
	    NewMessageData(&md, topicName, message);

	    struct exec_t
	    {
	    	exec_t(MessageData* p) : pmd(p) {}
	    	void operator() (IoTMQTTSubscription* s) { s->OnTopicNotification(pmd); }
	    	MessageData* pmd;
	    } exec(&md);

	    for_each(begin(m_Subscriptions), bound, exec);

	    return rc;
	}

	int IoTMQTT::keepalive()
	{
	    int rc = FAILURE;

	    if (keepAliveInterval != 0)
	    {
	    	if (TimerIsExpired(&ping_timer))
	    	{
	    		if (!ping_outstanding)
	    		{
	    			Timer timer;
	    			TimerInit(&timer);
	    			TimerCountdownMS(&timer, 1000);
	    			int len = MQTTSerialize_pingreq(buf, buf_size);
	    			if (len > 0 && (rc = sendPacket(len, &timer)) == SUCCESS) // send the ping packet
	                {
	    				ping_outstanding = 1;
	                }
	    		}
	    	}
	    }
	    return rc;
	}

	int IoTMQTT::cycle(Timer* timer)
	{
	    // read the socket, see what work is due
	    unsigned short packet_type = readPacket(timer);

	    int len = 0,
	        rc = SUCCESS;

	    switch (packet_type)
	    {
	        case CONNACK:
	        case PUBACK:
	        case SUBACK:
	            break;
	        case PUBLISH:
	        {
	            MQTTString topicName;
	            MQTTMessage msg;
	            int intQoS;
	            if (MQTTDeserialize_publish(&msg.dup, &intQoS, &msg.retained, &msg.id, &topicName,
	               (unsigned char**)&msg.payload, (int*)&msg.payloadlen, readbuf, readbuf_size) != 1)
	                goto exit;
	            msg.qos = (enum QoS)intQoS;
	            printf("X1\n");
	            deliverMessage(&topicName, &msg);
	          	printf("Publish ... qos = %d\n", msg.qos);
	            if (msg.qos != QOS0)
	            {

	                if (msg.qos == QOS1) {
	                    len = MQTTSerialize_ack(buf, buf_size, PUBACK, 0, msg.id);
	                } else if (msg.qos == QOS2) {
	                    len = MQTTSerialize_ack(buf, buf_size, PUBREC, 0, msg.id);
	                } if (len <= 0) {
	                    rc = FAILURE;
	                } else {
	                    rc = sendPacket(len, timer);
	                }
	                if (rc == FAILURE) {
	                    goto exit; // there was a problem
	                }
	            }
	            break;
	        }
	        case PUBREC:
	        {
	            unsigned short mypacketid;
	            unsigned char dup, type;
	            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, readbuf, readbuf_size) != 1)
	                rc = FAILURE;
	            else if ((len = MQTTSerialize_ack(buf, buf_size, PUBREL, 0, mypacketid)) <= 0)
	                rc = FAILURE;
	            else if ((rc = sendPacket(len, timer)) != SUCCESS) // send the PUBREL packet
	                rc = FAILURE; // there was a problem
	            if (rc == FAILURE)
	                goto exit; // there was a problem
	            break;
	        }
	        case PUBCOMP:
	            break;
	        case PINGRESP:
	            ping_outstanding = 0;
	            break;
	    }
	    keepalive();
	exit:
	    if (rc == SUCCESS)
	        rc = packet_type;
	    return rc;
	}

	int IoTMQTT::waitfor(int packet_type, Timer* timer)
	{
	    int rc = FAILURE;

	    do
	    {
	        if (TimerIsExpired(timer))
	            break; // we timed out
	    }
	    while ((rc = cycle(timer)) != packet_type);

	    return rc;
	}

	void IoTMQTT::threadFunc()
	{
		while(1)
		{
			Yield(1000);
		}
	}


	char IoTMQTT::isTopicMatched(char* topicFilter, MQTTString* topicName)
	{
	    char* curf = topicFilter;
	    char* curn = topicName->lenstring.data;
	    char* curn_end = curn + topicName->lenstring.len;

	    while (*curf && curn < curn_end)
	    {
	        if (*curn == '/' && *curf != '/')
	            break;
	        if (*curf != '+' && *curf != '#' && *curf != *curn)
	            break;
	        if (*curf == '+')
	        {   // skip until we meet the next separator, or end of string
	            char* nextpos = curn + 1;
	            while (nextpos < curn_end && *nextpos != '/')
	                nextpos = ++curn + 1;
	        }
	        else if (*curf == '#')
	            curn = curn_end - 1;    // skip until end of string
	        curf++;
	        curn++;
	    };

	    return (curn == curn_end) && (*curf == '\0');
	}

	void IoTMQTT::NewMessageData(MessageData* md, MQTTString* aTopicName, MQTTMessage* aMessage)
	{
		md->topicName = aTopicName;
		md->message = aMessage;
	}

	IoTMQTT* IoTMQTT::getInstance()
	{
		if(nullptr == sm_Instance)
		{
			sm_Instance = new IoTMQTT();
		}
		return sm_Instance;
	}

	IoTMQTT* IoTMQTT::sm_Instance = nullptr;

} /* namespace TBTIoT */

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
 *	For more info see http://paulvandenbergh.be
 * =====================================================================
 */

/*
 * DCCGen.cpp
 *
 *  Created on: Feb 26, 2018
 *      Author: paulvdbergh
 */

#include "DCCGen.h"

#include <cstring>

#include <chrono>
#include <iostream>
using namespace std;

namespace TBTIoT
{

	DCCGen::DCCGen(	gpio_num_t RailcomGPIONum /* = GPIO_NUM_16 */,
					gpio_num_t PowerGPIONum /* = GPIO_NUM_17 */,
					gpio_num_t DccGPIONum /* = GPIO_NUM_18 */,
					rmt_channel_t channel /* = RMT_CHANNEL_0 */
				  )
	:	m_bContinue(true)
	,	m_PowerState(PowerOn)
	,	m_PowerGPIONum(PowerGPIONum)
	,	m_RailcomGPIONum(RailcomGPIONum)
	,	m_DccGPIONum(DccGPIONum)
	,	m_Channel(channel)
	{
	    m_thread = thread([this]{threadFunc();});
	}

	DCCGen::~DCCGen()
	{
		m_bContinue = false;
		m_thread.join();
	}

	DCCMessage* DCCGen::getNextDccMessage()
	{
		return nullptr;
	}

	void DCCGen::threadFunc()
	{
		gpio_config_t gpioConfig;
		memset(&gpioConfig, 0, sizeof(gpio_config_t));

		gpioConfig.pin_bit_mask = ((1ULL << m_RailcomGPIONum) | (1ULL << m_PowerGPIONum));
		gpioConfig.mode = GPIO_MODE_OUTPUT;
		gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
		gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
		gpioConfig.intr_type = GPIO_INTR_DISABLE;

		ESP_ERROR_CHECK(gpio_config(&gpioConfig));

		gpio_pad_select_gpio(m_PowerGPIONum);
		gpio_pad_select_gpio(m_RailcomGPIONum);

		ESP_ERROR_CHECK(gpio_set_direction(m_PowerGPIONum, GPIO_MODE_OUTPUT));
		ESP_ERROR_CHECK(gpio_set_direction(m_RailcomGPIONum, GPIO_MODE_OUTPUT));

		ESP_ERROR_CHECK(gpio_set_pull_mode(m_PowerGPIONum, GPIO_PULLUP_ONLY));
		ESP_ERROR_CHECK(gpio_set_pull_mode(m_RailcomGPIONum, GPIO_PULLUP_ONLY));

		ESP_ERROR_CHECK(gpio_pullup_en(m_PowerGPIONum));
		ESP_ERROR_CHECK(gpio_pullup_en(m_RailcomGPIONum));

		ESP_ERROR_CHECK(gpio_intr_disable(m_PowerGPIONum));
		ESP_ERROR_CHECK(gpio_intr_disable(m_RailcomGPIONum));

		ESP_ERROR_CHECK(gpio_set_level(m_PowerGPIONum, 0));
		ESP_ERROR_CHECK(gpio_set_level(m_RailcomGPIONum, 0));

		memset(&m_config, 0, sizeof(m_config));

	    m_config.rmt_mode = RMT_MODE_TX;
	    m_config.channel = m_Channel;
	    m_config.clk_div = 0x01;
	    m_config.gpio_num = m_DccGPIONum;
	    m_config.mem_block_num = 1;
	    m_config.tx_config.idle_output_en = 1;
	    m_config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;

	    ESP_ERROR_CHECK(rmt_config(&m_config));
	    ESP_ERROR_CHECK(rmt_driver_install(m_config.channel, 0, 0));

		const uint8_t _idleMsg[] = {0x03, 0xFF, 0x00, 0xFF};
		DCCMessage IdleMessage(_idleMsg);

		/* const */ rmt_item32_t preamble_items[PREAMBLE_NBR_CYCLES];
		rmt_item32_t pItems[64];
		uint16_t ItemCount = 0;

		for(uint8_t i = 0; i < PREAMBLE_NBR_CYCLES; i++)
		{
			preamble_items[i] = DCCMessage::DCC_ONE_BIT;
		}

		DCCMessage* pNextMessage = nullptr;

		while(m_bContinue)
		{
			//	send the preamble_items
			ESP_ERROR_CHECK(rmt_write_items(m_config.channel, preamble_items, PREAMBLE_NBR_CYCLES, false));

			usleep(28);
			//	Shutdown power
			gpio_set_level(m_PowerGPIONum, 0);
			usleep(2);
			//	Short the outputs
			gpio_set_level(m_RailcomGPIONum, 1);
			usleep(439);
			//	un-Short outputs
			gpio_set_level(m_RailcomGPIONum, 0);
			usleep(2);
			//	Power up Power
			if(m_PowerState == PowerOn)
			{
				gpio_set_level(m_PowerGPIONum, 1);
			}


			if(nullptr == (pNextMessage = getNextDccMessage()))
			{
				pNextMessage = &IdleMessage;
			}

			ItemCount = pNextMessage->getItemCount();
			ItemCount = (ItemCount > 64) ? 64 : ItemCount;
			memcpy(pItems, pNextMessage->getItems(), ItemCount * sizeof(rmt_item32_t));

			ESP_ERROR_CHECK(rmt_wait_tx_done(m_config.channel, PREAMBLE_WAIT_TIME));
			ESP_ERROR_CHECK(rmt_write_items(m_config.channel, pItems, ItemCount, true));
		}

		ESP_ERROR_CHECK(rmt_driver_uninstall(m_config.channel));
	}
}	//	namespace TBTIoT

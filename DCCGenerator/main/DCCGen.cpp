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

namespace TBTIoT
{

	DCCGen::DCCGen(gpio_num_t gpio_num, rmt_channel_t channel /* = RMT_CHANNEL_0 */)
	:	m_bContinue(true)
	,	m_PowerState(PowerOff)
	{
		memset(&m_config, 0, sizeof(m_config));

	    m_config.rmt_mode = RMT_MODE_TX;
	    m_config.channel = channel;
	    m_config.clk_div = 4;
	    m_config.gpio_num = gpio_num;
	    m_config.mem_block_num = 1;
	    m_config.tx_config.idle_output_en = 1;
	    m_config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;

	    ESP_ERROR_CHECK(rmt_config(&m_config));
	    ESP_ERROR_CHECK(rmt_driver_install(m_config.channel, 0, 0));

	    m_bContinue = true;

	    m_thread = thread([this]{threadFunc();});
	}

	DCCGen::~DCCGen()
	{
		m_bContinue = false;
		m_thread.join();
		ESP_ERROR_CHECK(rmt_driver_uninstall(m_config.channel));
	}

	bool DCCGen::getNextDccCommand()
	{
		return false;
	}

	void DCCGen::threadFunc()
	{
#define PREAMBLE_WAIT_TIME ((TickType_t)10)

		static const rmt_item32_t preamble_items[16] = {0};
		static const rmt_item32_t idle_items[] = {0};
		static rmt_item32_t dcc_items[64];
		static rmt_item32_t* pItems;

		while(m_bContinue)
		{
			//	send the preamble_items
			ESP_ERROR_CHECK(rmt_write_items(m_config.channel, preamble_items, 16, false));

			//	TODO : Insert RAILCOM gap logic here

			if(getNextDccCommand())
			{
				pItems = dcc_items;
			}
			else
			{
				pItems = const_cast<rmt_item32_t*>(idle_items);
			}

			ESP_ERROR_CHECK(rmt_wait_tx_done(m_config.channel, PREAMBLE_WAIT_TIME));

			ESP_ERROR_CHECK(rmt_write_items(m_config.channel, pItems, 1, true));
		}
	}
}	//	namespace TBTIoT

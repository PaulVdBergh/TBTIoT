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
 * DCCGen.h
 *
 *  Created on: Feb 26, 2018
 *      Author: paulvdbergh
 */

#include "driver/gpio.h"
#include "driver/rmt.h"
#include <thread>

using namespace std;

#ifndef MAIN_DCCGEN_H_
#define MAIN_DCCGEN_H_

namespace TBTIoT
{
#define ZERO_PULSE_LENGTH (8640)
#define ONE_PULSE_LENGTH (4640)

#define PREAMBLE_WAIT_TIME ((TickType_t)1000000000)

//	class DCCMessage;	//	Forward declaration

	class DCCGen
	{
		public:
/*			typedef enum
			{
				PowerOff = 0,
				PowerOn
			} PowerState_t;
*/
			DCCGen(	gpio_num_t RailcomGPIONum = GPIO_NUM_4,
					gpio_num_t PowerGPIONum = GPIO_NUM_2,
					gpio_num_t DccGPIONum = GPIO_NUM_0,
					rmt_channel_t channel = RMT_CHANNEL_0);

			virtual ~DCCGen();

		protected:

		private:
			void threadFunc(void);

			thread			m_thread;
			volatile bool 	m_bContinue;

			rmt_config_t 	m_rmtConfig;

			gpio_num_t		m_PowerGPIONum;
			gpio_num_t		m_RailcomGPIONum;
			gpio_num_t		m_DccGPIONum;
			rmt_channel_t	m_Channel;

			static const rmt_item32_t	DCC_ZERO_BIT;
			static const rmt_item32_t	DCC_ONE_BIT;
			static const rmt_item32_t 	preamble_items[];
			static const rmt_item32_t 	idle_items[];
	};
}	//	namespace TBTIoT

#endif /* MAIN_DCCGEN_H_ */

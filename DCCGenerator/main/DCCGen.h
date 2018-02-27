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
 * DCCGen.h
 *
 *  Created on: Feb 26, 2018
 *      Author: paulvdbergh
 */

#ifndef MAIN_DCCGEN_H_
#define MAIN_DCCGEN_H_

#include "driver/rmt.h"
#include <thread>

using namespace std;

#include "DCCMessage.h"

namespace TBTIoT
{
	class DCCGen
	{
		public:
			typedef enum
			{
				PowerOff = 0,
				PowerOn
			} PowerState_t;

			DCCGen(gpio_num_t gpio_num, rmt_channel_t channel = RMT_CHANNEL_0);
			virtual ~DCCGen();

		protected:
			virtual bool	getNextDccCommand(void);

			rmt_config_t m_config;

		private:
			void threadFunc(void);

			thread			m_thread;
			volatile bool 	m_bContinue;
			PowerState_t	m_PowerState;
	};
}	//	namespace TBTIoT

#endif /* MAIN_DCCGEN_H_ */

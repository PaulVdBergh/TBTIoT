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
 * SystemState.h
 *
 *  Created on: May 31, 2018
 *      Author: paulvdbergh
 */

#ifndef SYSTEMSTATE_SRC_SYSTEMSTATE_H_
#define SYSTEMSTATE_SRC_SYSTEMSTATE_H_

#include <stdint.h>

#include "MQTTPublisher.h"
#include "MQTTSubscription.h"
using namespace TBTIoT;

#include <string>
#include <mutex>
using namespace std;

namespace IoTT
{

#define csEmergencyStop			0x01
#define csTrackVoltageOff		0x02
#define csShortCircuit			0x04
#define csProgrammingModeActive	0x20

#define cseHighTemperature		0x01
#define csePowerLost			0x02
#define cseShortCircuitExternal	0x04
#define cseShortCircuitInternal	0x08

	class SystemState : public MQTTSubscription
	{
		public:
			SystemState(const string& topic);
			virtual ~SystemState();

			virtual void onNewData(const string& topic, const string& payload);

			uint16_t	getMainCurrent(void);
			uint16_t	getProgCurrent(void);
			uint16_t	getFilteredMainCurrent(void);
			uint16_t	getTemperature(void);
			uint16_t	getSupplyVoltage(void);
			uint16_t	getVCCVoltage(void);
			bool		getEmergencyStopStatus(void);
			bool		getTrackVoltageOffStatus(void);
			bool		getShortCircuitStatus(void);

			void	setMainCurrent(const uint16_t& newVal);
			void	setProgCurrent(const uint16_t& newVal);
			void	setFilteredMainCurrent(const uint16_t& newVal);
			void	setTemperature(const uint16_t& newVal);
			void	setSupplyVoltage(const uint16_t& newVal);
			void	setVCCVoltage(const uint16_t& newVal);

			void	setEmergencyStop(void);
			void	resetEmergencyStop(void);

			void	setTrackVoltageOff(void);
			void	setTrackVoltageOn(void);

			void	setShortCircuit(void);
			void	resetShortCircuit(void);

		protected:
			MQTTPublisher	m_MainCurrentPublisher;
			MQTTPublisher	m_ProgCurrentPublisher;
			MQTTPublisher	m_FilteredMainCurrentPublisher;
			MQTTPublisher	m_TemperaturePublisher;
			MQTTPublisher	m_SupplyVoltagePublisher;
			MQTTPublisher	m_VCCVoltagePublisher;
			MQTTPublisher	m_CentralStatePublisher;

		private:
			/**
			 *
			 <table>
			 <caption>response:</caption>
			 <tr><th colspan="2">DataLen<th colspan="2">Headers<th>Data
			 <tr><td>0x14<td>0x00<td>0x84<td>0x00<td>SystemState (16 Bytes)
			 </table>
			 <table>
			 <caption><b>SystemState</b> layout (INT16 is little-endian)</caption>
			 <tr><th>Byte offset<th>Type<th>Name<th>Unit<th>note
			 <tr><td>0<td>int16_t<td>MainCurrent<td>mA<td>Main Track Current
			 <tr><td>2<td>int16_t<td>ProgCurrent<td>mA<td>Programm Track Current
			 <tr><td>4<td>int16_t<td>FilteredMainCurrent<td>mA<td>Smoothed Main Track Current
			 <tr><td>6<td>int16_t<td>Temperature<td>&#176;C<td>Internal temperature
			 <tr><td>8<td>uint16_t<td>SupplyVoltage<td>mV<td>Supply voltage
			 <tr><td>10<td>uint16_t<td>VCCVoltage<td>mV<td>Voltage at the track
			 <tr><td>12<td>uint8_t<td>CentralState<td>bitmask<td>see Manager::getCentralState
			 <tr><td>13<td>uint8_t<td>CentralStateEx<td>bitmask<td>see Manager::getCentralStateEx
			 <tr><td>14<td>uint8_t<td>reserved<td><td>
			 <tr><td>15<td>uint8_t<td>reserved<td><td>
			 </table>
			 */
			struct SystemState_t
			{
				uint8_t DataLen = 0x14;
				uint8_t filler1 = 0x00;
				uint8_t Header = 0x84;
				uint8_t filler2 = 0x00;
				int16_t MainCurrent = 0x0000;
				int16_t ProgCurrent = 0x0000;
				int16_t FilteredMainCurrent = 0x0000;
				int16_t Temperature = 0x0000;
				uint16_t SupplyVoltage = 0x0000;
				uint16_t VCCVoltage = 0x0000;
				uint8_t CentralState = csTrackVoltageOff;
				uint8_t CentralStateEx = 0x00;
				uint8_t reserved1 = 0x00;
				uint8_t reserved2 = 0x00;
			}__attribute__((packed)) m_SystemState;

			recursive_mutex		m_MSystemState;

	};

} /* namespace IoTT */

#endif /* SYSTEMSTATE_SRC_SYSTEMSTATE_H_ */

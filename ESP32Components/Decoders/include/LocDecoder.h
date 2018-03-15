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
 * LocDecoder.h
 *
 *  Created on: Mar 11, 2018
 *      Author: paulvdbergh
 */

#ifndef DECODERS_INCLUDE_LOCDECODER_H_
#define DECODERS_INCLUDE_LOCDECODER_H_

#include "../src/Decoder.h"
#include "MQTTPublisher.h"

namespace TBTIoT
{

	class LocDecoder: public Decoder
	{
		public:
			LocDecoder(const DCCAddress_t& address);
			virtual ~LocDecoder();

			virtual bool getNextDCCCommand(uint8_t* pBuffer);
			virtual void onNewMQTTData(const string& topic, const string& payload);

			void	getLANLocInfo(uint8_t* pMsg);
			uint8_t	getLocMode(void) { return m_LocMode; }
			uint8_t	setLocMode(const uint8_t& newMode);

			uint8_t	getSpeed(void);
			bool	getBusy(void);
			uint8_t	getSpeedSteps(void);
			uint8_t	getLocoDrive14(void);
			uint8_t	getLocoDrive27(void);
			uint8_t	getLocoDrive28(void);
			uint8_t	getLocoDrive128(void);
			bool	getDirection(void);
			bool	getDualTraction(void);
			uint8_t	getFunctionGroup1(void);
			uint8_t	getFunctionGroup2(void);
			uint8_t	getFunctionGroup3(void);
			uint8_t	getFunctionGroup4(void);
			uint8_t	getFunctionGroup5(void);

			bool	getLight(void);
			bool	getF0(void);
			bool	getF1(void);
			bool	getF2(void);
			bool	getF3(void);
			bool	getF4(void);
			bool	getF5(void);
			bool	getF6(void);
			bool	getF7(void);
			bool	getF8(void);
			bool	getF9(void);
			bool	getF10(void);
			bool	getF11(void);
			bool	getF12(void);
			bool	getF13(void);
			bool	getF14(void);
			bool	getF15(void);
			bool	getF16(void);
			bool	getF17(void);
			bool	getF18(void);
			bool	getF19(void);
			bool	getF20(void);
			bool	getF21(void);
			bool	getF22(void);
			bool	getF23(void);
			bool	getF24(void);
			bool	getF25(void);
			bool	getF26(void);
			bool	getF27(void);
			bool	getF28(void);

			void	setSpeed(const uint8_t& newValue);
			void	setBusy(const bool& newValue);
			void	setSpeedSteps(const uint8_t& newValue);
			void	setLocoDrive14(const uint8_t& newValue);
			void	setLocoDrive27(const uint8_t& newValue);
			void	setLocoDrive28(const uint8_t& newValue);
			void	setLocoDrive128(const uint8_t& newValue);
			void	setDirection(const bool& newValue);
			void	setDualTraction(const bool& newValue);
			void	setSmartSearch(const bool& newValue);
			void	setFunctionGroup1(const uint8_t& newValue);
			void	setFunctionGroup2(const uint8_t& newValue);
			void	setFunctionGroup3(const uint8_t& newValue);
			void	setFunctionGroup4(const uint8_t& newValue);
			void	setFunctionGroup5(const uint8_t& newValue);

			void	setLight(const bool& newValue);
			void	setF0(const bool& newValue);
			void	setF1(const bool& newValue);
			void	setF2(const bool& newValue);
			void	setF3(const bool& newValue);
			void	setF4(const bool& newValue);
			void	setF5(const bool& newValue);
			void	setF6(const bool& newValue);
			void	setF7(const bool& newValue);
			void	setF8(const bool& newValue);
			void	setF9(const bool& newValue);
			void	setF10(const bool& newValue);
			void	setF11(const bool& newValue);
			void	setF12(const bool& newValue);
			void	setF13(const bool& newValue);
			void	setF14(const bool& newValue);
			void	setF15(const bool& newValue);
			void	setF16(const bool& newValue);
			void	setF17(const bool& newValue);
			void	setF18(const bool& newValue);
			void	setF19(const bool& newValue);
			void	setF20(const bool& newValue);
			void	setF21(const bool& newValue);
			void	setF22(const bool& newValue);
			void	setF23(const bool& newValue);
			void	setF24(const bool& newValue);
			void	setF25(const bool& newValue);
			void	setF26(const bool& newValue);
			void	setF27(const bool& newValue);
			void	setF28(const bool& newValue);


		protected:
			bool	getDCCSpeedMessage(uint8_t* pBuffer);

			struct  locInfo_t
			{
				uint32_t	DataLen		= 0x0040000E;
				uint8_t		X_Header 	= 0xEF;
				uint8_t		Addr_MSB 	= 0x00;
				uint8_t		Addr_LSB 	= 0x00;
				uint8_t		DB2 		= 0x00;
				uint8_t		DB3 		= 0x00;
				uint8_t		DB4 		= 0x00;
				uint8_t		DB5 		= 0x00;
				uint8_t		DB6 		= 0x00;
				uint8_t		DB7 		= 0x00;
				uint8_t		XOR 		= 0x00;
			}__attribute__((packed));

			uint8_t		m_DCCState;
			uint8_t		m_LocMode;
			locInfo_t	m_LocInfo;
			string		m_BaseTopic;

		private:
			uint8_t* insertDCCAddress(uint8_t* pMsg);

	};

} /* namespace TBTIoT */

#endif /* DECODERS_INCLUDE_LOCDECODER_H_ */

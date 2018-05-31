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
 * SystemState.cpp
 *
 *  Created on: May 31, 2018
 *      Author: paulvdbergh
 */

#include "SystemState.h"

#include <esp_log.h>

namespace IoTT
{

	SystemState::SystemState(const string& topic)
	:	MQTTSubscription(topic + "SystemState/#")
	,	m_MainCurrentPublisher("/IoTT/SystemState/MainCurrent")
	,	m_ProgCurrentPublisher("/IoTT/SystemState/ProgCurrent")
	,	m_FilteredMainCurrentPublisher("/IoTT/SystemState/FilteredMainCurrent")
	,	m_TemperaturePublisher("/IoTT/SystemState/Temperature")
	,	m_SupplyVoltagePublisher("/IoTT/SystemState/SupplyVoltage")
	,	m_VCCVoltagePublisher("/IoTT/SystemState/VCCVoltage")
	,	m_CentralStatePublisher("/IoTT/SystemState/CentralState")
	{
	// TODO Auto-generated constructor stub

	}

	SystemState::~SystemState()
	{
	// TODO Auto-generated destructor stub
	}

	void SystemState::onNewData(const string& topic, const string& payload)
	{
		ESP_LOGI("SystemState", "onNewData(topic = %s, payload = %s) called.", topic.c_str(), payload.c_str());
	}

	uint16_t SystemState::getMainCurrent(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		return m_SystemState.MainCurrent;
	}

	uint16_t SystemState::getProgCurrent(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		return m_SystemState.ProgCurrent;
	}

	uint16_t SystemState::getFilteredMainCurrent(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		return m_SystemState.FilteredMainCurrent;
	}

	uint16_t SystemState::getTemperature(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		return m_SystemState.Temperature;
	}

	uint16_t SystemState::getSupplyVoltage(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		return m_SystemState.SupplyVoltage;
	}

	uint16_t SystemState::getVCCVoltage(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		return m_SystemState.VCCVoltage;
	}

	bool SystemState::getEmergencyStopStatus(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		return (m_SystemState.CentralState & csEmergencyStop);
	}

	bool SystemState::getTrackVoltageOffStatus(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		return (m_SystemState.CentralState & csTrackVoltageOff);
	}

	bool SystemState::getShortCircuitStatus(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		return (m_SystemState.CentralState & csShortCircuit);
	}

	void SystemState::setMainCurrent(const uint16_t& newVal)
	{
		m_MainCurrentPublisher.Publish(newVal);
	}

	void SystemState::setProgCurrent(const uint16_t& newVal)
	{
		m_ProgCurrentPublisher.Publish(newVal);
	}

	void SystemState::setFilteredMainCurrent(const uint16_t& newVal)
	{
		m_FilteredMainCurrentPublisher.Publish(newVal);
	}

	void SystemState::setTemperature(const uint16_t& newVal)
	{
		m_TemperaturePublisher.Publish(newVal);
	}

	void SystemState::setSupplyVoltage(const uint16_t& newVal)
	{
		m_SupplyVoltagePublisher.Publish(newVal);
	}

	void SystemState::setVCCVoltage(const uint16_t& newVal)
	{
		m_VCCVoltagePublisher.Publish(newVal);
	}

	void SystemState::setEmergencyStop(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		m_SystemState.CentralState |= csEmergencyStop;
		m_CentralStatePublisher.Publish(m_SystemState.CentralState);
	}

	void SystemState::resetEmergencyStop(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		m_SystemState.CentralState &= ~(csEmergencyStop | csTrackVoltageOff);
		m_CentralStatePublisher.Publish(m_SystemState.CentralState);
	}

	void SystemState::setTrackVoltageOff(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		m_SystemState.CentralState |= csTrackVoltageOff;
		m_CentralStatePublisher.Publish(m_SystemState.CentralState);
	}

	void SystemState::setTrackVoltageOn(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		m_SystemState.CentralState &= ~(csEmergencyStop | csTrackVoltageOff);
		m_CentralStatePublisher.Publish(m_SystemState.CentralState);
	}

	void SystemState::setShortCircuit(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		m_SystemState.CentralState |= csShortCircuit;
		m_CentralStatePublisher.Publish(m_SystemState.CentralState);
	}

	void SystemState::resetShortCircuit(void)
	{
		lock_guard<recursive_mutex> lock(m_MSystemState);
		m_SystemState.CentralState &= ~(csShortCircuit);
		m_CentralStatePublisher.Publish(m_SystemState.CentralState);
	}

} /* namespace IoTT */

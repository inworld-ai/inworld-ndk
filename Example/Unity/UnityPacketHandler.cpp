/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "UnityPacketHandler.h"


void NDKUnity::CUnityPacketHandler::SetPacketCallback(
	UnityPacketCallback pktCallBack,
	PhonemeCallBack phonemeCallBack,
	TriggerParamCallBack triggerParamCallBack)
{
	m_PacketCallback = pktCallBack,
	m_PhonemeCallBack = phonemeCallBack;
	m_TriggerParamCallBack = triggerParamCallBack;
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::TextEvent& Event)
{	
	if (m_PacketCallback)
		m_PacketCallback(UnityNDKPacket(Event));
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::AudioDataEvent& Event)
{
	if (m_PacketCallback)
		m_PacketCallback(UnityNDKPacket(Event));
	for (const auto& phonemes = Event.GetPhonemeInfos(); const auto& phone : phonemes)
	{
		if (m_PhonemeCallBack)
			m_PhonemeCallBack(PhonemeInfo(Event, phone));
	}
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::CustomEvent& Event)
{
	if (m_PacketCallback)
		m_PacketCallback(UnityNDKPacket(Event));
	for (const auto& params = Event.GetParams(); const auto& pair : params)
	{
		if (m_TriggerParamCallBack)
			m_TriggerParamCallBack(TriggerParam(Event, pair.first, pair.second));
	}
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::ActionEvent& Event)
{
	if (m_PacketCallback)
		m_PacketCallback(UnityNDKPacket(Event));
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::RelationEvent& Event)
{
	if (m_PacketCallback)
		m_PacketCallback(UnityNDKPacket(Event));
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::CancelResponseEvent& Event)
{
	if (m_PacketCallback)
		m_PacketCallback(UnityNDKPacket(Event));
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::EmotionEvent& Event)
{
	if (m_PacketCallback)
		m_PacketCallback(UnityNDKPacket(Event));
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::ControlEvent& Event)
{
	if (m_PacketCallback)
		m_PacketCallback(UnityNDKPacket(Event));
}

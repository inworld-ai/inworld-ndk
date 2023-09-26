/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "UnityPacketHandler.h"


void NDKUnity::CUnityPacketHandler::SetPacketCallback(
	TextCallBack textCallBack,
	AudioCallBack audioCallBack,
	ControlCallBack controlCallBack,
	EmotionCallBack emotionCallBack,
	CancelResponseCallBack cancelResponseCallBack,
	CustomCallBack customCallBack,
	PhonemeCallBack phonemeCallBack,
	TriggerParamCallBack triggerParamCallBack)
{
	m_TextCallBack = textCallBack;
	m_AudioCallBack = audioCallBack;
	m_ControlCallBack = controlCallBack;
	m_EmotionCallBack = emotionCallBack;
	m_CancelResponseCallBack = cancelResponseCallBack;
	m_CustomCallBack = customCallBack;
	m_PhonemeCallBack = phonemeCallBack;
	m_TriggerParamCallBack = triggerParamCallBack;
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::TextEvent& Event)
{	
	if (m_TextCallBack)
		m_TextCallBack(TextPacket(Event));
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::AudioDataEvent& Event)
{
	if (m_AudioCallBack)
	{
		m_AudioCallBack(AudioPacket(Event));
	}
	for (const auto& phonemes = Event.GetPhonemeInfos(); const auto& phone : phonemes)
	{
		if (m_PhonemeCallBack)
			m_PhonemeCallBack(PhonemeInfo(Event, phone));
	}
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::CustomEvent& Event)
{
	if (m_CustomCallBack)
		m_CustomCallBack(CustomPacket(Event));
	for (const auto& params = Event.GetParams(); const auto& pair : params)
	{
		if (m_TriggerParamCallBack)
			m_TriggerParamCallBack(TriggerParam(Event, pair.first, pair.second));
	}
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::CancelResponseEvent& Event)
{
	if (m_CancelResponseCallBack)
		m_CancelResponseCallBack(CancelResponsePacket(Event));
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::EmotionEvent& Event)
{
	if (m_EmotionCallBack)
		m_EmotionCallBack(EmotionPacket(Event));
}

void NDKUnity::CUnityPacketHandler::Visit(const Inworld::ControlEvent& Event)
{
	if (m_ControlCallBack)
		m_ControlCallBack(ControlPacket(Event));
}

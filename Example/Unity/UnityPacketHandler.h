/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include "Packets.h"
#include "Data/UnityNDKInteropData.h"

namespace NDKUnity
{
	class CUnityPacketHandler : public Inworld::PacketVisitor
	{
	public:
		CUnityPacketHandler() = default;	
		virtual ~CUnityPacketHandler() = default;
		
		void Visit(const Inworld::TextEvent& Event) override;
		void Visit(const Inworld::AudioDataEvent& Event) override;
		void Visit(const Inworld::ControlEvent& Event) override;
		void Visit(const Inworld::EmotionEvent& Event) override;
		void Visit(const Inworld::CancelResponseEvent& Event) override;
		void Visit(const Inworld::CustomEvent& Event) override;
		
		void SetPacketCallback(TextCallBack textCallBack,
			AudioCallBack audioCallBack,
			ControlCallBack controlCallBack,
			EmotionCallBack emotionCallBack,
			CancelResponseCallBack cancelResponseCallBack,
			CustomCallBack customCallBack,
			PhonemeCallBack phonemeCallBack,
			TriggerParamCallBack triggerParamCallBack);

	private:
		TextCallBack m_TextCallBack;
		AudioCallBack m_AudioCallBack;
		ControlCallBack m_ControlCallBack;
		EmotionCallBack m_EmotionCallBack;
		CancelResponseCallBack m_CancelResponseCallBack;
		CustomCallBack m_CustomCallBack;
		PhonemeCallBack m_PhonemeCallBack;
		TriggerParamCallBack m_TriggerParamCallBack;
	};
}


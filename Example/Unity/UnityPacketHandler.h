/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
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
		void Visit(const Inworld::ActionEvent& Event) override;
		void Visit(const Inworld::RelationEvent& Event) override;
		void SetPacketCallback(UnityPacketCallback pktCallBack, PhonemeCallBack phonemeCallBack, TriggerParamCallBack triggerParamCallBack);

	private:
		UnityPacketCallback m_PacketCallback;
		PhonemeCallBack m_PhonemeCallBack;
		TriggerParamCallBack m_TriggerParamCallBack;
	};
}


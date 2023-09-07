#pragma once
#include "Packets.h"
#include "Data/UnityNDKInteropData.h"

namespace NDKUnity
{
	class CUnityPacketHandler : public Inworld::PacketVisitor
	{
	public:
		CUnityPacketHandler() = default;
		virtual ~CUnityPacketHandler() override = default;
		
		virtual void Visit(const Inworld::TextEvent& Event) override;
		virtual void Visit(const Inworld::AudioDataEvent& Event) override;
		virtual void Visit(const Inworld::ControlEvent& Event) override;
		virtual void Visit(const Inworld::EmotionEvent& Event) override;
		virtual void Visit(const Inworld::CancelResponseEvent& Event) override;
		virtual void Visit(const Inworld::CustomEvent& Event) override;

		void SetTextCallback(const TextCallBack& callBack);

	private:
		TextCallBack m_TextCallBack;
	};
}


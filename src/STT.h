/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#ifdef INWORLD_STT_LOCAL
#include <string>
#include "RunnableCommand.h"
	
namespace Inworld
{
	class RunnableSTT : public Runnable
	{
	public:
		RunnableSTT(SharedQueue<std::shared_ptr<AudioDataEvent>>& InEvents, std::function<void(std::shared_ptr<TextEvent>)> OutCallback)
			: _InEvents(InEvents)
			, _OutCallback(OutCallback)
		{}

		virtual void Run() override;
		virtual void Deinitialize() override;

	private:
		SharedQueue<std::shared_ptr<AudioDataEvent>>& _InEvents;
		std::function<void(std::shared_ptr<TextEvent>)> _OutCallback;
	};
}

#endif
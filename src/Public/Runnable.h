/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <atomic>
#include "Define.h"

namespace Inworld
{
	class INWORLD_EXPORT Runnable
	{
	public:
		virtual ~Runnable() = default;

		bool IsDone() const { return _IsDone; };
		void Stop()
		{
			_IsDone = true;
			Deinitialize();
		}

		virtual void Run() = 0;
		virtual void Deinitialize() {}

	protected:
		std::atomic<bool> _IsDone = false;
	};
}

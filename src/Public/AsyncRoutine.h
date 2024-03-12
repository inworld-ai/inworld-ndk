/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include <thread>
#include <atomic>
#include <string>

#include "Runnable.h"
#include "Define.h"

namespace Inworld
{
	class INWORLD_EXPORT AsyncRoutine
	{
	public:
		~AsyncRoutine() { Stop(); }

		void Start(std::string ThreadName, std::unique_ptr<Runnable> Runnable);
		void Stop();
		
		bool IsDone() const
		{
			return _Runnable ? _Runnable->IsDone() : false;
		}
		
		bool IsValid() const
		{ 
			return _Runnable && _Thread;
		}

		Runnable* GetRunnable()
		{
			return _Runnable.get();
		}

	protected:
		std::unique_ptr<Runnable> _Runnable;
		std::unique_ptr<std::thread> _Thread;
	};

}
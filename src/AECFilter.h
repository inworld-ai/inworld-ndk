/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include "Define.h"
#include <vector>

namespace Inworld
{
	class INWORLD_EXPORT AECFilter
	{
	public:
		AECFilter();
		~AECFilter();

		std::vector<int16_t> FilterAudio(const std::vector<int16_t>& InputData, const std::vector<int16_t>& OutputData);

	private:
		void* AecHandle = nullptr;
	};
}


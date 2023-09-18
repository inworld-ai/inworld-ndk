/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include <ctime>
#include "Types.h"

namespace NDKUnity
{
	struct UnityAgentInfo : Inworld::AgentInfo
	{
		std::string RpmModelUri;
		std::string RpmImageUriPortrait;
		std::string RpmImageUriPosture;
		std::string AvatarImg;
		std::string AvatarImgOriginal;	
	};

}

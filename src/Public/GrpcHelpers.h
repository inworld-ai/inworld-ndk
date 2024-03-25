/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <string>
#include "Define.h"

namespace ai { namespace inworld { namespace studio { namespace v1alpha
{
	class Character;
}}}}

namespace InworldV1alpha = ai::inworld::studio::v1alpha;

namespace Inworld
{
	namespace GrpcHelper
	{
		class CharacterInfo
		{
		public:
			CharacterInfo() = default;
			CharacterInfo(const InworldV1alpha::Character& GrpcCharacter);
			~CharacterInfo() = default;

			std::string _Name;
			std::string _RpmModelUri;
			std::string _RpmImageUri;
			std::string _RpmPortraitUri;
			std::string _RpmPostureUri;
			bool _bMale;
		};

		INWORLD_EXPORT CharacterInfo CreateCharacterInfo(const InworldV1alpha::Character& GrpcCharacter);
		
	}
}
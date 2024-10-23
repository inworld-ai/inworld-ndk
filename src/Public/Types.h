/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <string>
#include <ctime>
#include <unordered_set>
#include "Define.h"

namespace Inworld
{
	using ClientHeaderData = std::vector<std::pair<std::string, std::string>>;

	struct INWORLD_EXPORT SessionToken
	{
		std::string SessionId;
		std::string Token;
		int64_t ExpirationTime;

		bool IsValid() const
		{
			return !Token.empty() && !SessionId.empty() && ExpirationTime > std::time(0);
		}
	};

	struct INWORLD_EXPORT SessionSave
	{
		std::string SceneId;
		std::string State;

		bool IsValid() const
		{
			return !SceneId.empty() && !State.empty();
		}
	};

	struct INWORLD_EXPORT AgentInfo
	{
		std::string BrainName;
		std::string AgentId;
		std::string GivenName;
	};

	struct INWORLD_EXPORT InteractionFeedback
	{
	public:
		bool bIsLike = false;
		enum INWORLD_EXPORT DislikeType : uint8_t
		{
			UNSPECIFIED = 0,
			IRRELEVANT = 1,
			UNSAFE = 2,
			UNTRUE = 3,
			INCORRECT_USE_KNOWLEDGE = 4,
			UNEXPECTED_ACTION = 5,
			NEXPECTED_GOAL_BEHAVIOR = 6,
			REPETITION = 7,
		};
		std::unordered_set<DislikeType> DislikeReasons;
		std::string comment;
	};

}

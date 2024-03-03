/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <vector>
#include <string>

#include "Define.h"

namespace Inworld
{
	namespace Utils
	{
		INWORLD_EXPORT std::string GetSslRootCerts();
		INWORLD_EXPORT std::string PhonemeToViseme(const std::string& Phoneme);

		INWORLD_EXPORT void HmacSha256(const std::vector<uint8_t>& Data, const std::vector<uint8_t>& Key, std::vector<uint8_t>& Signature);
		INWORLD_EXPORT std::string ToHex(const std::vector<uint8_t>& Data);
	}
}

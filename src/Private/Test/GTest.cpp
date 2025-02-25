/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#ifdef INWORLD_WITH_GTEST

#include <iostream>
#include <cstring>

#include "gtest/gtest.h"
#include "Utils/Utils.h"

TEST(Utils, SslRootSerts)
{
    EXPECT_NE(Inworld::Utils::GetSslRootCerts(), "");
}

TEST(Utils, PhonemeToViseme)
{
	EXPECT_EQ(Inworld::Utils::PhonemeToViseme("b"), "PP");
	EXPECT_EQ(Inworld::Utils::PhonemeToViseme("n"), "Nn");
	EXPECT_EQ(Inworld::Utils::PhonemeToViseme("j"), "I");
}

static std::vector<uint8_t> StrToVec(const std::string& Data)
{
	std::vector<uint8_t> Res;
	Res.resize(Data.size());
	std::memcpy(Res.data(), Data.data(), Data.size());
	return Res;
}

TEST(Utils, HmacSha256)
{
	const std::string Data = "20220408182450";
	const std::string Key = "IW1eVA0wHtiN8snf27KA4Zl2HfBZUi8pXfazni1oy5ahsyEDV1lrIzh8ILOVf7cO43u";
	const std::string ExpectedRes = "c2fd81041cbeaaedd91220906920f911297cfda7851de7578856225f72e1e886";

	std::vector<uint8_t> Signature(32);
	Inworld::Utils::HmacSha256(StrToVec(Data), StrToVec(Key), Signature);
	const auto Res = Inworld::Utils::ToHex(Signature);

	EXPECT_EQ(ExpectedRes, Res);
}

#endif

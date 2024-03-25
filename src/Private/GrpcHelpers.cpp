/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "GrpcHelpers.h"
#include "ai/inworld/studio/v1alpha/characters.pb.h"

Inworld::GrpcHelper::CharacterInfo Inworld::GrpcHelper::CreateCharacterInfo(const InworldV1alpha::Character& GrpcCharacter)
{
	Inworld::GrpcHelper::CharacterInfo Info(GrpcCharacter);
	return Info;
}

Inworld::GrpcHelper::CharacterInfo::CharacterInfo(const InworldV1alpha::Character& GrpcCharacter)
	: _Name(GrpcCharacter.name())
	, _RpmModelUri(GrpcCharacter.default_character_assets().rpm_model_uri())
	, _RpmImageUri(GrpcCharacter.default_character_assets().rpm_image_uri())
	, _RpmPortraitUri(GrpcCharacter.default_character_assets().rpm_image_uri_portrait())
	, _RpmPostureUri(GrpcCharacter.default_character_assets().rpm_image_uri_posture())
	, _bMale(GrpcCharacter.default_character_description().pronoun() == InworldV1alpha::Character_CharacterDescription_Pronoun_PRONOUN_MALE)
{}

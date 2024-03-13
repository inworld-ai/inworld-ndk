/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "StudioClient.h"
#include "Service.h"
#include "Log.h"
#include "GrpcHelpers.h"
#include <memory>

void Inworld::StudioClient::RequestStudioUserDataAsync(const std::string& Token, const std::string& ServerUrl, std::function<void(bool bSuccess)> Callback)
{
	ClearError();

	_ServerUrl = ServerUrl;
	_Callback = Callback;

	Request(
		"RunnableGenerateUserTokenRequest",
		std::make_unique<Inworld::RunnableGenerateUserTokenRequest>(
			Token,
			ServerUrl,
			[this](const grpc::Status& Status, const InworldV1alpha::GenerateTokenUserResponse& Response)
			{
				if (!Status.ok())
				{
					Error(Inworld::Format("RunnableGenerateUserTokenRequest FALURE! %s, Code: %d", Status.error_message().c_str(), Status.error_code()));
					return;
				}

				OnGenerateUserToken(Response);
			}
		)
	);
}

void Inworld::StudioClient::OnGenerateUserToken(const InworldV1alpha::GenerateTokenUserResponse& Response)
{
	_InworldToken = Response.token().c_str();
	_StudioUserData.Workspaces.clear();

	Request(
		"RunnableListWorkspacesRequest",
		std::make_unique<Inworld::RunnableListWorkspacesRequest>(
			_InworldToken,
			_ServerUrl,
			[this](const grpc::Status& Status, const InworldV1alpha::ListWorkspacesResponse& Response)
			{
				if (!Status.ok())
				{
					Error(Inworld::Format("RunnableListWorkspacesRequest FALURE! %s, Code: %d", Status.error_message().c_str(), Status.error_code()));
					return;
				}

				OnWorkspacesReady(Response);
			}
		)
	);
}

static std::string CreateShortName(const std::string& Name)
{
	const int32_t Idx = Name.find_last_of('/');
	if (Idx != std::string::npos)
	{
		return Name.substr(Idx + 1, Name.size() - Idx + 1);
	}
	return Name;
}

void Inworld::StudioClient::OnWorkspacesReady(const InworldV1alpha::ListWorkspacesResponse& Response)
{
	_StudioUserData.Workspaces.reserve(Response.workspaces_size());

	for (int32_t i = 0; i < Response.workspaces_size(); i++)
	{
		const auto& GrpcWorkspace = Response.workspaces(i);
		auto& Workspace = _StudioUserData.Workspaces.emplace_back();
		Workspace.Name = GrpcWorkspace.name().data();
		Workspace.ShortName = CreateShortName(Workspace.Name);

		Request(
			"RunnableListScenesRequest",
			std::make_unique<Inworld::RunnableListScenesRequest>(
				_InworldToken,
				_ServerUrl,
				GrpcWorkspace.name(),
				[this, &Workspace](const grpc::Status& Status, const InworldV1alpha::ListScenesResponse& Response)
				{
					if (!Status.ok())
					{
						Error(Inworld::Format("RunnableListScenesRequest FALURE! %s, Code: %d", Status.error_message().c_str(), Status.error_code()));
						return;
					}

					OnScenesReady(Response, Workspace);
				}
			)
		);

		Request(
			"RunnableListCharactersRequest",
			std::make_unique<Inworld::RunnableListCharactersRequest>(
				_InworldToken,
				_ServerUrl,
				GrpcWorkspace.name(),
				[this, &Workspace](const grpc::Status& Status, const InworldV1alpha::ListCharactersResponse& Response)
				{
					if (!Status.ok())
					{
						Error(Inworld::Format("RunnableListCharactersRequest FALURE! %s, Code: %d", Status.error_message().c_str(), Status.error_code()));
						return;
					}

					OnCharactersReady(Response, Workspace);
				}
			)
		);

		Request(
			"RunnableListApiKeysRequest",
			std::make_unique<Inworld::RunnableListApiKeysRequest>(
				_InworldToken,
				_ServerUrl,
				GrpcWorkspace.name(),
				[this, &Workspace](const grpc::Status& Status, const InworldV1alpha::ListApiKeysResponse& Response)
				{
					if (!Status.ok())
					{
						Error(Inworld::Format("RunnableListCharactersRequest FALURE! %s, Code: %d", Status.error_message().c_str(), Status.error_code()));
						return;
					}

					OnApiKeysReady(Response, Workspace);
				}
			)
		);
	}
}

void Inworld::StudioClient::OnApiKeysReady(const InworldV1alpha::ListApiKeysResponse& Response, Inworld::StudioUserWorkspaceData& Workspace)
{
	Workspace.ApiKeys.reserve(Response.api_keys_size());

	for (int32_t i = 0; i < Response.api_keys_size(); i++)
	{
		const auto& GrpcApiKey = Response.api_keys(i);
		auto& ApiKey = Workspace.ApiKeys.emplace_back();
		ApiKey.Name = GrpcApiKey.name().data();
		ApiKey.Key = GrpcApiKey.key().data();
		ApiKey.Secret = GrpcApiKey.secret().data();
		ApiKey.IsActive = GrpcApiKey.state() == InworldV1alpha::ApiKey_State_ACTIVE;
	}

	CheckRequestsDone();
}

void Inworld::StudioClient::OnScenesReady(const InworldV1alpha::ListScenesResponse& Response, Inworld::StudioUserWorkspaceData& Workspace)
{
	Workspace.Scenes.reserve(Response.scenes_size());

	for (int32_t i = 0; i < Response.scenes_size(); i++)
	{
		const auto& GrpcScene = Response.scenes(i);
		auto& Scene = Workspace.Scenes.emplace_back();
		Scene.Name = GrpcScene.name().data();
		Scene.ShortName = CreateShortName(Scene.Name);
		Scene.Characters.reserve(GrpcScene.character_references_size());
		for (int32_t j = 0; j < GrpcScene.character_references_size(); j++)
		{
			Scene.Characters.emplace_back(GrpcScene.character_references(j).character().data());
		}
	}

	CheckRequestsDone();
}

void Inworld::StudioClient::OnCharactersReady(const InworldV1alpha::ListCharactersResponse& Response, Inworld::StudioUserWorkspaceData& Workspace)
{
	Workspace.Characters.reserve(Response.characters_size());

	for (int32_t i = 0; i < Response.characters_size(); i++)
	{
		const auto& GrpcCharacter = Response.characters(i);
		auto& Character = Workspace.Characters.emplace_back();
		Inworld::GrpcHelper::CharacterInfo CharInfo = Inworld::GrpcHelper::CreateCharacterInfo(GrpcCharacter);
		Character.Name = CharInfo._Name.c_str();
		Character.ShortName = CreateShortName(Character.Name);
		Character.RpmModelUri = CharInfo._RpmModelUri.c_str();
		Character.RpmImageUri = CharInfo._RpmImageUri.c_str();
		Character.RpmPortraitUri = CharInfo._RpmPortraitUri.c_str();
		Character.RpmPostureUri = CharInfo._RpmPostureUri.c_str();
		Character.bMale = CharInfo._bMale;
	}

	CheckRequestsDone();
}

void Inworld::StudioClient::CancelRequests()
{
	std::lock_guard<std::mutex> Lock(_RequestsMutex);

	_Requests.clear();
}

void Inworld::StudioClient::Request(const std::string& ThreadName, std::unique_ptr<Inworld::Runnable> Runnable)
{
	std::lock_guard<std::mutex> Lock(_RequestsMutex);

	auto& AsyncTask = _Requests.emplace_back(std::make_shared<AsyncRoutine>());
	AsyncTask->Start(ThreadName, std::move(Runnable));
}

void Inworld::StudioClient::CheckRequestsDone()
{
	std::lock_guard<std::mutex> Lock(_RequestsMutex);

	for (auto& R : _Requests)
	{
		if (!R->IsDone())
		{
			return;
		}
	}

	_Requests.clear();
	_Callback(true);
}

void Inworld::StudioClient::Error(std::string Message)
{
	Inworld::LogError("%s", Message.c_str());
	_ErrorMessage = Message;
	CancelRequests();
	_Callback(false);
}

void Inworld::StudioClient::ClearError()
{
	_ErrorMessage.clear();
}

static std::unique_ptr<Inworld::StudioClient> g_ClientPtr;

INWORLD_EXPORT void Inworld::CreateStudioClient()
{
	g_ClientPtr = std::make_unique<Inworld::StudioClient>();
}

INWORLD_EXPORT void Inworld::DestroyStudioClient()
{
	g_ClientPtr.reset();
}

INWORLD_EXPORT std::unique_ptr<Inworld::StudioClient>& Inworld::GetStudioClient()
{
	return g_ClientPtr;
}

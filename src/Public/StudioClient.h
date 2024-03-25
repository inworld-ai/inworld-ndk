/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <mutex>

#include "AsyncRoutine.h"
#include "Utils/SharedQueue.h"
#include "Define.h"

namespace ai { namespace inworld { namespace studio { namespace v1alpha
{
	class GenerateTokenUserResponse;
	class ListWorkspacesResponse;
	class ListApiKeysResponse;
	class ListScenesResponse;
	class ListCharactersResponse;
}}}}

namespace InworldV1alpha = ai::inworld::studio::v1alpha;


namespace Inworld
{
	struct INWORLD_EXPORT StudioTokenOptions
	{
		std::string ServerUrl;
		std::string ExchangeToken;
	};

	struct INWORLD_EXPORT StudioUserCharacterData
	{
		std::string Name;
		std::string ShortName;
		std::string RpmModelUri;
		std::string RpmImageUri;
		std::string RpmPortraitUri;
		std::string RpmPostureUri;
		bool bMale = false;

		mutable std::string RpmModelData;
	};

	struct INWORLD_EXPORT StudioUserSceneData
	{
		std::string Name;
		std::string ShortName;
		std::vector<std::string> Characters;
	};

	struct INWORLD_EXPORT StudioUserApiKeyData
	{
		std::string Name;
		std::string Key;
		std::string Secret;
		bool IsActive = false;
	};

	struct INWORLD_EXPORT StudioUserWorkspaceData
	{
		std::string Name;
		std::string ShortName;
		std::vector<StudioUserCharacterData> Characters;
		std::vector<StudioUserSceneData> Scenes;
		std::vector<StudioUserApiKeyData> ApiKeys;
	};

	struct INWORLD_EXPORT StudioUserData
	{
		std::vector<StudioUserWorkspaceData> Workspaces;
	};

	// use for studio client lifecycle
	class StudioClient;
	INWORLD_EXPORT void CreateStudioClient();
	INWORLD_EXPORT void DestroyStudioClient();
	INWORLD_EXPORT std::unique_ptr<StudioClient>& GetStudioClient();

	class INWORLD_EXPORT StudioClient final
	{
	public:
		StudioClient() = default;
		~StudioClient() { CancelRequests(); }
		
		// callback will be called not on main thread 
		void RequestStudioUserDataAsync(const std::string& Token, const std::string& ServerUrl, std::function<void(bool bSuccess)> Callback);

		void CancelRequests();
		bool IsRequestInProgress() const { return !_Requests.empty(); }

		const std::string& GetError() const { return _ErrorMessage; }
		const StudioUserData& GetStudioUserData() const { return _StudioUserData; }

	private:
		std::function<void(bool bSuccess)> _Callback;
		std::string _ServerUrl;
		std::string _InworldToken;
		StudioUserData _StudioUserData;

		std::mutex _RequestsMutex;
		std::vector<std::shared_ptr<AsyncRoutine>> _Requests;

		std::string _ErrorMessage;

	private:
		void Request(const std::string& ThreadName, std::unique_ptr<Inworld::Runnable> Runnable);
		void CheckRequestsDone();

		void OnGenerateUserToken(const InworldV1alpha::GenerateTokenUserResponse& Response);
		void OnWorkspacesReady(const InworldV1alpha::ListWorkspacesResponse& Response);
		void OnApiKeysReady(const InworldV1alpha::ListApiKeysResponse& Response, StudioUserWorkspaceData& Workspace);
		void OnScenesReady(const InworldV1alpha::ListScenesResponse& Response, StudioUserWorkspaceData& Workspace);
		void OnCharactersReady(const InworldV1alpha::ListCharactersResponse& Response, StudioUserWorkspaceData& Workspace);

		void Error(std::string Error);
		void ClearError();
	};
}
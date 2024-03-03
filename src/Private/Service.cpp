/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Service.h"
#include "Utils/Log.h"

#include <iomanip>
#include <random>
#include <sstream>
#include <thread>

void Inworld::RunnableRead::Run()
{
	while (!_HasReaderWriterFinished)
	{
		InworldPackets::InworldPacket IncomingPacket;
		if (!_ClientStream.Read(&IncomingPacket))
		{
			if (!_HasReaderWriterFinished)
			{
				_HasReaderWriterFinished = true;
				_ErrorCallback(_ClientStream.Finish());
			}

			_IsDone = true;

			return;
		}

		std::shared_ptr<Inworld::Packet> Packet;
		// Text event
		if (IncomingPacket.has_text())
		{
			Packet = std::make_shared<Inworld::TextEvent>(IncomingPacket);
		}
		else if (IncomingPacket.has_data_chunk())
		{
			// Audio response with Uncompressed 16-bit signed little-endian samples (Linear PCM) data.
			if (IncomingPacket.data_chunk().type() == ai::inworld::packets::DataChunk_DataType_AUDIO)
			{
				Packet = std::make_shared<Inworld::AudioDataEvent>(IncomingPacket);
			}
		}
		else if (IncomingPacket.has_control())
		{
			Packet = std::make_shared<Inworld::ControlEvent>(IncomingPacket);
		}
		else if (IncomingPacket.has_emotion())
		{
			Packet = std::make_shared<Inworld::EmotionEvent>(IncomingPacket);
		}
		else if (IncomingPacket.has_custom())
		{
			Packet = std::make_shared<Inworld::CustomEvent>(IncomingPacket);
		}
		else if (IncomingPacket.has_action())
		{
			Packet = std::make_shared<Inworld::ActionEvent>(IncomingPacket);
		}
		else if (IncomingPacket.has_debug_info())
		{
			Packet = std::make_shared<Inworld::RelationEvent>(IncomingPacket);
		}
		else if (IncomingPacket.has_session_control_response())
		{
			if (IncomingPacket.session_control_response().has_loaded_scene())
			{
				Packet = std::make_shared<Inworld::SessionControlResponse_LoadScene>(IncomingPacket);
			}
			else if (IncomingPacket.session_control_response().has_loaded_characters())
			{
				Packet = std::make_shared<Inworld::SessionControlResponse_LoadCharacters>(IncomingPacket);
			}
		}
		else
		{
			// Unknown packet type
			continue;
		}

		_Packets.PushBack(Packet);

		_ProcessedCallback(Packet);
	}

	_IsDone = true;
}

void Inworld::RunnableWrite::Run()
{
	while (!_HasReaderWriterFinished && !_Packets.IsEmpty())
	{
		auto Packet = _Packets.Front();
		InworldPackets::InworldPacket Event = Packet->ToProto();
		if (!_ClientStream.Write(Event))
		{
			if (!_HasReaderWriterFinished)
			{
				_HasReaderWriterFinished = true;
				_ErrorCallback(_ClientStream.Finish());
			}

			_IsDone = true;

			return;
		}

		_Packets.PopFront();

		_ProcessedCallback(Packet);
	}

	_IsDone = true;
}

#ifdef INWORLD_AUDIO_DUMP
void Inworld::RunnableAudioDumper::Run()
{
	AudioDumper.OnSessionStart(FileName);

	while (!_IsDone)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		std::string Chunk;
		while (AudioChuncks.PopFront(Chunk))
		{
			AudioDumper.OnMessage(Chunk);
		}
	}

	AudioDumper.OnSessionStop();
}
#endif

grpc::Status Inworld::RunnableGenerateSessionToken::RunProcess()
{
	InworldEngine::GenerateTokenRequest AuthRequest;
	AuthRequest.set_key(_ApiKey);
	if(!_Resource.empty())
	{
			AuthRequest.add_resources(_Resource);
	}

	auto& AuthCtx = UpdateContext({ {"authorization", GenerateHeader() } });

	return CreateStub()->GenerateToken(AuthCtx.get(), AuthRequest, &_Response);
}

std::unique_ptr<Inworld::ClientStream> Inworld::ServiceSession::OpenSession()
{
	return CreateStub()->OpenSession(Context().get());
}

std::unique_ptr<ClientContext>& Inworld::ServiceSession::Context()
{
	return UpdateContext({
				{ "authorization", std::string("Bearer ") + _Token },
				{ "session-id", _SessionId }
		});
}

std::string Inworld::RunnableGenerateSessionToken::GenerateHeader() const
{
	std::time_t Time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm Tm = *std::gmtime(&Time);
	std::stringstream Stream;
	Stream << std::put_time(&Tm, "%Y%m%d%H%M%S");
	const std::string CurrentTime = Stream.str();

	const std::string Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
	std::string Nonce = "00000000000";

	std::random_device Rd;
	std::mt19937 Gen(Rd());
	std::uniform_int_distribution<> Distr(0, Chars.size() - 1);

	for (int i = 0; i < Nonce.size(); i++)
	{
		Nonce[i] = Chars[Distr(Gen)];
	}

	const std::string Url = _ServerUrl.substr(0, _ServerUrl.find(":"));
	std::vector<std::string> CryptoArgs = {
		CurrentTime,
		Url,
		"ai.inworld.engine.WorldEngine/GenerateToken",
		Nonce,
		"iw1_request"
	};

	const std::string IniKey = std::string("IW1") + _ApiSecret;
	std::vector<uint8_t> Key;
	Key.resize(IniKey.size());
	memcpy(Key.data(), IniKey.data(), IniKey.size());

	for (const auto& Arg : CryptoArgs)
	{
		std::vector<uint8_t> Data;
		Data.resize(Arg.size());
		memcpy(Data.data(), Arg.data(), Arg.size());
		std::vector<uint8_t> NewKey(32);
		Inworld::Utils::HmacSha256(Data, Key, NewKey);
		Key = NewKey;
	}

	const std::string Signature = Inworld::Utils::ToHex(Key);

	return 
		std::string("IW1-HMAC-SHA256 ApiKey=") + _ApiKey + 
		",DateTime=" + CurrentTime +
		",Nonce=" + Nonce + 
		",Signature=" + Signature;
}

grpc::Status Inworld::RunnableGenerateUserTokenRequest::RunProcess()
{
	InworldV1alpha::GenerateTokenUserRequest Request;
	Request.set_type(InworldV1alpha::AuthType::AUTH_TYPE_FIREBASE);
	Request.set_token(_FirebaseToken);

	auto& Ctx = UpdateContext({
		{"x-authorization-bearer-type", "firebase"},
		{"authorization", std::string("Bearer ") + _FirebaseToken }
		});

	return CreateStub()->GenerateTokenUser(Ctx.get(), Request, &_Response);
}

grpc::Status Inworld::RunnableGetSessionState::RunProcess()
{
	InworldEngineV1::GetSessionStateRequest Request;
	Request.set_name(_SessionName);

	auto& Ctx = UpdateContext({
		{ "authorization", std::string("Bearer ") + _Token }
		});

	return CreateStub()->GetSessionState(Ctx.get(), Request, &_Response);
}


grpc::Status Inworld::RunnableListWorkspacesRequest::RunProcess()
{
	InworldV1alpha::ListWorkspacesRequest Request;

	auto& Ctx = UpdateContext({
		{"x-authorization-bearer-type", "inworld"},
		{"authorization", std::string("Bearer ") + _InworldToken }
		});

	return CreateStub()->ListWorkspaces(Ctx.get(), Request, &_Response);
}

grpc::Status Inworld::RunnableListScenesRequest::RunProcess()
{
	InworldV1alpha::ListScenesRequest Request;
	Request.set_parent(_Workspace);

	auto& Ctx = UpdateContext({
		{"x-authorization-bearer-type", "inworld"},
		{"authorization", std::string("Bearer ") + _InworldToken }
		});

	return CreateStub()->ListScenes(Ctx.get(), Request, &_Response);
}

grpc::Status Inworld::RunnableListCharactersRequest::RunProcess()
{
	InworldV1alpha::ListCharactersRequest Request;
	Request.set_parent(_Workspace);
	Request.set_view(InworldV1alpha::CharacterView::CHARACTER_VIEW_DEFAULT);

	auto& Ctx = UpdateContext({
		{"x-authorization-bearer-type", "inworld"},
		{"authorization", std::string("Bearer ") + _InworldToken }
		});

	return CreateStub()->ListCharacters(Ctx.get(), Request, &_Response);
}

grpc::Status Inworld::RunnableListApiKeysRequest::RunProcess()
{
	InworldV1alpha::ListApiKeysRequest Request;
	Request.set_parent(_Workspace);

	auto& Ctx = UpdateContext({
		{"x-authorization-bearer-type", "inworld"},
		{"authorization", std::string("Bearer ") + _InworldToken }
		});

	return CreateStub()->ListApiKeys(Ctx.get(), Request, &_Response);
}

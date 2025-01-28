/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Service.h"
#include "Client.h"
#include "Utils/Log.h"

#include <iomanip>
#include <random>
#include <sstream>
#include <thread>

void Inworld::RunnableRead::Run()
{
	while(!_HasReaderWriterFinished && !_IsDone)
	{
		InworldPackets::InworldPacket IncomingPacket;
		{
			std::unique_lock<std::mutex> lock(_Mutex);
			_ClientStream->Read(&IncomingPacket, &_ReadTag);
			_ReadWaiting = true;
			_ReadCV.wait(lock, [this]{ return !_ReadWaiting; });
		}

		std::shared_ptr<Inworld::Packet> Packet = nullptr;
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
			else if (IncomingPacket.data_chunk().type() == ai::inworld::packets::DataChunk_DataType_NVIDIA_A2F_ANIMATION_HEADER)
			{
				Packet = std::make_shared<Inworld::A2FHeaderEvent>(IncomingPacket);
			}
			else if (IncomingPacket.data_chunk().type() == ai::inworld::packets::DataChunk_DataType_NVIDIA_A2F_ANIMATION)
			{
				Packet = std::make_shared<Inworld::A2FContentEvent>(IncomingPacket);
			}
		}
		else if (IncomingPacket.has_control())
		{
			if (IncomingPacket.control().has_conversation_event())
			{
				Packet = std::make_shared<Inworld::ControlEventConversationUpdate>(IncomingPacket);
			}
			else if (IncomingPacket.control().has_current_scene_status())
			{
				Packet = std::make_shared<Inworld::ControlEventCurrentSceneStatus>(IncomingPacket);
			}
			else
			{
				Packet = std::make_shared<Inworld::ControlEvent>(IncomingPacket);
			}
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
		else if (IncomingPacket.has_latency_report())
		{
			if (IncomingPacket.latency_report().has_ping_pong())
			{
				Packet = std::make_shared<Inworld::PingEvent>(IncomingPacket);
			}
		}
		else if (IncomingPacket.has_log())
		{
			Packet = std::make_shared<Inworld::LogEvent>(IncomingPacket);
		}

		if(Packet != nullptr)
		{
			_Packets.PushBack(Packet);
		}

		if (!_HasReaderWriterFinished)
		{
			_ProcessedCallback();
		}
	}
	_IsDone = true;
}

void Inworld::RunnableWrite::Run()
{
	while(!_HasReaderWriterFinished)
	{
		if(_Packets.IsEmpty())
		{
			continue;
		}

		{
			std::unique_lock<std::mutex> lock(_Mutex);
			_ClientStream->Write(_Packets.Front()->ToProto(), &_WriteTag);
			_WriteWaiting = true;
			_WriteCV.wait(lock, [this] { return !_WriteWaiting; });
		}
	}
	_IsDone = true;
}

void Inworld::RunnableRpcHandler::Run()
{
	while(true)
	{
		if(!_IsRunning && !_HasReaderWriterFinished && !_ShuttingDown)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}
		
		void* outTag;
		bool ok;
		auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(5);

		switch (_CompletionQueue->AsyncNext(&outTag, &ok, deadline))
		{
		case grpc::CompletionQueue::GOT_EVENT:
			switch(*static_cast<TagType*>(outTag))
			{
				case TagType::Write:
					if(ok)
					{
						_Packets.PopFront();
					} else
					{
						EndStream();
					}

					{
						std::lock_guard<std::mutex> lock(_Mutex);
						_WriteWaiting = false;
						_WriteCV.notify_one();
					}
					break;
				case TagType::Read:
					if(!ok)
					{
						EndStream();
					}
				
					{
						std::lock_guard<std::mutex> lock(_Mutex);
						_ReadWaiting = false;
						_ReadCV.notify_one();
					}
					break;
				case TagType::CloseSession:
					if (_HasReaderWriterFinished && !_ShuttingDown)
					{
						_ShuttingDown = true;
						_CompletionQueue->Shutdown();
					}
					break;
				case TagType::OpenSession:
					if(_OpeningClientStream)
					{
						_OpeningClientStream = false;
						_SessionOpenCallback();
					}
					break;
				}
			break;
		case grpc::CompletionQueue::TIMEOUT:
			{
				if(_OpeningClientStream)
				{
					_OpeningClientStream = false;
					_Status = grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED, "Failed to open stream - deadline expired.");
				
					_EndCallback(_Status);
					_IsDone = true;
					return;
				}
				continue;
			}
		case grpc::CompletionQueue::SHUTDOWN:
			_EndCallback(_Status);
			_IsDone = true;
			return;
		}
	}
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
}

void Inworld::RunnableAudioDumper::Deinitialize()
{
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

std::unique_ptr<Inworld::ClientStream> Inworld::ServiceSession::OpenSessionAsync(const ClientHeaderData& Metadata)
{
	if(_CompletionQueue)
	{
		_CompletionQueue.reset();
	}
	_CompletionQueue = std::make_unique<grpc::CompletionQueue>();
	std::unique_ptr<ClientContext>& ClientContext = Context();
	for (const auto& Data : Metadata)
	{
		ClientContext->AddMetadata(Data.first, Data.second);
	}
	return CreateStub()->AsyncOpenSession(ClientContext.get(), _CompletionQueue.get(), &_OpenSessionTag);
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

grpc::Status Inworld::RunnableGetSessionState::RunProcess()
{
	InworldEngineV1::GetSessionStateRequest Request;
	Request.set_name(_SessionName);

	auto& Ctx = UpdateContext({
		{ "authorization", std::string("Bearer ") + _Token }
		});

	return CreateStub()->GetSessionState(Ctx.get(), Request, &_Response);
}

grpc::Status Inworld::RunnableCreateInteractionFeedback::RunProcess()
{
	InworldEngineV1::CreateInteractionFeedbackRequest FeedbackRequest;
	FeedbackRequest.set_parent(_ResourceId);

	auto InteractionFeedback = FeedbackRequest.mutable_interaction_feedback();
	InteractionFeedback->set_is_like(_Feedback.bIsLike);

	for(const InteractionFeedback::DislikeType& DislikeType : _Feedback.DislikeReasons)
	{
		InteractionFeedback->add_type(static_cast<InworldEngineV1::InteractionDislikeType>(DislikeType));
	}

	InteractionFeedback->set_comment(_Feedback.comment);

	auto& Ctx = UpdateContext({
				{ "authorization", std::string("Bearer ") + _Token },
				{ "session-id", _SessionId}
		});

	return CreateStub()->CreateInteractionFeedback(Ctx.get(), FeedbackRequest, &_Response);
}

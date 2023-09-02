#pragma once
#include "Client.h"

struct ClientOptions : Inworld::ClientOptions
{
	std::string ClientID;
	std::string ClientVersion;
};

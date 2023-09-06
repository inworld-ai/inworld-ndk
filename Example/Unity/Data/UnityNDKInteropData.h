#pragma once
#include "RunnableCommand.h"
#include "comutil.h"
#include "Utils/Log.h"

#if __cplusplus
extern "C"
{
#endif
	
namespace NDKUnity
{
	inline BSTR StringToBSTR(std::string rhs)
	{
		_bstr_t tmp = _bstr_t(rhs.c_str());
		return tmp.copy();
	}
	
	using OnTokenGenerated = void (*)();
	using OnSceneLoaded = void (*)(std::vector<Inworld::AgentInfo>);


	struct Capabilities
	{
		// YAN: The minimum size of C# is int. Do not use Char or Bool.
		int Text ;
		int Audio;
		int Emotions ;
		int Interruptions;
		int Triggers;
		int PhonemeInfo;
		int TurnBasedSTT;
		int NarratedActions;
		// YAN: No need to set others.
		
		Inworld::CapabilitySet ToNDK()
		{
			Inworld::CapabilitySet set;
			set.Text = Text;
			set.Audio = Audio;
			set.Emotions = Emotions;
			set.Interruptions = Interruptions;
			set.Triggers = Triggers;
			set.PhonemeInfo = PhonemeInfo;
			set.TurnBasedSTT = TurnBasedSTT;
			set.NarratedActions = NarratedActions;
			return set;
		}
	};

	struct AgentInfo
	{
		BSTR BrainName;
		BSTR AgentId;
		BSTR GivenName;

		AgentInfo()
		{
			BrainName = SysAllocString(L"__DUMMY__");
			AgentId = SysAllocString(L"__DUMMY__");
			GivenName = SysAllocString(L"__DUMMY__");
		}

		explicit AgentInfo(const Inworld::AgentInfo& rhs)
		{
			BrainName = StringToBSTR(rhs.BrainName);
			AgentId = StringToBSTR(rhs.AgentId);
			GivenName = StringToBSTR(rhs.GivenName);
			Inworld::Log("ID: {0} BrainName {1}", rhs.AgentId, rhs.BrainName);
		}
	};
	
}

#if __cplusplus
}
#endif
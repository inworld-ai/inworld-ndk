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
	// YAN: BSTR is the string format used in COM as well as Unity Communication.
	//		According to https://learn.microsoft.com/zh-cn/cpp/atl-mfc-shared/allocating-and-releasing-memory-for-a-bstr?view=msvc-170
	//		Unity will handle GC for BSTR itself.		
	inline BSTR StringToBSTR(std::string rhs)
	{
		return _com_util::ConvertStringToBSTR(rhs.c_str());
		_bstr_t tmp = _bstr_t(rhs.c_str());
		return tmp.copy();
	}	
	using UnityCallback = void (*)();

	struct Capabilities
	{
		// YAN:
		// The minimum size (char/bool) of C# in C++ is int by default.
		// Changing memory alignment (#pragma pack) on either side will lose efficiency.		
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

	struct SessionInfo
	{
		BSTR SessionId;
		BSTR Token;
		BSTR SessionSavedState;
		int64_t ExpirationTime;
		int IsValid;

		SessionInfo()
		{
			SessionId = SysAllocString(L"__DUMMY__");
			Token = SysAllocString(L"__DUMMY__");
			SessionSavedState = SysAllocString(L"__DUMMY__");
			ExpirationTime = 0;
			IsValid = false;
		}

		explicit SessionInfo(const Inworld::SessionInfo& rhs)
		{
			SessionId = StringToBSTR(rhs.SessionId);
			Token = StringToBSTR(rhs.Token);
			SessionSavedState = StringToBSTR(rhs.SessionSavedState);
			ExpirationTime = rhs.ExpirationTime;
			IsValid = rhs.IsValid();
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
		}
	};
	
}

#if __cplusplus
}
#endif
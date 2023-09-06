#pragma once
#include "RunnableCommand.h"

namespace NDKUnity
{
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
}


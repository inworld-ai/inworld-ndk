/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "InworldVAD.h"
#include <iostream>

int main(int argc, char* argv[])
{
	Inworld::VAD_Initialize("model");
	std::vector<float> AudioData(300000, 0.0f);
	const float Res = Inworld::VAD_Process(AudioData);
	std::cout << "VAD Result: " << Res << std::endl;

	return 0;
}
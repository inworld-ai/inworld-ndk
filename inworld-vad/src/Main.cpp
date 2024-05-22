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
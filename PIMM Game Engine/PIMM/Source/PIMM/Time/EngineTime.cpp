#include <PIMM/Time/EngineTime.h>
#include <PIMM/Core/Logger.h>

pimm::EngineTime* pimm::EngineTime::sharedInstance = nullptr;

void pimm::EngineTime::Initialize()
{
	sharedInstance = new EngineTime();
}

pimm::EngineTime::EngineTime()
{
}

void pimm::EngineTime::LogFrameStart()
{
	//if (!sharedInstance) sharedInstance = new EngineTime();

	sharedInstance->timeStart = std::chrono::system_clock::now();
}

void pimm::EngineTime::LogFrameEnd()
{
	//if (!sharedInstance) sharedInstance = new EngineTime();

	sharedInstance->timeEnd = std::chrono::system_clock::now();
	std::chrono::duration<d64> elapsedSeconds = sharedInstance->timeEnd - sharedInstance->timeStart;

	sharedInstance->deltaTime = elapsedSeconds.count();

	//std::cout << "Frame update finished in: " << sharedInstance->deltaTime << " seconds\n";
}

pimm::d64 pimm::EngineTime::GetDeltaTime()
{
	return sharedInstance->deltaTime;
}

pimm::EngineTime::~EngineTime()
{
}

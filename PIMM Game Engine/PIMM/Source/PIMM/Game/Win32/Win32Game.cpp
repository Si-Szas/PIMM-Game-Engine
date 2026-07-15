#include <PIMM/Game/Game.h>
#include <PIMM/Time/EngineTime.h>
#include <Windows.h>

void pimm::Game::Run()
{
	OnCreate();

	MSG msg{};
	pimm::EngineTime::Initialize();

	while (m_isRunning) {

		//Log the start of the time
		pimm::EngineTime::LogFrameStart();

		//Rendering
		OnInternalUpdate(pimm::EngineTime::GetDeltaTime());

		//Process messages. Removes next message from the queue 
		//Once queue is empty, while loop terminates

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{ 
			//If the message is to quit, the game loop is set to false and program terminates
			if (msg.message == WM_QUIT)
			{
				m_isRunning = false;
				break;
			}

			//Each of the message is translated and dispatched to the window procedure
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//Log the End of the time
		pimm::EngineTime::LogFrameEnd();
	}
}

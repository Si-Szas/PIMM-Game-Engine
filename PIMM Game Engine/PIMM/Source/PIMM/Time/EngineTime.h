#pragma once
#include <PIMM/Core/Core.h>
#include <iostream>
#include <chrono>
#include <ctime>

namespace pimm
{
	class EngineTime
	{
		public:
			static void Initialize();
			static d64 GetDeltaTime();

		private:
			EngineTime();
			~EngineTime();
			EngineTime& operator = (const EngineTime&) {};
			EngineTime& operator = (EngineTime&&) {};

			static void LogFrameStart();
			static void LogFrameEnd();

		private:
			static EngineTime* sharedInstance;

			std::chrono::system_clock::time_point timeStart;
			std::chrono::system_clock::time_point timeEnd;

			d64 deltaTime = 0.0;

			friend class Game;

		//public:
		//	//CONSTRUCTOR
		//	EngineTime();
		//
		//	//FUNCTIONS
		//	static void LogFrameStart();
		//	static void LogFrameEnd();
		//
		//	//GETTER
		//	d64 GetDeltaTime();
		//
		//	//DESTRUCTOR
		//	~EngineTime();
		//
		//private:
		//	//Disables copy constructor, so avoids creating another copy of EngineTime
		//	EngineTime(const EngineTime&) = delete;
		//	//Makes sure it is not moveable
		//	EngineTime(EngineTime&&) = delete;
		//	//Ensures it cannot be assigned to another value (disables assignment copy operator)
		//	EngineTime& operator = (const EngineTime&) = delete;
		//	//Also disables move assignment operator
		//	EngineTime& operator = (EngineTime&&) = delete;
		//
		//private:
		//	std::chrono::system_clock::time_point timeStart;
		//	std::chrono::system_clock::time_point timeEnd;
		//
		//	d64 deltaTime = 0.0;
		//
		//friend class Window;
	};
}

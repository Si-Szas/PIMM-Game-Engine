#pragma once

#include <PIMM/Core/Core.h>
#include <format>
#include <vector>

namespace pimm{
	class Logger final
	{
		pimm_disable_copy_and_move(Logger)
		public:
			//ENUMS
			enum class LogLevel {
				Error = 0,
				Warning,
				Information
			};

			//CONSTRUCTOR
			//Explicit has single argument constructors
			explicit Logger(LogLevel logLevel = LogLevel::Error);

			//Log Method
			//const tells function that the function doesn't alter state of the class
			template<typename... Args>
			void Log(LogLevel level, std::format_string<Args...> fmt, Args&&... args)
			{
				auto str = std::format(fmt, std::forward<Args>(args)...);
				_Log(level, str.c_str());
				debugLogLevel = level;
				std::string logStr = convertDebugLogLevelToString(level) + str;
				logMessages.push_back(logStr);
			}

			std::vector<std::string> getLogMessages();
			LogLevel getDebugLogLevel();

			//DESTRUCTOR
			~Logger();

		private:
			void _Log(LogLevel level, const char* message) const;
			std::string convertDebugLogLevelToString(LogLevel level);

		private:
			LogLevel m_logLevel = LogLevel::Error;
			LogLevel debugLogLevel;

			// A vector of log messages for printing in the debug panel
			std::vector<std::string> logMessages;

	
	};
}

#define PIMMLog(logger, type, message, ...)\
	logger.Log((type), {message} __VA_OPT__(,) __VA_ARGS__);

#define PIMMLogThrow(logger, exception, type, message, ...)\
	{\
		PIMMLog(logger,type,message, __VA_ARGS__);\
		throw exception(message);\
	}

//Macro to log error messages
#define PIMMLogInformation(message, ...)\
	PIMMLog(GetLogger(), Logger::LogLevel::Information, message, __VA_ARGS__)

#define PIMMLogWarning(message, ...)\
	PIMMLog(GetLogger(), Logger::LogLevel::Warning, message, __VA_ARGS__)

//Place enumeration in () to avoid errors
#define PIMMLogError(message, ...)\
	PIMMLog(GetLogger(), Logger::LogLevel::Error, message, __VA_ARGS__)

//Create a macro using the defined preprocesser directive
//Tells compiler to replace all instances of a specific name of a value before compilation	
#define PIMMLogThrowError(message, ...)\
	PIMMLogThrow(GetLogger(), std::runtime_error, Logger::LogLevel::Error, message, __VA_ARGS__)

//Throw an invalid argument exception (not run-time error)
#define PIMMLogThrowInvalidArgument(message, ...)\
	PIMMLogThrow(GetLogger(), std::invalid_argument, Logger::LogLevel::Error, message, __VA_ARGS__)
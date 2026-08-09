#include <PIMM/Core/Logger.h>
#include <iostream>
#include <string>

pimm::Logger::Logger(LogLevel logLevel): m_logLevel(logLevel)
{
}

void pimm::Logger::_Log(LogLevel level, const char* message) const
{
	//Lambda function to convert log level to string
	auto logLevelToString = [](LogLevel level) {
		switch (level) {
			case LogLevel::Error: return "LOG ERROR";
			case LogLevel::Warning: return "LOG Warning";
			case LogLevel::Information: return "LOG Information";
			default: return "Unknown";
		}
	};

	//If level is set to error, print all error messages
	//Information should get all types of messages
	if (level > m_logLevel) return;
	std::clog << "[PIMM " << logLevelToString(level) << "]: " << message << "\n";
}

std::string pimm::Logger::convertDebugLogLevelToString(LogLevel level)
{
	switch (level)
	{
	case LogLevel::Error: return "[PIMM Engine] ERROR : ";
	case LogLevel::Warning: return "[PIMM Engine] WARNING : ";
	case LogLevel::Information: return "[PIMM Engine] INFO : ";
	default: return "[PIMM Engine] UNKNOWN : ";
	}
}

std::vector<std::string> pimm::Logger::getLogMessages()
{
	return logMessages;
}

pimm::Logger::LogLevel pimm::Logger::getDebugLogLevel()
{
	return debugLogLevel;
}

pimm::Logger::~Logger()
{
}

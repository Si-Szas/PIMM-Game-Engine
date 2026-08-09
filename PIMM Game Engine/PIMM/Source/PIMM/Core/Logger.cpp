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

std::vector<std::string> pimm::Logger::getLogMessages()
{
	return logMessages;
}

pimm::Logger::~Logger()
{
}

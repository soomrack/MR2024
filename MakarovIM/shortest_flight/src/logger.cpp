#include "logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

Logger& Logger::get_instance()
{
	static Logger instance;
	return instance;
}


void Logger::initialize(const std::string& filename)
{
	if (!filename.empty()) {
		log_file.open(filename, std::ios::app);
		use_file = true;
	}
}


Logger::~Logger()
{
	if (log_file.is_open()) {
		log_file.close();
	}
}


const char* Logger::level_to_string(LogLevel level) 
{
	switch (level)
	{
	case LogLevel::DEBUG: return "DEBUG";
	case LogLevel::INFO: return "INFO";
	case LogLevel::WARNING: return "WARNING";
	case LogLevel::ERROR: return "ERROR";
	default: return "INKNOWN";
	}
}


void Logger::log(LogLevel level, const std::string& module, const std::string& message) {
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);

	std::ostringstream ss;
	ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
		<< " [" << level_to_string(level) << "] "
		<< "[" << module << "] "
		<< message;

	const std::string log_message = ss.str();

	if (use_file && log_file.is_open()) {
		log_file << log_message << std::endl;
	}
	std::cerr << log_message << std::endl;
}

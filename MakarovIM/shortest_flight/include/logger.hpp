#pragma once
#include <string>
#include <fstream>


enum class LogLevel { DEBUG, INFO, WARNING, ERROR };


class Logger {
public:
	static Logger& get_instance();
	void initialize(const std::string& filename = "");
	void log(LogLevel level, const std::string& module, const std::string& message);

private:
	Logger() = default;
	~Logger();

	std::ofstream log_file;
	bool use_file = false;

	const char* level_to_string(LogLevel level);
};
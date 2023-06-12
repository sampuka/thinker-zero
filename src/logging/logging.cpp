#include "logging.hpp"

#include <cstdarg>
#include <fstream>
#include <mutex>

constexpr size_t max_log_length = 1000;

std::string log_filepath;
std::ofstream log_file;

std::mutex log_mutex;

void log(const std::string& log_string)
{
	std::lock_guard lock(log_mutex);

	if (!log_file.is_open())
	{
		if (log_filepath == "")
		{
			return;
		}

		log_file.open(log_filepath);
	}

	log_file << log_string;

	log_file.flush();
}

void log_msg(const char* format, ...)
{
	char msg[max_log_length];

	va_list args;
	va_start(args, format);

	std::vsnprintf(msg, sizeof msg, format, args);

	log(std::string{"Log: "} + msg + std::string{"\n\n"});
}

void log_error(const char* format, ...)
{
	char msg[max_log_length];

	va_list args;
	va_start(args, format);

	std::vsnprintf(msg, sizeof msg, format, args);

	log(std::string{"Error: "} + msg + std::string{"\n\n"});
}

void set_log_filepath(const std::string& filepath)
{
	std::lock_guard lock(log_mutex);

	if (log_filepath == filepath)
	{
		return;
	}

	if (log_file.is_open())
	{
		log_file.close();
	}

	log_filepath = filepath;
}
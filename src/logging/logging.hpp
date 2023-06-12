#ifndef LOGGING_LOGGING_HPP
#define LOGGING_LOGGING_HPP

#include <string>

void log_msg(const char* format, ...);
void log_error(const char* format, ...);

void set_log_filepath(const std::string& filepath);

#endif  // LOGGING_LOGGING_HPP

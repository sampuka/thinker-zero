#include "console_parsing.hpp"

#include "console/position_printer.hpp"
#include "console/uci_input.hpp"
#include "engine/Engine.hpp"
#include "logging/logging.hpp"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

enum class EngineInterface
{
	NONE,
	UCI
};

std::unique_ptr<std::jthread> console_interface_thread = nullptr;
bool console_interface_running = true;

EngineInterface selected_interface = EngineInterface::UCI;  // Assume UCI for now

void parse_uci_command(const std::string& command, const std::vector<std::string>& args)
{
	if (command == "isready")
	{
		uci_isready();
	}
	else if (command == "ucinewgame")
	{
		uci_ucinewgame();
	}
	else if (command == "position")
	{
		if (args.size() >= 1)
		{
			uci_position(args);
		}
		else
		{
			log_error("Position command without arguments");
			std::printf("Command 'position' requires FEN string\n");
		}
	}
	else if (command == "go")
	{
		uci_go(args);
	}
	else
	{
		std::printf("Unknown command: '%s'\n", command.c_str());
		log_error("Unknown command: '%s'", command.c_str());
	}
}

void parse_command(const std::string& command, const std::vector<std::string>& args)
{
	if (command == "")
	{
		// Just do nothing for empty input
	}
	else if (command == "help" || command == "?")
	{
		std::printf(
			"The program intends to support the UCI standard\n"
			"Available commands (UCI omitted):\n\n"
			"quit\n"
			"  Quits application\n\n");
	}
	else if (command == "quit")
	{
		console_interface_running = false;
		engine.shutdown();
	}
	else if (command == "print")
	{
		std::string position_string = format_position_to_string(engine.get_position());

		std::printf("Current position:\n%s", position_string.c_str());
	}
	else if (command == "uci")
	{
		selected_interface = EngineInterface::UCI;
		uci_start();
	}
	else
	{
		switch (selected_interface)
		{
			case EngineInterface::UCI:
			{
				parse_uci_command(command, args);
				break;
			}

			default:
			{
				log_error("Unknown command for selected interface");
				std::printf("Unknown command for selected interface\n");
			}
		}
	}
}

std::tuple<std::string, std::vector<std::string>> parse_input(const std::string& input_line)
{
	std::string command;
	std::vector<std::string> args;

	std::stringstream ss(input_line);
	std::string token;

	std::getline(ss, command, ' ');

	while (std::getline(ss, token, ' '))
	{
		args.push_back(token);
	}

	return {command, args};
}

void console_interface_thread_loop()
{
	std::printf("Thinker-zero Chess Engine by Mathias Ebbensgaard Jensen\n");

	while (console_interface_running)
	{
		std::string input_line;
		std::getline(std::cin, input_line);

		log_msg("> %s", input_line.c_str());

		auto [command, args] = parse_input(input_line);

		parse_command(command, args);
	}
}

void start_console_interface_thread()
{
	console_interface_thread = std::make_unique<std::jthread>(console_interface_thread_loop);
}

void end_console_interface_thread()
{
	console_interface_thread = nullptr;
}

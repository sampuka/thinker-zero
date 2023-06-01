#include "uci_interface.hpp"

#include "engine/Engine.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <thread>

std::unique_ptr<std::jthread> uci_interface_thread = nullptr;
bool is_running = true;

void parse_input(std::string& input_line)
{
    if (input_line == "quit")
    {
        is_running = false;
        engine.shutdown();
        std::printf("Quitting...\n");
    }
    else
    {
        std::printf("You wrote \"%s\"\n", input_line.c_str());
    }
}

void uci_interface_thread_loop()
{
    while (is_running)
    {
        std::printf("> ");
        
        std::string input_line;
        std::getline(std::cin, input_line);

        parse_input(input_line);
    }
}

void start_uci_interface_thread()
{
    uci_interface_thread = std::make_unique<std::jthread>(uci_interface_thread_loop);
}

void end_uci_interface_thread()
{
    uci_interface_thread = nullptr;
}

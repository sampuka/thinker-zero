#ifndef CONSOLE_UCI_INPUT_HPP
#define CONSOLE_UCI_INPUT_HPP

#include <string>
#include <vector>

void uci_start();

void uci_isready();

void uci_ucinewgame();

void uci_position(const std::vector<std::string>& args);

void uci_go(const std::vector<std::string>& args);

void uci_setoption(const std::vector<std::string>& args);

#endif  // CONSOLE_UCI_INPUT_HPP

#include "engine/Engine.hpp"
#include "logging/logging.hpp"
#include "console/console_parsing.hpp"

int main()
{
    set_log_filepath("thinker-zero-log.txt");

    start_console_interface_thread();

    engine.main_loop(); // Will loop here until application ends

    end_console_interface_thread();

    return 0;
}
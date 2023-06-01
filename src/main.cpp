#include "engine/Engine.hpp"
#include "uci/uci_interface.hpp"

#include <cstdio>

int main()
{
    std::printf("Hello India!\n");

    start_uci_interface_thread();

    engine.main_loop(); // Will loop here until application ends

    end_uci_interface_thread();
    
    std::printf("Goodbye India!\n");

    return 0;
}
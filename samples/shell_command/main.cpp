// This sample demonstrates the use of shell commands (subcommands). It's a simple application
// with two commands, read and write, which each have their own set of arguments.
#include <iostream>
#include <filesystem>
#include <ookii/shell_command.h>
#include "read_command.h"
#include "write_command.h"

int main(int argc, char *argv[])
{
    std::string name;
    if (argc > 0) 
    {
        name = std::filesystem::path{argv[0]}.filename().string(); 
    }

    // Register the available commands.
    ookii::shell_command_manager manager{name};
    manager.add_command<read_command>()
        .add_command<write_command>();

    // Find and run the command based on the command line arguments, and print error messages and
    // usage help as appropriate.
    return manager.run_command(argc, argv);
}

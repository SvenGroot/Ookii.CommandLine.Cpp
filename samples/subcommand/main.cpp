// This sample demonstrates the use of subcommands. It's a simple application with two commands,
// read and write, which each have their own set of arguments.
#include <iostream>
#include <filesystem>
#include <ookii/command_line.h>
#include "read_command.h"
#include "write_command.h"

int main(int argc, char *argv[])
{
    auto name = ookii::command_line_parser::get_executable_name(argc, argv);
    ookii::command_manager manager{name};

    // Register the available commands.
    manager.description("Subcommand sample for Ookii.CommandLine.")
        .common_help_argument("-Help")
        .add_command<read_command>()
        .add_command<write_command>()
        .add_version_command([]() { std::cout << "Ookii.CommandLine Subcommand Sample 2.0" << std::endl; });

    // Find and run the command based on the command line arguments, and print error messages and
    // usage help as appropriate.
    // If creating the command failed, we return an error exit code (1 in this case).
    return manager.run_command(argc, argv).value_or(1);
}

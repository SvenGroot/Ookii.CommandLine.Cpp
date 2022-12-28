#include <iostream>
#include <filesystem>
#include <ookii/command_line.h>
#include "read_command.h"
#include "write_command.h"

int main(int argc, char *argv[])
{
    // Create a command_manager, whose constructor argument is the application executable name, used
    // in the usage help.
    auto name = ookii::command_line_parser::get_executable_name(argc, argv);
    ookii::command_manager manager{name};

    manager
        // Set an application description that will be shown before the command list usage help.
        .description("Subcommand sample for Ookii.CommandLine.")
        // Tell the command manager that all commands have a "-Help" argument, so it can print an
        // instruction to the user on how to get help for a command at the bottom of the command
        // list usage help.
        .common_help_argument("-Help")
        // Add a version command that runs the specified lambda when invoked.
        .add_version_command([]() { std::cout << "Ookii.CommandLine Subcommand Sample 2.0" << std::endl; })
        // Add the commands defined by this aplication.
        .add_command<read_command>()
        .add_command<write_command>();

    // Find and run the command based on the command line arguments, and print error messages and
    // usage help as appropriate.
    // If creating the command failed, we return an error exit code (1 in this case).
    return manager.run_command(argc, argv).value_or(1);
}

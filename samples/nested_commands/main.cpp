#include <ookii/command_line.h>
#include "course_commands.h"
#include "student_commands.h"
#include "list_command.h"

// Although this sample uses the code New-Subcommand.ps1 script, we can't generate a main() function,
// nor can we call the generated register_commands function, since we only want to register some of
// the commands.
int main(int argc, char *argv[])
{
    ookii::command_manager manager{ookii::command_line_parser::get_executable_name(argc, argv)};
    manager
        .common_help_argument("-Help")
        .description("Nested subcommands sample for Ookii.CommandLine.")
        .add_version_command([]() { std::cout << "Ookii.CommandLine Nested Commands Sample 2.0" << std::endl; })
        .add_command<course_command>()
        .add_command<student_command>()
        .add_command<list_command>();

    return manager.run_command(argc, argv).value_or(1);
}

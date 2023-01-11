#include <ookii/command_line_generated.h>
#include "course_commands.h"
#include "student_commands.h"
#include "list_command.h"

// Although this sample uses the code New-Subcommand.ps1 script, we can't generate a main() function,
// because we want to add the non-generated parent commands.
int main(int argc, char *argv[])
{
    // This will set the options from the [global] block, and only register the list command.
    auto manager = ookii::register_commands(ookii::command_line_parser::get_executable_name(argc, argv));

    // Add non-generated top-level commands.
    manager
        .add_command<course_command>()
        .add_command<student_command>();

    return manager.run_command(argc, argv).value_or(1);
}

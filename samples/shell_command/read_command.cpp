#include <iostream>
#include <fstream>
#include "read_command.h"

// Create the command line arguments for the command. The builder will have been initialized with
// the name and description of the command, as well as the locale and case sensitivity settings
// used by the shell_command_manager. If we want to override any settings, that can be done here
// too.
read_command::read_command(read_command::builder_type &builder)
{
    builder.add_argument(_file_name, "FileName").positional().required().description("The name of the file to read.")
        .add_argument(_max_lines, "MaxLines").description("The maximum number of lines to read.");
}

// Run the command, which reads from the specified file.
int read_command::run()
{
    std::ifstream file{_file_name};
    if (!file)
    {
        std::cerr << "Failed to open file " << _file_name << std::endl;
        return 1;
    }

    std::string line;
    for (int count = 0;
        std::getline(file, line) && (!_max_lines || count < *_max_lines);
        ++count)
    {
        std::cout << line << std::endl;
    }

    return 0;
}